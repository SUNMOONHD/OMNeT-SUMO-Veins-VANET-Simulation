#include "veins/modules/application/SVMD/SVMDDataApp.h"
#include "veins/modules/application/SVMD/SVMDformerMessage_m.h"
#include <fstream>

using namespace veins;

Define_Module(veins::SVMDDataApp);

void SVMDDataApp::initialize(int stage)
{
    // 调用父类初始化函数
    DemoBaseApplLayer::initialize(stage);
    if (stage == 0) { // 初始化阶段0，读取参数并设置初始状态
        /// 读取基础参数（消息广播周期、恶意节点比例、伪ID有效期、攻击类型）
        beaconInterval = par("beaconInterval");
        datasetSeed = par("datasetSeed").intValue();
        misbehaviorRatio = par("misbehaviorRatio").doubleValue();
        pseudonymLifetime = par("pseudonymLifetime");
        attackType = par("misbehaviorType").intValue();

        // 判定恶意节点
        isMalicious = uniform(0.0, 1.0, 0) < misbehaviorRatio;

        /// 初始化运行变量（车辆ID、当前伪ID、上一次发送时间、上一次速度值、基础消息发送自消息、伪ID更换自消息）
        int nodeId = getParentModule()->getIndex();
        currentPseudonym = generatePseudonym();
        msgSerial = 0;
        lastSendTime = simTime();
        lastSpeedVal = 0.0;
        sendBeaconEvt = new cMessage("sendBeaconEvt");
        pseudonymChangeMsg = new cMessage("pseudonym_change");

        /// 数据集写入
        char filename[256];
        if (isMalicious) {
            snprintf(filename, sizeof(filename),
                "datasets/abnormalDataset_attackType%02d/vehicle_abnormal%03d_messages.csv",
                attackType, nodeId);
        }
        else {
            snprintf(filename, sizeof(filename),
                "datasets/normalDataset/vehicle_normal%03d_messages_batch%02d.csv",
                nodeId, attackType);
        }

        datasetFile.open(filename, std::ios::out | std::ios::app);
        if (!datasetFile.is_open()) {
            EV_ERROR << "Cannot open " << filename << endl;
            throw cRuntimeError("Failed to open datasets file for node %d", nodeId);
        }

        datasetFile << "timestamp, PseudoID,pos_x,pos_y,spd_x,spd_y,"
                       "acc_x,acc_y,hed_x,hed_y\n";

        // 可视化标记
        findHost()->getDisplayString().setTagArg("i", 1, isMalicious ? "red" : "white");

        // 调度首次发送BSM和伪ID更换
        if (!isMalicious) {
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            scheduleAt(simTime() + pseudonymLifetime, pseudonymChangeMsg);
        }
    }

    /// 初始化阶段1，恶意节点初始化攻击参数
    if (stage == 1 && isMalicious) {
        initAttack();
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        scheduleAt(simTime() + pseudonymLifetime, pseudonymChangeMsg);
    }
}

/// @brief 初始化恶意节点攻击参数
void SVMDDataApp::initAttack()
{
    frozenPos = mobility->getPositionAt(simTime());
    posOffset = Coord(
        par("posOffsetX").doubleValue(),
        par("posOffsetY").doubleValue(),
        0.0);
    double angle = uniform(0, 2 * M_PI);

    double rPos = sqrt(uniform(0, 1)) * par("randomPosRange").doubleValue();

    randomPos = Coord(
        rPos * cos(angle),
        rPos * sin(angle),
        0.0);
    frozenSpeed = mobility->getHostSpeed();
    speedOffset = Coord(
        par("speedOffsetX").doubleValue(),
        par("speedOffsetY").doubleValue(),
        0.0);
    double rSpeed = sqrt(uniform(0, 1)) * par("randomSpeedMax").doubleValue();

    randomSpeed = Coord(
        rSpeed * cos(angle),
        rSpeed * sin(angle),
        0.0);
    replayDelay = par("replayDelay");
    delayedReplayEvt = new cMessage("delayedReplayEvt");
    constantDoSInterval = par("constantDoSInterval");
    isReactiveTriggered = false;
    reactiveStopEvt = new cMessage("reactiveStopEvt");
    triggerDistance = par("triggerDistance");
    dosDuration = par("dosDuration");
    victimPath = par("targetVictimModulePath").stdstringValue();
    gridSybilCount = par("gridSybilCount").intValue();
    gridSybilSpacing = par("gridSybilSpacing").doubleValue();
}

