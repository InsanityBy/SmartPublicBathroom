# 项目开发规范  
## 修改历史  
|版本|日期|说明|作者|  
|----|----|----|----|  
|V1.0|2022年4月1日|创建项目开发规范|马博阳|  
|V1.1|2022年4月9日|模块部分未完成译名的补充|马博阳|  
## 简述  
为实现团队合作进行的项目硬件开发、代码编写和文档编写在各自负责的模块之间规范和统一，制定本规范。在项目开发前仔细阅读，并在开发过程中严格遵守，不符合本规范的文件提交将被拒绝。  
## 命名规范  
### 基本要求  
1. 中文的说明文档内使用中文名称；  
2. 文件夹、硬件设计、代码等使用英文名称；  
3. 在文件、文件夹、变量、函数命名时英文不加空格，有从属关系可加下划线连接，但在注释和说明中单词间必须加空格；  
4. 英文名称均遵循以下原则：使用能清楚描述功能的短语，除冠词、介词等无意义单词外，首字母大写。  
### 标准名称  
#### 项目名称：  
公共浴室智能管理系统（Smart Public Bathroom）  
#### 设备名称：  
淋浴器终端（Shower Terminal）  
淋浴间主机（Shower Host）  
云端服务平台（Cloud Service）  
#### 模块名称：  
信号输入（Signa Input）  
信号输出（Signal Output）  
人机接口（Human Machine Interface）  
通信（Communicate）  
电源（Power）  
外壳（Enclosure）  
数据库（Database）  
预约管理系统（Reservation Management System）  
温度传感器（Temperature Sensor）  
湿度传感器（Humidity Sensor）  
光照传感器（Light Sensor）  
LCD显示 （LCD）  
控制驱动（Control & Drive）  
### 示例  
项目文件夹结构  
>SmartPublicBathroom  
>>0_Project  
>>>CloudService  
>>>Document  
>>>ShowerHost  
>>>ShowerTerminal  
>>  
>>1_OpeningReport  
>>>Document  
>>>Image  
>>>Presentation  
>>>Reference  
>>  
>>2_DesignReview  
>>3_MidtermReview  
>>4_FinalReport  
## 代码规范  
1. 文件开头用注释写明：模块名、作者、最后修改时间、作者、模块功能和接口函数介绍，最后附上修改记录；  
2. 头文件需添加代码避免重复包含；  
3. 模块代码编写必须分为.h和.c文件，为主程序或其他模块提供调用和接口的函数和变量必须在.h中声明，并写清注释和使用说明；  
4. 变量和函数命名：英语全称，不可使用拼音或缩写；不加空格，有从属关系可加下划线连接，但在注释和说明中单词间必须加空格；使用能清楚描述功能的短语，除冠词、介词等无意义单词外，首字母大写；  
5. 关键代码写英文注释说明，函数注释说明功能、参数和返回值。  
## 文档规范  
1. `README.md`存放在每个模块文件夹根目录下，其他文档存放在`Document`文件夹下，文档根据内容命名如：`TemperatureSensor_Design`、`TemperatureSensor_UserManual`；  
2. `README.md`分为以下几部分：  
   - 标题  
   - 修改历史  
   - 模块功能描述  
   - 模块接口定义  
   - 模块使用说明  
   - 本文件夹结构及各文件、文档内容简介  
3. 所有的文档必须在标题后用表格列出修改历史。  
## 上传规范  
git上传必须加中英文备注，说明此次上传的目的（实现某功能、修改某功能、修改bug等）。