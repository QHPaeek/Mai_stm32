基于stm32f411ceu6设计的mai控制器

本设计基于STM32cubeIDE与HAL库进行开发，采用FreeRTOS，旨在最大程度减少延迟，同时实现尽可能多的功能。

**关于使用：**

1.将\`Release\F411_Mai.bin`使用STM32CubeProgrammer通过USB_DFU模式或者STlink刷入设备中。

2.重新插拔设备，按说明分配COM口序号

3.编辑mai2.ini使其停止Hook串口。(DummyTouchPanel=1)

4.直接启动游戏。ENJOY！

**关于软件：**

依赖[AL94_USB_Composite]([https://github.com/alambe94/I-CUBE-USBD-Composite)库实现USB多设备复合工作。请在CUBEIDE中正确安装这个库。

**关于IO：**

触摸、灯光、按键各自占用一路串口，因此设备复合出了三路串口，需要分配到不同的串口号。

触摸：作为1P使用时，串口应分配于COM3，作为2P则分配到COM4。

灯光与按键当前版本未实现

插上设备之后默认出现3路串口，在没有修改过串口序号的情况下，COM数字最小的那一个是触摸，居中的是灯光，最大的是按键IO。

本设计的硬件尚未完善，目前只是能运行的DEMO版本。后续将持续不断更新

PB9:I2C1_SDA

PB8:I2C1_SCL

PB3:I2C2_SDA

PB10:I2C2_SCL

PB4:I2C3_SDA

PA8:I2C3_SDA

请在三路I2C上各接一个CY8CMBR3116模块。

**关于传感器：**

[cy8cmbr3116_touch_module - 嘉立创EDA开源硬件平台](https://oshwhub.com/affinelab/cy8cmbr3116_touch_module)

**关于主控：**

STM32F411CEU6开发板

**已知的BUG：**

1.AL94_USB_Composite库在生成代码时并不会保护USER CODE区域中的用户代码，而本程序需要在USB接收回调函数中实现功能。因此请手动保护`\F411_chunithm_15.6\Middlewares\Third_Party\AL94_USB_Composite\COMPOSITE\App`目录下的`usbd_cdc_acm_if.c`以及`usbd_cdc_acm_if.h`两个文件。
