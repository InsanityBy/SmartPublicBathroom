# 淋浴器终端引脚配置  

## 修改历史  
|版本|日期|说明|作者|  
|----|----|----|----|  
|V1.0|2022年4月16日|创建淋浴器终端引脚配置|马博阳|  
|V1.1|2022年4月20日|修改部分引脚名|马博阳|  
|V1.2|2022年4月25日|分配引脚|马博阳|  

## 电源  
1. +5V电源：+5V  
2. -5V电源：-5V  
3. 3.3V电源：3V3  
4. 地：GND  
5. 模拟/信号地：AGND  

## 信号输入  
1. 流量计：FLOWMETER **- PA0**  
2. 温度传感器：TEMPERATURE  **- PA1**  
3. 红外传感器人体：INFRARED_HUMAN_RX **- PA2**，INFRARED_HUMAN_TX  **- PA8**  
4. 红外传感器物体：INFRARED_OBJECT_RX **- PC2**，INFRARED_OBJECT_TX **- PA3**  

## 信号输出  
1. 电磁阀控制：VALVE_CONTROL **- PA4**  

## 人机接口  
1. 报修按键：B1 **- PA5**  
2. 求助按键：B2 **- PA6**  
3. LCD显示屏：LCD_SDA **- PB7**，LCD_SCL **- PB6**  
4. 扬声器：SPEAKER_DATA **- PB3**，SPEAKER_CLK **- PB10**  
5. NFC读卡器：NFC_RST  **- PA7**，NFC_IRQ **- PB0**，NFC_MISO **- PB14**，NFC_MOSI **- PB15**，NFC_SCK **- PB13**，NFC_NSS **- PB12**  

## 通信  
1. ZigBee：ZigBee_RX **- PA9**，ZigBee_TX **- PA10**  
