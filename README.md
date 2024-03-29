# Midi2CV_Trs 

#### 版本说明 (程序请前往对应目录进行上传升级)
`V2.0` 2复音版本  
`V3.0` 2通道版本 ch1 ch2   3xcv: vel mod cc2   
`V3.1` 2通道版本 ch1 ch2   3xcv: vel1 vel2 mod  

## Arduino IDE 程序更新操作说明(Midi2CV_Trs 需要取下跳线帽)
https://github.com/FortySevenEffects/arduino_midi_library   这个是arduino midi库 需要下载zip 并用arduino的库管理器安装该zip  
打开项目中对应的 .ino 文件(使用Arduino IDE打开)  
左上角有个框 点击后下拉选中您的usb连接的com口号码 然后找到 Arduino Nano 右边usb需要再次选择一下com口号码 然后按确定  
上方工具/tools > Processror > Atmega168(新版选Atmega328p Old)  
注:以上操作做一次之后不用再做 除非你换了个ino打开则需要重新选择  
点击左上方第二个叫做 上传 的按钮 等待右下方的提示信息直到提示 上传完成 并重新装回跳线帽 整个上传程序流程完成  

## 拓展说明:
以上所有功能未来都可以通过固件升级/改进程序拓展  
增加程序滑音 通过某CC来控制滑音量  
增加Arp功能 通过某CC来选择开启并改变模式  
增加播控按钮映射到CV上   
编程使之成为一个程序音序器,通过Midi键盘上某些note cc触发来使程序记录下来数值与时间通过某些旋钮来改变每个音序的参数并且允许实时录制等  
(这方面工作量比较大,未来考虑拓展与开源,也欢迎懂程序的模块玩家一起来贡献)  
注:程序基于Arduino Nano,C语言开发,依赖Arduino Midi Library; 如需修改程序 请取下Tx接口旁的跳线帽.如果不取下会无法上传程序  