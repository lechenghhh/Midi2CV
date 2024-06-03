# Midi2CV_Trs V4.0

Eurorack 自研发基于Arduino nano的强大Midi2CV模块  
Trs Midi2CV 3HP宽度 40mm深度  

## 功能与参数介绍:  

1xTrs Midi In(可通过跳线盘切换Trs TypeA/B)  
1xClock Out(Midi时钟) 另可通过cc24调整clock倍速**  
2xPoly Out(2复音)(CV范围0-5V)(3.0版本改为midi Ch1 与midi Ch2 更适合音序器使用)  
2xGate Out(与音对应的门)  
3xCV Out(Midi CC信息转化而来的CV 有五种模式)  

*clock功能需要设备开启Clock Send(时钟同步发送)  否则将无法收到时钟讯号  
*clock sync功能设备开启Transport Send(走带控制发送) 否则无法根据播放同步时钟,时钟将自行周期触发  

## Midi发送设备上的CC10可以切换以下4种模式  
0.      模式名称      Clock       Note1         Gate1           Note2         Gate2         CV1         CV2       CV3
1.      通道1/2模式   Clock       Ch1Note       Ch1NoteOn       Ch2Note       Ch2NoteOn     Vel1        Vel2      Mod
2.      通道3/4模式   Clock1/2    Ch3Note       Ch3NoteOn       Ch4Note       Ch4NoteOn     Vel1        Vel2      Mod
3.      复音模式      Clock       Ch1Poly1      Ch1Poly1On      Ch1Poly2      Ch1Poly2On    Vel1        Vel2      Mod   

新功能: 当设备20s内无midi响应时 自动进入随机触发模式
该模式下 clk接口将作为触发输入口 note1 gate1 note2 gate2 为80%/60%/40%/20%的概率 同时cc1 cc2 cc3则为一定范围的SnH信号
当midi信号收到时 将立刻回到midi2cv的基础功能

以上所有功能未来都可以通过固件升级/改进程序拓展  

## Arduino IDE 程序更新操作说明(Midi2CV_Trs 需要取下跳线帽)  

https://github.com/FortySevenEffects/arduino_midi_library   这个是arduino midi库 需要下载zip 并用arduino的库管理器安装该zip  
打开项目中对应的 .ino 文件(使用Arduino IDE打开)  
左上角有个框 点击后下拉选中您的usb连接的com口号码 然后找到 Arduino Nano 右边usb需要再次选择一下com口号码 然后按确定  
上方工具/tools > Processror > Atmega168(或328p等 可以都试试看)  
注:以上对arduino ide的操作做一次之后不用再做 除非你换了个ino打开则需要重新选择  
点击左上方第二个叫做 上传 的按钮 等待右下方的提示信息直到提示 上传完成 并重新装回跳线帽 整个上传程序流程完成  


