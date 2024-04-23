# Midi2CV_Trs V4.0

Eurorack 自研发基于Arduino nano的强大Midi2CV模块  
Trs Midi2CV 3HP宽度 40mm深度  

## 功能与参数介绍:  

1xTrs Midi In(可通过跳线盘切换Trs TypeA/B)  
1xClock Out(Midi时钟) 另可通过cc11调整clock倍速(待测试)  
2xPoly Out(2复音)(CV范围0-5V)(3.0版本改为midi Ch1 与midi Ch2 更适合音序器使用)  
2xGate Out(与音对应的门)  
3xCV Out(Midi CC信息转化而来的CV 有五种模式)  

## Midi发送设备上的CC10可以切换以下4种模式  
0.      模式名称      Clock       Note1         Gate1           Note2         Gate2         CV1         CV2       CV3
1.      通道1/2模式   Clock       Ch1Note       Ch1NoteOn       Ch2Note       Ch2NoteOn     Vel1        Vel2      Mod
2.      通道3/4模式   Clock1/2    Ch3Note       Ch3NoteOn       Ch4Note       Ch4NoteOn     Vel1        Vel2      Mod
3.      音序器模式    Clock       Ch1Note       Ch1NoteOn       SeqPitch      SeqGate       Vel1        Seq2      Mod
4.      复音模式      Clock       Ch1Poly1      Ch1Poly1On      Ch1Poly2      Ch1Poly2On    Vel1        Vel2      Mod   

以上所有功能未来都可以通过固件升级/改进程序拓展  

### 内部音序器说明: 
##### 请先将d12跳线帽插上(旧版可能没有这个跳线配置)或者将cc10 调整到40; 此时将开启内部音序器,使用note2/gate2/cc2来输出音序器的控制信息  
触碰cc101-cc116表示选中step1-step16  
选中某step之后,再调整以下旋钮则修改当前step的----  
`cc21->Pitch`  
`cc22->Gate`  
`cc23->Velocity`  
在任何step之下,都可以调整音序器的以下状态----  
`cc24->clock div`(此参数也会修改midi2cv模块的总时钟输出间隔)  
`cc25->step length`(0-16)  
`cc26->current page`(暂未开放)  
`cc27->loop mode`(顺序/倒序/随机/随机2)  
`cc28->play state`(播放/暂停/停止)  

## Arduino IDE 程序更新操作说明(Midi2CV_Trs 需要取下跳线帽)  

https://github.com/FortySevenEffects/arduino_midi_library   这个是arduino midi库 需要下载zip 并用arduino的库管理器安装该zip  
打开项目中对应的 .ino 文件(使用Arduino IDE打开)  
左上角有个框 点击后下拉选中您的usb连接的com口号码 然后找到 Arduino Nano 右边usb需要再次选择一下com口号码 然后按确定  
上方工具/tools > Processror > Atmega168(或328p等 可以都试试看)  
注:以上对arduino ide的操作做一次之后不用再做 除非你换了个ino打开则需要重新选择  
点击左上方第二个叫做 上传 的按钮 等待右下方的提示信息直到提示 上传完成 并重新装回跳线帽 整个上传程序流程完成  


