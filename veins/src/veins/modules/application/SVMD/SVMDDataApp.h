#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <deque>
#include <algorithm>
#include <cmath>

namespace veins {

class SVMDformerMessage; // 自定义BSM消息类型

class VEINS_API SVMDDataApp : public DemoBaseApplLayer {
public:
    void initialize(int stage) override; // 初始化函数
    void finish() override; // 仿真结束时的清理函数

protected:
    simtime_t beaconInterval; // Beacon消息广播周期
    double misbehaviorRatio; // 恶意节点比例

    bool isMalicious; // 当前车辆是否为恶意节点
    long msgSerial; // 消息的自增序列号
    cMessage* sendBeaconEvt; // 定时触发BSM发送的自消息
    simtime_t lastSendTime; // 上一次发送BSM的时间戳
    double lastSpeedVal = 0.0; // 记录上一次的速度值，用于计算加速度

    std::ofstream datasetFile; // 数据集文件流

    cMessage* pseudonymChangeMsg = nullptr; // 自消息，用于触发伪ID更换
    int currentPseudonym; // 当前伪ID
    simtime_t pseudonymLifetime; // 伪ID有效期（秒）
    int pseudonymCounter = 0; // 伪id更换次数计数器

    int attackType = 0; // 恶意行为类型（0表示正常节点，1-19表示不同的攻击类型）
    Coord frozenPos; // 冻结位置
    Coord posOffset; // 位置偏移
    Coord randomPos; // 随机位置
    Coord frozenSpeed; // 冻结速度
    Coord speedOffset; // 速度偏移
    Coord randomSpeed; // 随机速度
    std::deque<SVMDformerMessage*> replayBuffer; // 消息缓存池
    const int MAX_BUFFER_SIZE = 50; // 缓存池最大容量
    cMessage* delayedReplayEvt = nullptr; // 延迟重放的自消息闹钟
    simtime_t replayDelay; // 延迟重放的时间间隔
    simtime_t constantDoSInterval; // 恒定DoS攻击间隔
    SVMDformerMessage* DelayMsg = nullptr; // 延迟重放的消息指针
    double triggerDistance; // 触发距离阈值
    bool isReactiveTriggered; // 是否触发了反应式攻击
    cMessage* reactiveStopEvt = nullptr; // 用于自动停火的自消息闹钟
    simtime_t dosDuration; // DoS攻击持续时间
    cModule* victimModule = nullptr; // 目标受害者模块指针
    int gridSybilCount; // 网格Sybil攻击的数量
    double gridSybilSpacing; // 网格Sybil攻击的间距
    std::string victimPath; // 目标受害者模块路径

protected:
    // -------------------------- 父类虚函数重写 --------------------------
    /// 处理下层递上来的WSM帧（DemoBaseApplLayer统一回调入口)
    void onWSM(BaseFrame1609_4* wsm) override;

    /// 自消息处理：定时触发BSM发送
    void handleSelfMsg(cMessage* msg) override;

    /// 车辆位置更新回调：每仿真步触发，用于按间隔触发广播
    void handlePositionUpdate(cObject* obj) override;

    // -------------------------- 自定义业务函数 --------------------------
    /// 构造并发送一条BSM消息
    void sendBSM();

    /// 处理收到的BSM消息，提取特征并记录数据集
    void onBSM(SVMDformerMessage* bsm);

    /// 生成新伪ID
    int generatePseudonym();

    /// 定义恶意行为攻击逻辑
    void applyAttack(SVMDformerMessage* bsm, Coord& realPos, Coord& realSpeed);

    /// 初始化攻击参数
    void initAttack();

    /// 将消息特征写入数据集，保持所有发送路径使用相同字段顺序
    void writeDatasetRow(const SVMDformerMessage* bsm, simtime_t deltaT);
};

} // namespace veins
