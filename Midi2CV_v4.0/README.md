# Midi2CV_Trs V4.0

Eurorack 自研发基于Arduino nano的强大Midi2CV模块  
Trs Midi2CV 2HP宽度 40mm深度  

## 功能与参数介绍:  

1xTrs Midi In(可通过跳线盘切换Trs TypeA/B)  
1xClock Out(Midi时钟) 另可通过cc24调整clock倍速**  
2xPoly Out(2复音)(CV范围0-5V)(3.0版本改为midi Ch1 与midi Ch2 更适合音序器使用)  
2xGate Out(与音对应的门)  
3xCV Out(Midi CC信息转化而来的CV 有五种模式)  

*clock功能需要设备开启Clock Send(时钟同步发送)  否则将无法收到时钟讯号  
*clock sync功能设备开启Transport Send(走带控制发送) 否则无法根据播放同步时钟,时钟将自行周期触发  

## Midi发送设备上的CC10可以切换以下4种模式  
|模式名称/接口名称|Clock|Note1|Gate1|Note2|Gate2|CV1|CV2|CV3|
|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|
|通道1&2模式|Clock|Ch1NotePitch|Ch1NoteOn|Ch2NotePitch|Ch2NoteOn|Vel1|Vel2|Mod|
|通道3&4模式|Clock1/2|Ch3NotePitch|Ch3NoteOn|Ch4NotePitch|Ch4NoteOn|Vel1|Vel2|Mod|
|复音模式|Clock|Ch1Poly1Pitch|Ch1Poly1On|Ch1Poly2Pitch|Ch1Poly2On|Vel1|Vel2|Mod|   
|概率触发模式-10s无midi进入|Trig In|80%RandTrig|60%RandTrig|40%RandTrig|GateRandLength|RandVoltage|RandVoltageInv|RandVoltage1/2|

10s无midi响应时(如果任意时刻收到midi信号则即刻回到midi模式)  
敬请期待更多更新

## Arduino IDE 程序更新操作说明(Midi2CV_Trs 需要取下跳线帽)  

https://github.com/FortySevenEffects/arduino_midi_library   这个是arduino midi库 需要下载zip 并用arduino的库管理器安装该zip  
打开项目中对应的 .ino 文件(使用Arduino IDE打开)  
左上角有个框 点击后下拉选中您的usb连接的com口号码 然后找到 Arduino Nano 右边usb需要再次选择一下com口号码 然后按确定  
上方工具/tools > Processror > Atmega168(或328p等 可以都试试看)  
注:以上对arduino ide的操作做一次之后不用再做 除非你换了个ino打开则需要重新选择  
点击左上方第二个叫做 上传 的按钮 等待右下方的提示信息直到提示 上传完成 并重新装回跳线帽 整个上传程序流程完成  


