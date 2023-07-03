# Midi2CV_Trs

Eurorack 自研发基于Arduino nano的强大Midi2CV模块
Trs Midi2CV 3HP宽度 40mm深度

功能与参数介绍:
1xTrs Midi In(可通过跳线盘切换Trs TypeA/B)
1xClock Out(Midi时钟)
2xPoly Out(2复音)(CV范围0-5V)
2xGate Out(与复音对应的门)
3xCV Out(Midi CC信息转化而来的CV 有五种模式)

CC2CV功能详解: Midi设备上的CC11可以切换5种模式
模式1: Mod->CV1 Vel->CV2 AfterTouch->CV3
模式2: CC12->CV1 CC13->CV2 CC14->CV3
模式3: CC22->CV1 CC23->CV2 CC24->CV3
模式4: CC32->CV1 CC33->CV2 CC34->CV3
模式5: Note1->CV1 Note2->CV2 Note3->CV3
(以上映射关系皆可以通过程序再改变 模式也可以不局限于5个)

玩法说明:
1.Midi键盘演奏时可以即时切换模式 一会用Vel/mod/AT 一会用旋钮 拓展控制范围
2.使用Midi音序器提前录制好多个CC的自动化 演奏时即兴切换模式 骤变控制信号
3.切换到模式5 使旋律音序器变成一个5轨道的鼓音序器

拓展说明:
以上所有功能都可以通过固件升级/改进程序大大拓展
增加程序滑音 通过某CC来控制滑音量
增加Arp功能 通过某CC来选择开启并改变模式
增加播控按钮映射到CV上 
编程使之成为一个程序音序器,通过Midi键盘上某些note cc触发来使程序记录下来数值与时间通过某些旋钮来改变每个音序的参数并且允许实时录制等
(这方面工作量比较大,未来考虑拓展与开源,也欢迎懂程序的模块玩家一起来贡献)
注:程序基于Arduino Nano,C语言开发,依赖Arduino Midi Library; 如需修改程序 请取下Tx接口旁的跳线帽.如果不取下会无法上传程序
