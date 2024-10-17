# Midi2CV_Trig V2.0  

## 硬件及接口介绍:  

1xTrs Midi In(可通过跳线盘切换Trs TypeA/B)  
1xClock Out*(可通过cc24调整clock倍速**)  
7xGate Out(可通过D12跳线盘切换单通道模式和多通道模式)

*clock功能需要设备开启Clock Send(时钟同步发送)  否则将无法收到时钟讯号  
**clock sync功能设备开启Transport Send(走带控制发送) 否则无法根据播放同步时钟,时钟将自行周期触发  

## 软件功能介绍:   
#### 可以根据D12跳线帽来切换两种模式(插上时为多音符模式 取下为多通道模式),同时,模块的第一个输出接口被固定应用于clk信号的输出,同时可以通过cc24来调整clk的速率:  
第一种模式用于在CH1通道的不同音符来触发模块的gate输出,比如note21可以触发模块的第二个输出,note22可以触发模块的第三个输出,以此类推;  
第二种模式适用于多通道任意音符触发----比如ch1通道的任意音符可以触发模块的第二个输出,ch2通道的任意音符可以触发模块的第三个输出,以此类推.

|模式名称/接口名称|1|2|3|4|5|6|7|8|
|:-------------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|:-------:|
|CH1多音符模式|Clock|Note21|Note22|Note23|Note24|Note25|Note26|Note27|
|CH1-CH7任意音符|Clock|CH1AllNote|CH2AllNote|CH3AllNote|CH4AllNote|CH5AllNote|CH6AllNote|CH7AllNote|   


## 固件更新说明  
1.请安装arduino IDE  
2.请安装midi依赖库: https://github.com/FortySevenEffects/arduino_midi_library 下载zip 并用arduino IDE的库管理器安装该zip  
3.下载当前Midi2CV固件程序,打开项目中对应的.ino 文件(使用Arduino IDE打开)  
4.进入arduino IDE的界面,同时使用ttl上传工具连接设备的Rx tx 5v Gnd接口并使用另一端与计算机连接:  
界面左上角找到长方形框,点击后下拉选中您的usb连接的com口号码,然后在列表中找到Arduino Nano,右边usb需要再次选择一下com口号码 然后按确定  
上方菜单栏:工具/tools > Processror > Atmega168(或328p等 由于不同版本可能用的控制器版本略有差异,若有不成功则可以都试试看)  
以上对arduino IDE的选择设备操作只需做一次,如果更换.ino则需要重新选择  
5.点击左上方第二个叫做 上传 的按钮 等待右下方的提示信息直到提示 上传完成 并重新装回跳线帽 整个上传程序流程完成  






