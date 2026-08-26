package org.car2x.veins.modules.application.SVMD;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple SVMDformerVehicleApp extends
DemoBaseApplLayer
{
    parameters:
        double misbehaviorRatio = 0.3;							//恶意节点比例
		double pseudonymLifetime = default(10s) @unit(s); 		//更换伪id时间间隔
		int misbehaviorType = default(0); 						//攻击类型
		double posOffsetX  = default(30m) @unit(m);				//固定X轴坐标偏移
		double posOffsetY  = default(20m) @unit(m);				//固定Y轴坐标偏移
		double randomPosRange=default(100m) @unit(m);			//随机偏移半径
		double speedOffsetX  = default(10mps) @unit(mps);		//固定X轴速度偏移
        double speedOffsetY = default(5mps) @unit(mps);			//固定Y轴速度偏移
        double randomSpeedMax  = default(15mps) @unit(mps);		//随机速度偏移范围
        double replayDelay= default(10s) @unit(s);				//重放攻击延迟时间
		double constantDoSInterval = default(0.1s) @unit(s);	//Dos固定攻击间隔
		double triggerDistance = default(50m) @unit(m);			//随机Dos攻击触发距离
		double dosDuration = default(5s) @unit(s);				//延迟Dos攻击时间
		string targetVictimModulePath = default("node[5]");		//定向女巫攻击车辆
		int gridSybilCount = default(5);       					//阵列女巫攻击车辆数
		double gridSybilSpacing  = default(20m) @unit(m);		//阵列女巫攻击车辆间隔距离

        @class(veins::SVMDDataApp)； 							//链接cc文件
        @display("i=block/app2");								//图形化仿真页面默认背景
}
