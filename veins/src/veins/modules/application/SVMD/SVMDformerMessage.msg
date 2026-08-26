//
// Copyright (C) 2011 David Eckhoff <eckhoff@cs.fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
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

import veins.base.utils.Coord;
import veins.modules.messages.BaseFrame1609_4;
import veins.base.utils.SimpleAddress;

namespace veins;

packet SVMDformerMessage extends BaseFrame1609_4 {
    LAddress::L2Type senderAddress = -1; // 真实发送节点的链路层地址
    simtime_t deltaT;          // 与该节点上一条发送消息的时间间隔
    long pseudoId;             // 对外使用的伪身份标识
    
    Coord pos;                 // pos (x,y) 位置坐标
    Coord speed;               // spd (x,y) 速度
    Coord accel;               // acl (x,y) 加速度
    Coord heading;             // hed (x,y) 航向角

    long serial = 0;           // 消息序列号
    bool isMaliciousLabel = false; // 数据集中的恶意节点标签
}