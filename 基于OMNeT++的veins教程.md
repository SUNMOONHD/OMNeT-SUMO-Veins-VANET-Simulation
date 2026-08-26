# 车联网项目数据集生成保姆级指南
## 基于OMNeT++ + SUMO + Veins的车联网数据集生成

### 一、三个仿真软件的核心作用

车联网数据集生成基于「OMNeT++ + SUMO + Veins」三者协同，各自分工明确：

1. **OMNeT++**：离散事件网络仿真宿主平台。负责网络通信层的事件调度、节点生命周期管理、无线消息传输仿真、可视化调试与结果统计；提供 NED 语言定义网络拓扑，支持 C++ 编写业务逻辑，提供图形化（Qtenv）和命令行（Cmdenv）两种运行模式。
2. **SUMO**：道路交通仿真工具。负责真实路网构建、车辆行驶动力学模拟、交通流轨迹生成；通过 TraCI 接口与 Veins 交互，向 Veins 输出车辆位置、速度等状态，也可接收 Veins 指令控制车辆行为（如强制停车模拟事故）。
3. **Veins**：车联网专用仿真框架。桥接 OMNeT++ 与 SUMO，内置 IEEE 802.11p 协议栈、车辆 / RSU 节点模型、无线信道衰减模型、移动性管理模块；提供分层的应用层开发框架，用户只需扩展应用层类，即可自定义 V2V/V2I 通信逻辑，是数据集采集的核心开发载体。