/// @brief 应用攻击策略
void SVMDDataApp::applyAttack(SVMDformerMessage* bsm, Coord& realPos, Coord& realSpeed)
{
    switch (attackType) {
    case 1:
        bsm->setPos(frozenPos);
        break;
    case 2:
        bsm->setPos(frozenPos + posOffset);
        break;
    case 3:
        bsm->setPos(randomPos + realPos);
        break;
    case 4:
        bsm->setPos(randomPos + realPos + posOffset);
        break;
    case 5:
        bsm->setSpeed(frozenSpeed);
        break;
    case 6:
        bsm->setSpeed(frozenSpeed + speedOffset);
        break;
    case 7:
        bsm->setSpeed(randomSpeed + realSpeed);
        break;
    case 8:
        bsm->setSpeed(randomSpeed + realSpeed + speedOffset);
        break;
    case 9:
        bsm->setPos(frozenPos);
        bsm->setSpeed(Coord(0, 0, 0));
        bsm->setAccel(Coord(0, 0, 0));
        break;
    case 10:
        // 从缓存复制历史消息，原消息仍由当前发送流程负责管理。
        if (!replayBuffer.empty()) {
            int randomIndex = uniform(0, replayBuffer.size() - 1);
            bsm = check_and_cast<SVMDformerMessage*>(replayBuffer[randomIndex]->dup());
        }
        break;
    case 11:
        if (!replayBuffer.empty()) {
            auto closestIt = std::min_element(replayBuffer.begin(), replayBuffer.end(),
                [&realPos](SVMDformerMessage* a, SVMDformerMessage* b) {
                    double distA = a->getPos().distance(realPos);
                    double distB = b->getPos().distance(realPos);
                    return distA < distB;
                });

            bsm = check_and_cast<SVMDformerMessage*>((*closestIt)->dup());
        }
        break;
    case 12:
        // 延迟重放消息由 DelayMsg 持有，并在 delayedReplayEvt 到期时发送。
        if (!replayBuffer.empty()) {
            if (!delayedReplayEvt->isScheduled()) {
                int randomIndex = uniform(0, replayBuffer.size() - 1);
                if (DelayMsg) {
                    delete DelayMsg;
                    DelayMsg = nullptr; // 置空避免野指针
                }
                DelayMsg = check_and_cast<SVMDformerMessage*>(replayBuffer[randomIndex]->dup());
                scheduleAt(simTime() + replayDelay, delayedReplayEvt);
            }
        }
        break;
    case 13: // DoS 的发送间隔由 handleSelfMsg() 根据攻击类型决定。
    case 14:
    case 15:
        break;
    case 16:
        bsm->setPseudoId(generatePseudonym());
        break;
    case 17: {
        // 无目标、目标缺少 mobility 或距离超限时，回退到随机位置伪造。
        cModule* targetModule = getSimulation()->getModuleByPath(victimPath.c_str());
        if (!targetModule) {
            bsm->setPos(randomPos + realPos);
            break;
        }

        TraCIMobility* victimMobility = dynamic_cast<TraCIMobility*>(targetModule->getSubmodule("mobility"));
        if (!victimMobility) {
            bsm->setPos(randomPos + realPos);
            break;
        }

        Coord victimPos = victimMobility->getPositionAt(simTime());
        Coord victimSpeed = victimMobility->getHostSpeed();

        double distToVictim = victimPos.distance(realPos);
        if (distToVictim > 300.0) { // 300米是DSRC默认有效通信范围
            bsm->setPos(randomPos + realPos);
            break;
        }

        Coord fakePos = victimPos;
        if (victimSpeed.length() > 1e-6) {
            Coord direction = victimSpeed / victimSpeed.length();
            fakePos = victimPos + direction * 10.0;
        }

        bsm->setPseudoId(generatePseudonym());
        bsm->setPos(fakePos);
        bsm->setSpeed(victimSpeed);

        break;
    }
    case 18:
        bsm->setPseudoId(generatePseudonym());
        bsm->setPos(randomPos + realPos);
        break;
    case 19:
        // 每个 Sybil 消息独立构造、记录并发送，同时更新下一条消息的时间基准。
        Coord direction = realSpeed / realSpeed.length();

        for (int i = 0; i < gridSybilCount; ++i) {
            Coord fakePos = realPos + direction * (gridSybilSpacing * (i + 1));
            SVMDformerMessage* sybilBsm = new SVMDformerMessage();
            populateWSM(sybilBsm);
            sybilBsm->setDeltaT(simTime() - lastSendTime);
            sybilBsm->setPseudoId(generatePseudonym());
            sybilBsm->setSenderAddress(generatePseudonym());
            sybilBsm->setPos(fakePos);
            sybilBsm->setSpeed(Coord(0, 0, 0));
            sybilBsm->setAccel(Coord(0, 0, 0));
            sybilBsm->setHeading(direction);
            sybilBsm->setSerial(msgSerial);
            sybilBsm->setIsMaliciousLabel(isMalicious);

            datasetFile << simTime() - lastSendTime << ","
                        << sybilBsm->getPseudoId() << ","
                        << sybilBsm->getPos().x << "," << sybilBsm->getPos().y << ","
                        << sybilBsm->getSpeed().x << "," << sybilBsm->getSpeed().y << ","
                        << sybilBsm->getAccel().x << "," << sybilBsm->getAccel().y << ","
                        << sybilBsm->getHeading().x << "," << sybilBsm->getHeading().y << ","
                        << (isMalicious ? 1 : 0) << "\n";

            sendDown(sybilBsm);
            lastSendTime = simTime();
        }
        break;
    }
}
int SVMDDataApp::generatePseudonym()
{
    pseudonymCounter++;

    int seed = getParentModule()->getIndex() * 10000 + pseudonymCounter;
    int randomPart = uniform(0, 9999, 0);

    int newPseudonym = 1000000 + (seed + randomPart * 7) % 9000000;

    return newPseudonym;
}

void SVMDDataApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
}

void SVMDDataApp::handleSelfMsg(cMessage* msg)
{
    if (msg == sendBeaconEvt) {
        cancelEvent(sendBeaconEvt);
        sendBSM();
        msgSerial++;
        // 类型 13~15 分别使用固定、随机和反应式 DoS 发送间隔。
        if (attackType == 13 && isMalicious) {
            scheduleAt(simTime() + constantDoSInterval, sendBeaconEvt);
        }
        else if (attackType == 14 && isMalicious) {
            simtime_t randomDosInterval = uniform(0.2, 1.0, 0);
            scheduleAt(simTime() + randomDosInterval, sendBeaconEvt);
        }
        else if (attackType == 15 && isMalicious) {
            if (isReactiveTriggered) {
                scheduleAt(simTime() + constantDoSInterval, sendBeaconEvt);
            }
            else {
                scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            }
        }
        else {
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        }
    }
    else if (msg == reactiveStopEvt) {
        isReactiveTriggered = false;
    }
    else if (msg == pseudonymChangeMsg) {
        cancelEvent(pseudonymChangeMsg);
        currentPseudonym = generatePseudonym();
        simtime_t jitter = dblrand(0) * 0.1; // 0~0.1秒随机偏移
        simtime_t nextChange = simTime() + pseudonymLifetime + jitter;

        scheduleAt(nextChange, pseudonymChangeMsg);
    }
    else if (msg == delayedReplayEvt) {
        if (DelayMsg) {
            datasetFile << simTime() - lastSendTime << ","
                        << DelayMsg->getPseudoId() << ","
                        << DelayMsg->getPos().x << "," << DelayMsg->getPos().y << ","
                        << DelayMsg->getSpeed().x << "," << DelayMsg->getSpeed().y << ","
                        << DelayMsg->getAccel().x << "," << DelayMsg->getAccel().y << ","
                        << DelayMsg->getHeading().x << "," << DelayMsg->getHeading().y << ","
                        << (isMalicious ? 1 : 0) << "\n";
            sendDown(DelayMsg);
            DelayMsg = nullptr;
            lastSendTime = simTime();
        }
    }
    else {
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}

void SVMDDataApp::sendBSM()
{

    SVMDformerMessage* bsm = new SVMDformerMessage();
    populateWSM(bsm);

    // 基础字段
    simtime_t deltaT = simTime() - lastSendTime;
    bsm->setDeltaT(deltaT);
    bsm->setPseudoId(currentPseudonym);

    Coord realPos = mobility->getPositionAt(simTime());
    Coord realSpeed = mobility->getHostSpeed();
    Coord realHeading = mobility->getCurrentOrientation();

    // 加速度计算（防除零）
    double currentSpeedVal = realSpeed.length();
    double accelVal = 0.0;
    if (deltaT.dbl() > 1e-6) {
        accelVal = (currentSpeedVal - lastSpeedVal) / deltaT.dbl();
    }
    lastSpeedVal = currentSpeedVal;
    Coord accel = realHeading * accelVal;

    bsm->setPos(realPos);
    bsm->setSpeed(realSpeed);
    bsm->setAccel(accel);
    bsm->setHeading(realHeading);
    bsm->setSenderAddress(myId);
    bsm->setSerial(msgSerial);
    bsm->setIsMaliciousLabel(isMalicious);
    // 恶意节点篡改数据
    if (isMalicious) {
        applyAttack(bsm, realPos, realSpeed);
    }

    // 类型 12 和 19 的消息由延迟重放或 Sybil 分支负责发送基础消息。
    if (!isMalicious || (attackType != 12 && attackType != 19)) {

        datasetFile << deltaT << ","
                    << bsm->getPseudoId() << ","
                    << bsm->getPos().x << "," << bsm->getPos().y << ","
                    << bsm->getSpeed().x << "," << bsm->getSpeed().y << ","
                    << bsm->getAccel().x << "," << bsm->getAccel().y << ","
                    << bsm->getHeading().x << "," << bsm->getHeading().y << ","
                    << (isMalicious ? 1 : 0) << "\n";
        lastSendTime = simTime();
        sendDown(bsm);
    }
    else {
        delete bsm;
    }
}

void SVMDDataApp::onWSM(BaseFrame1609_4* frame)
{
    // 分流自定义BSM消息
    if (SVMDformerMessage* bsm = dynamic_cast<SVMDformerMessage*>(frame)) {
        onBSM(bsm);
    }
    else {
        DemoBaseApplLayer::onWSM(frame);
    }
}

void SVMDDataApp::onBSM(SVMDformerMessage* bsm)
{
    if (isMalicious && (attackType == 10 || attackType == 11 || attackType == 12)) {

        SVMDformerMessage* clonedMsg = check_and_cast<SVMDformerMessage*>(bsm->dup());

        replayBuffer.push_back(clonedMsg);
    }
    if (replayBuffer.size() > MAX_BUFFER_SIZE) {
        delete replayBuffer.front();
        replayBuffer.pop_front();
    }
    if (isMalicious && attackType == 15) {
        if (bsm->getPos().distance(mobility->getPositionAt(simTime())) <= triggerDistance) {
            isReactiveTriggered = true;
            cancelEvent(reactiveStopEvt);
            scheduleAt(simTime() + dosDuration, reactiveStopEvt);
        }
    }
}

void SVMDDataApp::finish()
{
    if (sendBeaconEvt) {
        cancelAndDelete(sendBeaconEvt);
        sendBeaconEvt = nullptr;
    }
    if (pseudonymChangeMsg) {
        cancelAndDelete(pseudonymChangeMsg);
        pseudonymChangeMsg = nullptr;
    }

    if (delayedReplayEvt) {
        cancelAndDelete(delayedReplayEvt);
        delayedReplayEvt = nullptr;
    }
    if (reactiveStopEvt) {
        cancelAndDelete(reactiveStopEvt);
        reactiveStopEvt = nullptr;
    }
    if (DelayMsg) {
        delete DelayMsg;
        DelayMsg = nullptr;
    }
    for (auto* msg : replayBuffer) {
        delete msg;
    }
    replayBuffer.clear();
    if (datasetFile.is_open()) {
        datasetFile.close();
    }

    DemoBaseApplLayer::finish();
}
