# 基于 OMNeT++ + SUMO + Veins 的车联网仿真实验

[![OMNeT++](https://img.shields.io/badge/OMNeT++-5.x-blue.svg)](https://omnetpp.org/)
[![SUMO](https://img.shields.io/badge/SUMO-1.11.0-green.svg)](https://www.eclipse.org/sumo/)
[![Veins](https://img.shields.io/badge/Veins-5.x-orange.svg)](http://veins.car2x.org/)
[![License](https://img.shields.io/badge/License-GPL--2.0--or--later-blue.svg)](LICENSE)

本项目基于 **OMNeT++ + SUMO + Veins** 三大仿真框架协同，提供完整的车联网（VANET）V2V/V2I 通信仿真与数据集生成方案。项目包含从路网生成、交通流配置、应用层开发到数据采集的全流程实现，并内置多种异常/攻击场景的数据集样例，适用于车联网安全、异常检测、消息伪造检测等研究方向。

## 目录

- [项目特点](#项目特点)
- [技术架构](#技术架构)
- [环境要求](#环境要求)
- [目录结构](#目录结构)
- [快速开始](#快速开始)
- [使用教程](#使用教程)
- [数据集说明](#数据集说明)
- [注意事项](#注意事项)
- [参考文献](#参考文献)
- [许可证](#许可证)

## 项目特点

- **全流程仿真支持**：从 OpenStreetMap 地图导出 → SUMO 路网与交通流生成 → Veins 协议栈通信仿真 → CSV 数据集采集，全链路打通
- **三大框架协同**：OMNeT++（离散事件网络仿真）+ SUMO（道路交通流仿真）+ Veins（车联网 IEEE 802.11p 框架）
- **支持恶意节点仿真**：可配置恶意车辆比例，内置多种数据篡改攻击类型（位置伪造、速度篡改等共 18 种攻击场景）
- **自定义消息结构**：通过 `.msg` 文件定义数据集特征字段，编译自动生成 C++ 类，灵活扩展
- **完整的代码模板**：提供 NED、MSG、H、CC、XML 等所有配置文件的可复用模板
- **已包含样例数据集**：`manhattn` 示例中包含多组异常数据集（attackType01 ~ attackType18）

## 技术架构

```
┌─────────────────────────────────────────────────────────────┐
│                        数据集输出层                          │
│                    (CSV 格式 / 多车合并)                    │
├─────────────────────────────────────────────────────────────┤
│                        应用层开发                            │
│   自定义 .msg / .h / .cc / .ned → 重写 DemoBaseApplLayer    │
├─────────────────────────────────────────────────────────────┤
│                        Veins 框架                            │
│   IEEE 802.11p 协议栈 / V2V+V2I 通信 / TraCI 接口桥接       │
├─────────────────────┬───────────────────────────────────────┤
│      OMNeT++        │              SUMO                     │
│  离散事件仿真引擎    │      路网 + 交通流 + 车辆动力学        │
│  Qtenv / Cmdenv     │      net.xml / rou.xml / poly.xml     │
└─────────────────────┴───────────────────────────────────────┘
```

**Veins 应用层继承链**：

```
BaseLayer → BaseApplLayer → DemoBaseApplLayer → 自定义应用层（你的代码）
```

所有底层通用逻辑（初始化、消息分发、MAC 封装等）父类均已实现，仅需重写预留虚函数插入自定义逻辑。

## 环境要求

### 推荐配置（虚拟机方案）

由于三个软件的依赖和环境变量在 Windows 上容易冲突，**强烈建议使用 Veins 官方提供的预配置虚拟机**：

| 软件 | 版本 / 说明 |
| --- | --- |
| VirtualBox | 最新版 |
| Veins 虚拟机镜像 | 从 [veins.car2x.org/download](https://veins.car2x.org/download/) 下载后缀为 `i1` 的 `.ova` 文件 |
| 登录密码 | `veins` |

### 软件版本参考

- **OMNeT++**：5.x 系列（虚拟机已内置）
- **SUMO**：1.11.0（虚拟机已内置）
- **Veins**：5.x 系列（本项目基于此开发）

### 其他要求

- 足够的内存（建议 ≥ 4GB 分配给虚拟机）
- 多核心 CPU（批量仿真时提升速度）
- 磁盘空间：建议 ≥ 20GB（数据集生成可能占用较大空间）

## 目录结构

```
基于OMNeT+++SUMO+Veins的车联网仿真实验/
├── veins/                                          # Veins 框架源码及示例
│   ├── src/
│   │   └── modules/application/                    # 应用层代码目录（在此创建你的项目）
│   │       ├── ieee80211p/                         # 官方示例：DemoBaseApplLayer 等父类
│   │       └── traci/                              # 官方示例：TraCI 应用层
│   ├── examples/
│   │   └── manhattn/                               # 曼哈顿场景示例
│   │       ├── RSUExampleScenario.ned              # 网络拓扑 NED 文件
│   │       ├── config.xml                          # 物理层配置
│   │       ├── antenna.xml                         # 天线配置
│   │       └── datasets/                           # 样例数据集
│   │           ├── abnormalDataset_attackType01/   # 第1类攻击场景数据集
│   │           ├── abnormalDataset_attackType02/   # 第2类攻击场景数据集
│   │           └── ...                             # 共18种攻击场景
│   ├── doc/                                        # Veins 官方文档
│   ├── Makefile                                    # 编译规则（自动编译 application 下所有代码）
│   └── README.txt
├── 基于OMNeT++的veins教程.md                        # 详细开发教程（保姆级指南）
└── README.md                                       # 本文件
```

## 快速开始

### 第一步：启动环境

1. 导入 `.ova` 虚拟机镜像到 VirtualBox 并启动
2. 登录（密码 `veins`），点击左上角 **Activities** 菜单
3. 启动 `veins_launchd`（SUMO 通信守护进程）
4. 打开 **OMNeT++** IDE

### 第二步：准备基础文件（SUMO 侧）

在 `veins/examples/` 下创建你的项目文件夹，例如 `myProject/`，然后复制以下模板文件：

```bash
cd veins/examples/
mkdir myProject && cd myProject
# 复制基础配置模板（从官方示例或 manhattn 示例）
cp ../manhattn/config.xml .
cp ../manhattn/antenna.xml .
cp ../manhattn/RSUExampleScenario.ned .
# 复制 omnetpp.ini 模板（从官方示例）
cp ../veins/omnetpp.ini .
```

然后使用 SUMO 工具链生成路网文件：

```bash
# 1. 生成路网（从 OSM 地图）
netconvert --osm-files map.osm --output-file map.net.xml \
  --remove-edges.by-type "railway.rail,railway.subway,waterway.river,railway.platform,waterway.canal"

# 2. 生成建筑地形
polyconvert --osm-files map.osm --net-file map.net.xml \
  --type-file ~/src/sumo-1.11.0/data/typemap/osmPolyconvert.typ.xml \
  -o map.poly.xml

# 3. 生成车流路由（逐车生成）
python ~/src/sumo-1.11.0/tools/randomTrips.py \
  -n map.net.xml -r map.rou.xml \
  -b 0 -e 600 --period 2.55 --fringe-factor 10 \
  -s 1 --min-distance 3000 -o trips.trips.xml
```

### 第三步：编写配置文件

参照以下模板（只需改文件名即可）：

- **`.sumo.cfg`**：SUMO 总配置，关联 net/rou/poly 三个文件
- **`.launchd.xml`**：Veins-SUMO 连接配置，声明资源文件
- **`omnetpp.ini`**：仿真总控，配置应用层、RSU、通信参数等

所有配置文件模板详见 [教程文档 第五节](基于OMNeT++的veins教程.md#五有关配置文件代码模板)。

### 第四步：编写应用层代码

在 `veins/src/modules/application/` 下创建你的项目文件夹，包含 4 类文件：

| 文件类型 | 作用 |
| --- | --- |
| `XXX.ned` | 定义应用层模块、参数、绑定 C++ 类 |
| `XXX.msg` | 定义自定义消息字段（坐标、速度、标签等）→ 右键编译生成 `_m.h/.cc` |
| `XXX.h` | 头文件，声明自定义类和重写的虚函数 |
| `XXX.cc` | 实现文件，编写 initialize / handleSelfMsg / sendBSM / onWSM 等逻辑 |

具体代码模板详见 [教程文档 第六、七节](基于OMNeT++的veins教程.md#六有关业务代码没提到的代码块直接使用即可)。

### 第五步：编译并运行

1. 在 OMNeT++ 中右键项目 → **Build Project** 编译
2. 双击 `omnetpp.ini`，点击运行按钮
3. 在弹出的配置页中，建议将 **User interface** 选择为 `Cmdenv`（命令行模式，速度更快）
4. 仿真结束后，在项目目录下查看生成的 CSV 数据集

## 使用教程

详细的图文/代码级开发教程请参考：

👉 **[车联网数据集生成保姆级指南](基于OMNeT++的veins教程.md)**

教程涵盖以下内容：

1. **三大仿真软件的核心作用**与环境搭建
2. **文件分类**：SUMO 自动生成 vs 手动编写的文件
3. **文件架构与注意事项**：为什么代码要写在 `src/modules/application` 下
4. **Veins 父类框架**说明
5. **配置文件完整模板**：config.xml / sumo.cfg / antenna.xml / launchd.xml
6. **业务代码模板**：.ned / .msg / .h / .cc 文件骨架
7. **父类虚函数重写指南**：initialize / finish / handleSelfMsg / sendBSM 等
8. **omnetpp.ini 完整配置**：逐段标注需修改之处
9. **常见问题与排错**：崩溃排查、数据写入建议、性能优化等

## 数据集说明

### CSV 数据集字段格式（示例）

每个车辆发送一条 Beacon 消息即写入一行数据，字段顺序如下：

| 列号 | 字段名 | 类型 | 说明 |
| --- | --- | --- | --- |
| 1 | `deltaT` | double | 与上一条消息的时间间隔 |
| 2 | `pseudoId` | long | 车辆伪 ID（节点索引） |
| 3-4 | `pos_x`, `pos_y` | double | 位置坐标 (x, y) |
| 5-6 | `speed_x`, `speed_y` | double | 速度向量 (x, y) |
| 7-8 | `accel_x`, `accel_y` | double | 加速度向量 (x, y) |
| 9-10 | `heading_x`, `heading_y` | double | 航向角单位向量 (x, y) |
| 11 | `isMalicious` | int | 标签：0 = 正常，1 = 恶意 |

### 内置样例数据集

`veins/examples/manhattn/datasets/` 下已包含 **18 种攻击类型**的异常数据集，每种攻击类型文件夹内包含多辆被标记为异常的车辆消息 CSV 文件（`vehicle_abnormalXXX_messages.csv`），可直接用于模型训练与测试。

## 注意事项

### 编译相关

由于所有项目共用 `veins/src` 下的 Makefile，**`src/modules/application` 下你编写的所有项目代码必须全部编译通过**，即使当前运行的 ini 只用到其中一个项目的应用层。

### 数据写入

- 推荐**每辆车写入一个独立文件**，最后通过 Python 脚本合并，避免并行写入导致数据串行或丢失
- 在**发送端**写入数据（而不是接收端），可避免数据集中出现重复记录
- 仿真终止时可能导致每辆车的最后一条 Beacon 数据不完整，直接删除末尾异常行即可

### 运行速度优化

- 批量生成数据集时使用 **Cmdenv**（命令行模式）而非 Qtenv（图形模式）
- 在 `.sumo.cfg` 中关闭 XML 校验和 step 日志
- 适当减小 `sim-time-limit` 或降低车辆密度进行调试

### 崩溃排查

仿真刚开始就崩溃 → 大概率是**指针问题**：
- 重复 `cancelAndDelete` 同一个自消息
- 使用未初始化的指针
- `new` 与 `delete` 不配对

## 参考文献

- **Veins 官方网站**：[http://veins.car2x.org/](http://veins.car2x.org/)
- **OMNeT++ 官方文档**：[https://doc.omnetpp.org/](https://doc.omnetpp.org/)
- **SUMO 官方文档**：[https://sumo.dlr.de/docs/](https://sumo.dlr.de/docs/)
- **IEEE 802.11p**：车载环境无线接入（WAVE）标准

## 许可证

本项目基于 Veins 框架开发，遵循其许可证条款：

- 源代码部分：**GNU General Public License v2.0 (or later)**
- 非源代码部分（文档、配置等）：可选用 **Creative Commons Attribution-ShareAlike 4.0 International License**

详见各源文件头部的许可证声明，以及 [veins/README.txt](veins/README.txt)。