> **注意**：这三个软件需要配置相关的依赖，在 PC 上有些环境变量容易起冲突，建议在虚拟机上运行。首先下载 VirtualBox，然后从 [https://veins.car2x.org/download/](https://veins.car2x.org/download/) 下载后缀为 i1 的版本，得到 ova 文件，双击打开后在 VirtualBox 上运行，进行基础配置（内存大小、CPU 个数等）。运行后输入密码 `veins` 进入虚拟机，点击左上角 **Activities** 进入菜单，打开 OMNeT++ 编写业务代码。运行仿真时需启动 `veins_launchd`（在 Activities 中可看到）。

---

### 二、生成数据集的关键文件分类

生成车联网数据集的流程分为两步：**第一步用指令自动生成路网与交通流基础文件**，**第二步手动编写 / 修改仿真配置与业务代码**。

#### 2.1 可通过指令自动生成的文件（SUMO 工具链）

这类文件基于 OpenStreetMap 导出的 `.osm` 地图，通过 SUMO 自带命令行工具生成，无需手动编写代码。

| **生成文件** | **生成命令** | **核心参数含义** |
| --- | --- | --- |
| 路网文件 `map.net.xml` | `netconvert --osm-files map.osm --output-file map.net.xml --remove-edges.by-type "railway.rail,railway.subway,waterway.river,railway.platform,waterway.canal"` | `--osm-files`：输入的 OSM 原始地图<br>`--output-file`：输出的路网文件名<br>`--remove-edges.by-type`：过滤移除铁路、水路等无关道路<br>可使用 `netedit` 先对路网进行裁剪修改，此步骤完成后需要重新生成建筑文件和路由文件 |
| 建筑地形文件 `map.poly.xml` | `polyconvert --osm-files map.osm --net-file map.net.xml --type-file ~/src/sumo-1.11.0/data/typemap/osmPolyconvert.typ.xml -o map.poly.xml` | `--net-file`：对应生成的路网文件<br>`--type-file`：SUMO 自带的建筑类型映射配置，可使用 `find -name osmPolyconvert.typ.xml` 查询相关模板文件地址<br>`-o`：输出的建筑地形文件 |
| 车流路由文件 `map.rou.xml`（逐车生成） | `python 你的路径/randomTrips.py -n map.net.xml -r map.rou.xml -b 0 -e 600 --period 2.55 --fringe-factor 10 -s 1 --min-distance 3000 -o trips.trips.xml` | `-n`：输入路网文件<br>`-r`：输出车流路由文件<br>`-b/-e`：仿真起止时间（秒）<br>`--period`：车辆生成间隔（秒 / 辆）<br>`--fringe-factor`：地图边界生成车辆的权重<br>`-s`：随机数种子，控制路径随机性，种子数固定则生成车辆固定（保证实验数据可复现）<br>`--min-distance`：车辆最小行驶路径长度<br>可使用 `find -name randomTrips.py` 查询相关文件地址 |
| 车流路由文件 `map.rou.xml`（车队生成） | `python ~/src/sumo-1.11.0/tools/randomTrips.py -n manhattan.net.xml -r manhattan.rou.xml -b 0 -e 1800 --flows 10 --period 3 --fringe-factor 10 -s 1 --min-distance 3500` | `--flows`：生成的车流数量，批量生成车队 |

#### 2.2 需要手动编写 / 修改的文件

这类文件定义仿真逻辑、数据采集规则，是生成数据集的核心。

| **文件类型** | **核心编写 / 修改内容** |
| --- | --- |
| `.sumo.cfg`（SUMO 总配置） | 1. 关联 `.net.xml` / `.rou.xml` / `.poly.xml` 三个资源文件<br>2. 设置仿真起止时间、仿真步长（通常 0.1s）<br>3. 关闭冗余日志与校验，提升运行速度 |
| `.launchd.xml`（Veins-SUMO 连接配置） | 1. 声明 Veins 启动 SUMO 时需要加载的所有资源文件<br>2. 指定 SUMO 配置文件类型，作为两者通信的入口 |
| `omnetpp.ini`（仿真总控配置） | 1. 指定网络拓扑 NED 文件、仿真总时长、场景尺寸<br>2. 绑定 `.launchd.xml` 文件路径<br>3. 配置 RSU 数量、坐标、应用层类型、通信参数<br>4. 配置车辆应用层类型、自定义实验参数（如恶意车辆比例）<br>5. 可选：配置参数遍历，批量生成多组数据集 |
| 场景 NED 文件（如 `RSUExampleScenario.ned`） | 1. 继承 `Scenario` 父类<br>2. 声明 RSU 子模块数组，修改 RSU 数量<br>3. 配置 RSU 可视化样式 |
| 应用层 NED 文件 | 1. 继承 `DemoBaseApplLayer` 父类<br>2. 声明自定义仿真参数（如事故比例、随机种子）<br>3. 通过 `@class` 绑定对应的 C++ 实现类 |
| `config.xml` 文件（物理层配置） | 可直接使用相关样例文件中的代码 |
| `.msg` 自定义消息文件 | 1. 继承 `BaseFrame1609_4` 基类<br>2. 定义数据集需要的字段（坐标、速度、事故类型、时间戳等）<br>3. 编译后自动生成对应的 C++ 类文件 |
| 应用层 `.h` + `.cc` | 1. 重写父类虚函数，实现消息收发、位置更新逻辑<br>2. 编写数据采集代码，将特征写入文件生成数据集<br>3. 实现自定义消息的处理与转发逻辑 |

---

### 三、有关文件架构和注意事项（不包括 RSU 模块）

RSU 模块接收数据和 V2V 重复，所以取消 RSU 模块。

```ini
[General]
cmdenv-express-mode = true
cmdenv-autoflush = true
cmdenv-status-frequency = 1s
**.cmdenv-log-level = info
network = RSUExampleScenario
```

在编辑器中 **Ctrl + 点击** `RSUExampleScenario` 进入对应的 NED 文件，修改如下：

```ned
submodules:
    rsu[0]: RSU {                      // 将 rsu 中值改为 0，即不放置 RSU
        @display("p=150,140;i=veins/sign/yellowdiamond;is=vs");
    }
```

#### 有关文件地址

在 `veins/examples/` 文件夹下创建自己项目的文件夹（`examples` 下的 `veins` 文件夹是官方示例项目）。在自定义的文件夹下，先将官方示例中的 `config.xml`、`omnetpp.ini`、`antenna.xml`、`RSUExampleScenario.ned` 文件直接复制下来。接着打开终端，在当前目录下根据要求填写参数，使用 `netconvert`、`randomTrips.py`、`polyconvert` 等指令生成相关文件。

> 说明：生成 `rou.xml` 时会同步生成 `rou.alt.xml` 和 `trips.trips.xml` 文件，分别为备份文件和车辆出行信息文件，无需处理。

然后修改 `.launchd.xml` 和 `.sumo.cfg` 文件，**第五节**提供了模板文件（很好修改，只需要更改文件名即可）。

#### 有关应用层文件

在 `veins/src/modules/application` 下有官方示例项目文件夹 `traci`。你需要在该目录下创建自己项目的文件夹，里面包含 `.msg`、`.h`、`.cc`、`.ned` 文件。这里提到的应用层相关文件（`msg`、`h`、`cc`）都是车辆应用层的，RSU 应用层不用写。

具体每个文件的编写可见后续章节，本节只说明文件间的关系和注意事项：

**1. 为什么要在 `veins/src/modules/application` 下编写应用层文件？**

你编写的文件需要编译，要事先定义编译链。具体的编译规则需要在 Makefile 文件中书写，而官方示例在 `veins/src` 文件夹下提供了 Makefile，里面定义了 `veins/src/modules/application` 文件夹下全部文件的编译规则。此外，在编写 `.msg` 文件时需要使用 `import` 关键词导入相关物理层的包，`import` 是 Java 风格的导入机制，自行编写 Makefile 处理这件事会比较困难。

> **注意**：由于你所有项目的应用层代码都受同一个官方 Makefile 影响，因此你在 `veins/src/modules` 下编写的全部代码都必须编译成功才能启动仿真。即使当前 `ini` 文件链接的只是某一个项目的应用层，与其他项目的应用层完全无关，其他项目代码编译不通过也会导致无法启动。

**2. `.msg` 文件的作用与编译**

`.msg` 文件的书写非常简单，除去一些模板内容，你只需要定义一些不同类型的变量来对应数据集的特征即可。右键 `.msg` 文件进行编译，会自动生成对应的 `.h` 和 `.cc` 文件，里面将你之前定义的变量封装到一个类里，并自动生成一些成员函数（包括变量的赋值函数、取值函数）。可通过查看这两个文件了解这些方法（函数名非常直观：`getXxx()` 获取值，`setXxx()` 设置值）。

这些自动生成的方法主要用于写数据集时获取数据和车辆发送时定义数据。要在具体应用层中使用，只需通过 `#include "文件名_m.h"` 导入，然后定义类即可，类名已在 `.msg` 生成的 `.h` 文件中定义。

**3. 应用层文件的联系**

`ini` 文件关联到 `ned` 文件，`ned` 文件关联到 `cc` 文件，`cc` 文件关联到 `msg` 文件，这就是整体的引用链。每个文件具体涉及到哪些官方定义好的父类变量和成员函数，后续章节会详细说明。

---

### 四、Veins 父类框架说明

Veins 应用层有完整的继承链：

```
BaseLayer → BaseApplLayer → DemoBaseApplLayer → 自定义应用层
```

**所有底层通用逻辑父类都已实现，不需要从零编写初始化、消息分发等框架代码**，只需重写父类预留的虚函数，插入自定义的数据采集与通信逻辑即可。

---

### 五、有关配置文件代码模板

#### `config.xml` 文件（可直接使用）

```xml
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <AnalogueModels>
        <AnalogueModel type="SimplePathlossModel" thresholding="true">
            <parameter name="alpha" type="double" value="2.0"/>
        </AnalogueModel>
        <AnalogueModel type="SimpleObstacleShadowing" thresholding="true">
            <obstacles>
                <type id="building" db-per-cut="9" db-per-meter="0.4" />
            </obstacles>
        </AnalogueModel>
    </AnalogueModels>
    <Decider type="Decider80211p">
        <!-- The center frequency on which the phy listens -->
        <parameter name="centerFrequency" type="double" value="5.890e9"/>
    </Decider>
</root>
```

#### `.sumo.cfg`（SUMO 总配置）

```xml
<?xml version="1.0" encoding="UTF-8"?>

<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/sumoConfiguration.xsd">

    <input>
        <!-- 只需修改这里即可 -->
        <net-file value="erlangen.net.xml"/>        <!-- 修改为自定义的 net 文件名 -->
        <route-files value="erlangen.rou.xml"/>    <!-- 修改为自定义的 rou 文件名 -->
        <additional-files value="erlangen.poly.xml"/>  <!-- 修改为自定义的 poly 文件名 -->
    </input>

    <time>
        <begin value="0"/>          <!-- 仿真起始时间：0s -->
        <end value="1000"/>         <!-- 仿真结束时间：1000s -->
        <step-length value="0.1"/>  <!-- 仿真引擎步长（每隔 0.1s 做一次更新） -->
    </time>

    <report>
        <xml-validation value="never"/>
        <xml-validation.net value="never"/>
        <no-step-log value="true"/>
        <!-- 取消日志输出，提高运行效率 -->
    </report>

    <gui_only>
        <start value="true"/>
    </gui_only>

</configuration>
```

#### `antenna.xml`（物理配置相关文件，直接复制粘贴）

```xml
<?xml version="1.0" encoding="UTF-8"?>

<!--
// Copyright (C) 2016 Alexander Brummer <alexander.brummer@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: (GPL-2.0-or-later OR CC-BY-SA-4.0)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// -
//
// At your option, you can also redistribute and/or modify this file
// under a Creative Commons Attribution-ShareAlike 4.0 International License.
//
// You should have received a copy of the license along with this
// work.  If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.
-->

<root>
    <Antenna type="SampledAntenna1D" id="monopole">
        <!-- monopole antenna on roof -->
        <!-- samples taken from "Effects of Antenna Characteristics and Placements on a Vehicle-to-Vehicle Channel Scenario" -->
        <parameter name="samples" type="string" value="-0.888 -0.942 -1.109 -1.29 -1.543 -1.717 -1.898 -1.902 -1.979 -2.018 -2.18 -2.336 -2.354 -2.287 -2.181 -2.008 -1.837 -1.667 -1.538 -1.553 -1.687 -1.819 -1.921 -1.977 -1.902 -1.768 -1.672 -1.741 -1.888 -2.167 -2.304 -2.326 -2.114 -1.838 -1.53 -1.36 -1.275 -1.331 -1.524 -1.759 -2.046 -2.212 -2.251 -2.04 -1.732 -1.519 -1.476 -1.579 -1.713 -1.775 -1.73 -1.585 -1.423 -1.339 -1.263 -1.433 -1.62 -1.857 -1.973 -2.059 -2.114 -2.097 -1.991 -1.95 -1.865 -1.865 -1.736 -1.606 -1.371 -1.17 -0.986 -0.893"/>
    </Antenna>
    <Antenna type="SampledAntenna1D" id="panorama">
        <!-- monopole antenna on panorama glass roof -->
        <!-- samples taken from "Influence of car panorama glass roofs on Car2Car communication" -->
        <parameter name="samples" type="string" value="-14.962 -14.531 -14.035 -15.912 -13.103 -11.064 -9.902 -4.728 -6.49 -4.516 -2.66 -0.206 -1.223 2.692 3.219 2.568 3.52 5.896 6.006 6.384 5.405 5.279 5.243 5.433 3.03 2.296 1.664 0.618 1.708 -0.457 1.822 -0.799 1.658 2.735 0.948 0.622 1.156 2.046 1.655 2.611 1.335 -0.108 1.857 0.207 1.221 1.316 2.706 3.575 5.188 7.051 5.599 6.507 7.22 6.805 6.252 6.154 4.16 2.247 3.291 2.866 -1.093 -0.769 -2.331 -4.004 -5.806 -4.79 -10.014 -12.566 -15.903 -14.306 -11.265 -14.368"/>
    </Antenna>
    <Antenna type="SampledAntenna1D" id="patch">
        <!-- patch antenna on side mirrors -->
        <!-- samples taken from "Effects of Antenna Characteristics and Placements on a Vehicle-to-Vehicle Channel Scenario" -->
        <parameter name="samples" type="string" value="5.061 5.016 4.961 4.804 4.577 4.334 3.999 3.549 3.027 2.456 1.833 1.045 0.25 -0.579 -1.516 -2.48 -3.459 -4.348 -5.361 -6.195 -7.24 -8.236 -9.2 -10.345 -11.326 -12.302 -13.302 -13.833 -14.209 -14.186 -13.844 -13.361 -12.824 -12.239 -11.901 -11.588 -11.487 -11.523 -11.733 -12.162 -12.529 -13.15 -13.723 -14.141 -14.289 -14.069 -13.538 -12.537 -11.764 -10.582 -9.617 -8.646 -7.65 -6.601 -5.695 -4.599 -3.598 -2.608 -1.64 -0.741 0.079 0.958 1.62 2.36 2.911 3.515 3.962 4.313 4.593 4.808 4.957 5.041"/>
    </Antenna>
</root>
```

#### `.launchd.xml`（Veins 启动配置）

```xml
<?xml version="1.0"?>
<launch>
    <copy file="map.net.xml" />
    <copy file="map.rou.xml" />
    <copy file="map.poly.xml" />
    <copy file="map.sumo.cfg" type="config" />
    <!-- 只需将这四个文件改为自定义的文件名即可 -->
</launch>
```

---

### 六、有关业务代码（没提到的代码块直接使用即可）

#### `.ned` 文件和其在 `ini` 文件中的引用

```ned
package org.car2x.veins.modules.application.文件夹名;  // 定义该应用层所属的包
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;  // 导入父类，负责处理功能消息收发

simple 你自己定义的应用层名字（不含任何后缀） extends DemoBaseApplLayer
{
    parameters:
        int datasetSeed = 1;
        double misbehaviorRatio = 0.3;
        // 这里定义你需要设置的参数，可在 ini 文件中修改，如恶意节点比例、随机种子数等
        // 注意：由于继承了 DemoBaseApplLayer，DemoBaseApplLayer 里已经定义的参数（如 beaconInterval 发送间隔等）无需再设置。
        // 设置前可先查看 DemoBaseApplLayer 了解有哪些已定义好的参数。

        @class(veins::你写的 cc 文件名（不含后缀）);
        @display("i=block/app2");  // 仿真展示图标，照此粘贴即可
}
```

在 `omnetpp.ini` 中的引用：

```ini
##########################################################
#                      App Layer                         #
##########################################################
*.node[*].applType = "你的 ned 完整类型名（无后缀，需匹配 package + simple 名）"
*.node[*].appl.headerLength = 80 bit
*.node[*].appl.sendBeacons = false
*.node[*].appl.dataOnSch = false
*.node[*].appl.beaconInterval = 1s
# 以上为在 ini 中设置 ned 定义的参数
```

#### `.msg` 文件

```msg
import veins.base.utils.Coord;         // Coord：三维数据类型，用于定义速度、坐标、航向角等属性
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;
// 以上两个包属于物理层相关定义，用于包装二层 MAC 帧
// 说明：这里涉及计算机网络知识。简单理解就是每辆车都有一个网卡，每个网卡都有独一无二的 ID。
// 车辆给别的车辆发送数据时，需要构造 MAC 帧（系统会自动完成），帧中包含数据体和帧头
// （帧头包含发送方 ID、接收方 ID 和协议信息等）。物理层会自动完成封装，无需手动处理，
// 写 .msg 文件时只需原样导入以上包即可。

namespace veins;  // 命名空间

packet 你自己定义的 msg 名（无后缀，后续 cc 文件中使用的 msg 类就是这个） extends BaseFrame1609_4 {
    LAddress::L2Type senderAddress = -1;  // 发送节点 ID
    // LAddress::L2Type 是系统提供的 64 位数据类型，代表上述提到的网卡唯一 ID
    // 注意：真实 MAC 地址普遍为 48 位，仿真软件中无需较真

    simtime_t deltaT;          // 消息发送时间间隔
    long pseudoId;             // PseudoID 伪 ID

    Coord pos;                 // pos (x, y) 位置坐标
    Coord speed;               // spd (x, y) 速度
    Coord accel;               // acl (x, y) 加速度
    Coord heading;             // hed (x, y) 航向角
    long serial = 0;           // 消息序列号
    bool isMaliciousLabel = false;
    // 这里写你要定义的特征值即可
    // 注意：速度、坐标等有 API 可直接获取，加速度需要通过对速度微分得到，无直接 API，
    // 具体 API 和加速度求解方法在 cc 文件部分说明
}
```

> **说明**：`.msg` 文件编译后会在同目录下生成对应的 `_m.cc` 和 `_m.h` 文件，用处已在第三节提到。

#### `.h` 文件

```cpp
#pragma once

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include <fstream>
#include <set>

namespace veins {

class VEINS_API 当前 cc 文件名（无后缀） : public DemoBaseApplLayer {
public:
    void initialize(int stage) override;
    void finish() override;

protected:
    // ===== 基础状态变量 =====
    simtime_t lastDroveAt;        // 上次行驶时间
    bool sentMessage;             // 是否已发送消息
    int currentSubscribedServiceId;  // 当前订阅的服务 ID

    // ===== 自定义变量 =====
    // .msg 中定义的一些变量（如恶意节点比例、是否为恶意节点等，
    // 可复用的定义在此，速度之类使用临时变量即可）
    bool isMalicious;             // 是否为恶意节点标记
    int datasetSeed;              // 数据集随机种子
    double misbehaviorRatio;      // 恶意节点比例
    long msgSerial;               // 消息序列号
    double lastSpeedVal;          // 上一次记录的速度标量值

    // ===== 文件操作 =====
    std::ofstream datasetFile;    // 数据输出文件流

    // ===== 数据去重（可选）=====
    std::set<std::pair<long, long>> receivedMsgSet;

    // ===== 时间戳 =====
    simtime_t lastSendTime;       // 记录上一次发送消息的时间

    // ===== 自消息（定时器）=====
    // cMessage* sendBeaconEvt;
    // 说明：定时触发 BSM 发送的自消息。该数据结构只起「闹钟」作用，
    // 通过设置一个定时器，定时发送这个自消息。
    // 回调函数 handleSelfMsg（父类成员函数）接收到的自消息如果等于 sendBeaconEvt，
    // 就执行实际的消息发送逻辑。
    cMessage* sendBeaconEvt;

protected:
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;
    void onBSM(Message* bsm);  // 自定义 BSM 消息处理函数（根据你的 msg 类名调整）

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    void sendBSM();  // 自定义 BSM 发送函数

    // 以上均为父类 DemoBaseApplLayer 的虚函数，我们进行重写。
    // 后续分别介绍每个函数的作用和编写方式。
};

} // namespace veins
```

#### `.cc` 文件基本骨架

```cpp
#include "veins/modules/application/SVMD/DataApp.h"      // 替换为你自己的头文件路径
#include "veins/modules/application/SVMD/Message_m.h"    // 替换为你自己的 msg 生成头文件
// 调用头文件和 message 生成的头文件
#include <fstream>

using namespace veins;

Define_Module(veins::DataApp);  // 替换为你自己的类名

// 以下编写各函数具体实现，具体编写指导见第七节
```

---

### 七、有关父类成员函数的重写

#### `initialize()` 函数重写

该函数的作用是初始化参数，具体初始化方式大同小异，这里提供一种通用的方法。

```cpp
// 每辆车生成时都会调用这个函数
void DataApp::initialize(int stage) {
    DemoBaseApplLayer::initialize(stage);  // 先调用父类的启动函数，避免缺失某些定义

    // stage 表示初始化阶段，这里只处理阶段 stage == 0（参数定义、变量初始化等）
    if (stage == 0) {
        // ===== 读取参数（beaconInterval 继承自父类）=====
        beaconInterval = par("beaconInterval");
        // 这个参数在父类中已定义（ned 里无需重复定义），但 cc 中用到时需要在此获取
        datasetSeed = par("datasetSeed").intValue();
        misbehaviorRatio = par("misbehaviorRatio").doubleValue();
        // 导入参数语法：par("参数名")
        // 注意：导入的参数需要通过 .intValue() / .doubleValue() 等转换为需要的类型，
        // 具体 API 可在 cPar.h 文件中查看（右上角放大镜搜索）

        // ===== 判定恶意节点 =====
        int nodeId = getParentModule()->getIndex();
        // 获取每辆车 ID 的方法：getParentModule() 获取车辆的父模块，getIndex() 获取索引。
        // 相关 API 可在 cModule.h 中查询
        // 注意：默认用数组存储车辆，这里的 ID 即为索引，从 0、1、2... 开始
        isMalicious = uniform(0.0, 1.0, 0) < misbehaviorRatio;
        // 设置恶意节点比例：uniform 随机生成 0-1 之间的数，第三个参数为种子数（默认 0 即可）。
        // 该方法可以近似保证恶意节点比例，如果需要完全精确的比例，可使用洗牌法（具体可自行查询）

        // ===== 初始化运行变量 =====
        msgSerial = 0;
        lastSendTime = simTime();
        lastSpeedVal = 0.0;
        sendBeaconEvt = new cMessage("sendBeaconEvt");  // 初始化「闹钟」

        // ===== 发送数据的核心机制 =====
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
        // simTime()：获取当前仿真时间
        // 含义：在 simTime() + beaconInterval 这个时刻发送自消息 sendBeaconEvt。
        // handleSelfMsg（父类成员函数）接收到这个自消息后，执行数据收发逻辑。
        // （自消息可用 cMessage 自定义，也有内置的）
        // 这里通过 scheduleAt 初始化定义闹钟，只运行一次。

        // ===== 打开数据集文件，追加模式 =====
        datasetFile.open("svmd_dataset.csv", std::ios::app);

        // ===== 可视化标记（可选）=====
        // 在仿真界面将恶意节点车辆标注为红色，只生成数据集可以不写
        findHost()->getDisplayString().setTagArg("i", 1, isMalicious ? "red" : "white");
    }
}
```

#### `finish()` 函数重写

释放定义的自消息并调用父类函数即可。

```cpp
void DataApp::finish()
{
    if (sendBeaconEvt) {
        cancelAndDelete(sendBeaconEvt);
        sendBeaconEvt = nullptr;
    }
    // 释放定义的自消息

    DemoBaseApplLayer::finish();
    // 调用父类成员函数
}
```

#### `handlePositionUpdate()` 函数重写

**原理**：SUMO 引擎根据设置的步长定时返回一个 `cObject` 对象（存储位置信息），该函数接收后更新车辆位置。

```cpp
void DataApp::handlePositionUpdate(cObject* obj)
{
    DemoBaseApplLayer::handlePositionUpdate(obj);
    // 无特殊要求直接调用父类成员函数即可
}
```

#### `handleSelfMsg()` 函数重写

```cpp
void DataApp::handleSelfMsg(cMessage* msg)  // 该回调函数接收一个自消息
{
    if (msg == sendBeaconEvt) {
        sendBSM();               // 发送 Beacon
        lastSendTime = simTime();
        msgSerial++;

        if (sendBeaconEvt->isScheduled()) {
            // 检测下一个时刻的「闹钟」是否已定义，如果有则取消，避免重复定义
            cancelEvent(sendBeaconEvt);
        }
        // 定义下一个时刻发送消息的闹钟
        scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
    } else {
        // 调用父类成员函数处理其他自消息（指内置的一些自消息）
        DemoBaseApplLayer::handleSelfMsg(msg);
    }
}
```

#### `onWSM()` 函数重写

```cpp
void DataApp::onWSM(BaseFrame1609_4* frame)
{
    // 分流自定义 BSM 消息
    // WSM 是车辆分发的安全信息大类，我们项目中定义的 BSM 属于 WSM 这个大类，
    // 所以在 onWSM 中将消息分流到 onBSM 函数处理
    if (Message* bsm = dynamic_cast<Message*>(frame)) {
        onBSM(bsm);
    } else {
        DemoBaseApplLayer::onWSM(frame);
    }
}
```

#### `onBSM()` 函数重写

该函数主要处理车辆收到的消息，可根据要求自行编写逻辑。`Message*` 为 `.msg` 文件生成的 `.h` 中定义的类（根据你的实际类名调整）。

```cpp
void DataApp::onBSM(Message* bsm)
{
    // 在此编写接收消息后的处理逻辑
}
```

#### `sendBSM()` 函数实现（数据生成核心）

该函数是数据生成最关键的一步，发送数据处理、数据写入文件等逻辑都在此处完成。

```cpp
void DataApp::sendBSM()
{
    Message* bsm = new Message();  // 根据你实际的 msg 类名调整
    populateWSM(bsm);              // 将一些内置参数填入 bsm，如仿真时间、车辆 ID 等

    // ===== 基础字段 =====
    // 这里的 setDeltaT() 等函数在 msg 编译生成的 _m.h 中自动生成，
    // 目的是将数据写入到生成的 msg 类里，可查询 _m.h 文件查看具体函数
    simtime_t deltaT = simTime() - lastSendTime;
    bsm->setDeltaT(deltaT);
    bsm->setPseudoId(getParentModule()->getIndex());

    Coord realPos = mobility->getPositionAt(simTime());
    Coord realSpeed = mobility->getHostSpeed();
    Coord realHeading = mobility->getCurrentOrientation();
    // 以上分别获取位置、速度、航向，均为 Coord 类型（内置三维数据存储类型）
    // 注意：这里提到的获取数据的方法分别定义在 BaseMobility.h 和 TraCIMobility.h 中，
    // 可访问头文件查询其他 public 方法

    // ===== 加速度计算（防除零）=====
    double currentSpeedVal = realSpeed.length();  // length()：对速度向量取模
    double accelVal = 0.0;                        // 加速度标量
    if (deltaT.dbl() > 1e-6) {
        // deltaT 是 simtime_t 类型，需要通过 .dbl() 转换为 double 类型
        accelVal = (currentSpeedVal - lastSpeedVal) / deltaT.dbl();
    }
    lastSpeedVal = currentSpeedVal;
    Coord accel = accelVal * realHeading;  // realHeading 为航向（单位向量）
    // 这种计算得到的加速度并非精确值，因为使用的是标量乘以航向，
    // 但由于仿真步长仅为 0.1s，误差很小，在可接受范围内

    // ===== 恶意节点篡改数据 =====
    // 该模块负责生成恶意数据，具体逻辑根据项目要求生成，并将其写入 bsm
    if (isMalicious) {
        // 示例：使用固定的虚假位置（具体篡改逻辑按需求自行实现）
        static Coord fakePos = realPos;
        bsm->setPos(fakePos);
    } else {
        bsm->setPos(realPos);
    }
    bsm->setSpeed(realSpeed);
    bsm->setAccel(accel);
    bsm->setHeading(realHeading);

    // ===== 消息元信息 + 训练标签 =====
    // 关键：发送方把自己的标签放进消息
    bsm->setSenderAddress(myId);
    bsm->setSerial(msgSerial);
    bsm->setIsMaliciousLabel(isMalicious);

    // ===== 写入数据集文件 =====
    datasetFile << deltaT << ","
                << bsm->getPseudoId() << ","
                << bsm->getPos().x << "," << bsm->getPos().y << ","
                << bsm->getSpeed().x << "," << bsm->getSpeed().y << ","
                << bsm->getAccel().x << "," << bsm->getAccel().y << ","
                << bsm->getHeading().x << "," << bsm->getHeading().y << ","
                << (isMalicious ? 1 : 0) << "\n";
    // 在发送端执行数据写入操作，可避免数据集中出现重复数据

    sendDown(bsm);
    // sendDown：父类成员函数，执行后该车将信息广播到其他车辆，
    // 自动完成 MAC 帧的封装和发送，无需重写，直接使用即可
}
```

---

### 八、`ini` 文件配置

需要更改的地方已标明，没标注的地方直接使用即可。

```ini
[General]
# 基本不用动
cmdenv-express-mode = true
cmdenv-autoflush = true
cmdenv-status-frequency = 1s
**.cmdenv-log-level = info
network = RSUExampleScenario

##########################################################
#            Simulation parameters                       #
##########################################################
debug-on-errors = true
print-undisposed = true

# 仿真时间，根据实际更改
sim-time-limit = 200s

**.scalar-recording = true
**.vector-recording = true

# 地图大小
*.playgroundSizeX = 2500m
*.playgroundSizeY = 2500m
*.playgroundSizeZ = 50m


##########################################################
# Annotation parameters                                  #
##########################################################
*.annotations.draw = true

##########################################################
# Obstacle parameters                                    #
##########################################################
*.obstacles.obstacles = xmldoc("config.xml", "//AnalogueModel[@type='SimpleObstacleShadowing']/obstacles")

##########################################################
#            TraCIScenarioManager parameters             #
##########################################################
*.manager.updateInterval = 1s
*.manager.host = "localhost"
*.manager.port = 9999
*.manager.autoShutdown = true
# 这里改为自己的 launched 文件
*.manager.launchConfig = xmldoc("map.launched.xml")

##########################################################
#                       RSU SETTINGS                     #
##########################################################
*.rsu[0].mobility.x = 2000
*.rsu[0].mobility.y = 2000
*.rsu[0].mobility.z = 3

*.rsu[*].applType = "TraCIDemoRSU11p"
*.rsu[*].appl.headerLength = 80 bit
*.rsu[*].appl.sendBeacons = false
*.rsu[*].appl.dataOnSch = false
*.rsu[*].appl.beaconInterval = 1s
*.rsu[*].appl.beaconUserPriority = 7
*.rsu[*].appl.dataUserPriority = 5
*.rsu[*].nic.phy80211p.antennaOffsetZ = 0 m

##########################################################
#            11p specific parameters                     #
#                    NIC-Settings                        #
##########################################################
*.connectionManager.sendDirect = true
*.connectionManager.maxInterfDist = 2600m
*.connectionManager.drawMaxIntfDist = false

*.**.nic.mac1609_4.useServiceChannel = false

*.**.nic.mac1609_4.txPower = 20mW
*.**.nic.mac1609_4.bitrate = 6Mbps
*.**.nic.phy80211p.minPowerLevel = -110dBm

*.**.nic.phy80211p.useNoiseFloor = true
*.**.nic.phy80211p.noiseFloor = -98dBm

*.**.nic.phy80211p.decider = xmldoc("config.xml")
*.**.nic.phy80211p.analogueModels = xmldoc("config.xml")
*.**.nic.phy80211p.usePropagationDelay = true

*.**.nic.phy80211p.antenna = xmldoc("antenna.xml", "/root/Antenna[@id='monopole']")
*.node[*].nic.phy80211p.antennaOffsetY = 0 m
*.node[*].nic.phy80211p.antennaOffsetZ = 1.895 m

##########################################################
#                      App Layer                         #
##########################################################
# 这里改为自己的应用层
*.node[*].applType = "VehicleApp"
# 定义 ned 层里的参数，如恶意节点比例、随机数种子等
*.node[*].appl.headerLength = 80 bit
*.node[*].appl.sendBeacons = false
*.node[*].appl.dataOnSch = false
*.node[*].appl.beaconInterval = 1s

##########################################################
#                      Mobility                          #
##########################################################
*.node[*].veinsmobility.x = 0
*.node[*].veinsmobility.y = 0
*.node[*].veinsmobility.z = 0
*.node[*].veinsmobility.setHostSpeed = false
*.node[*0].veinsmobility.accidentCount = 1
*.node[*0].veinsmobility.accidentStart = 73s
*.node[*0].veinsmobility.accidentDuration = 50s

[Config Default]

[Config WithBeaconing]
*.rsu[*].appl.sendBeacons = true
*.node[*].appl.sendBeacons = true

[Config WithChannelSwitching]
*.**.nic.mac1609_4.useServiceChannel = true
*.node[*].appl.dataOnSch = true
*.rsu[*].appl.dataOnSch = true
```

---

### 九、有关注意事项和补充

1. **数据写入建议**：将数据写入文件时，最好每辆车一个文件，最后再通过 Python 脚本进行合并。因为如果同一时刻两个车辆同时写入同一个文件，可能导致数据串行或缺失。

2. **仿真崩溃排查**：如果仿真在开始后不久就崩溃，大概率是代码中出现了指针问题（如连续申请或释放同一块内存、使用空指针等）。

3. **提高运行速度**：生成数据时建议关闭图形模式。点击三角运行按钮，进入配置页面，将 **User interface** 中的 `Qtenv` 改为 `Cmdenv` 即可使用命令行模式运行。

4. **最后一次 Beacon 缺失问题**：每个车辆的最后一次 Beacon 消息可能出现数据缺失，这是由于仿真时间停止导致的，直接删除最后几条异常数据即可。
