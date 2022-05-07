# 淋浴间主机引脚配置  

## 修改历史  
|版本|日期|说明|作者|  
|----|----|----|----|  
|V1.0|2022年4月16日|创建淋浴间主机引脚配置|马博阳|  
|V1.1|2022年4月20日|修改部分引脚名|马博阳|  
|V1.2|2022年4月25日|分配引脚|马博阳|  

## 电源  
1. +5V电源：+5V  
2. -5V电源：-5V  
3. 3.3V电源：3V3  
4. 地：GND  
5. 模拟/信号地：AGND  

## 信号输入  
1. 光照传感器：LIGHT_SENSOR **- PC2**  
2. 温湿度传感器：TEMPERATURE&HUMIDITY  **- PA1**  

## 信号输出  
1. 灯光控制：LIGHT_CONTROL **- PA4**  
2. 通风控制：FAN_CONTROL **- PA3**  

## 人机接口  
1. 报修按键：B1 **- PA5**  
2. 求助按键：B2 **- PA6**  
3. LCD显示屏：LCD_SDA **- PB7**，LCD_SCL **- PB6**  
4. 扬声器：SPEAKER_DATA **- PB3**，SPEAKER_CLK **- PB10**  

## 通信  
1. WiFi：WiFi_RX **PC6**，WiFi_TX **PC7**  
2. ZigBee：ZigBee_RX **- PA9**，ZigBee_TX **- PA10**  
