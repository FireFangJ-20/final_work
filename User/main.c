#include "stm32f10x.h"                  // Device header
#include "led.h"
#include "usart.h"
#include "temhum.h"


#include "Delay_OLED.h"
#include "OLED.h"


/*       连接方法

OLED   GND   o  o   G     stm32
       VCC   o  o   3.3
	   SCL   o  o   B8
	   SDA   o  o   B9
	   

dht20  +     o  o   3.3   stm32
       -     o  o   G
	   SCL   o  o   B6
	   SDA   o  o   B7

esp    3v3   o  o   3.3
       RX    o  o   A10
       TX    o  o   A9
       GND   o  o   G
   
*/

const char* WIFINAME ="Redmi K40";                  //需要连接的wifi名称
const char* WIFIPSWD ="";                  //wifi密码
const char* CLINTID  ="7HLNUK07OPesp8266";          //客户端ID
const char* USRNAME  ="7HLNUK07OPesp8266;12010126;CUUXJ;1662459201";              //腾讯云标准用户名
const char* PASSWRD  ="e741954d40edf0776269de464182a7030b80183373f96479f170be53d72410d7;hmacsha256";     //腾讯云标准密码
const char* MQTSVER  ="7HLNUK07OP.iotcloud.tencentdevices.com";                   //腾讯云服务器
const char* PUBTOPC  ="$thing/up/property/7HLNUK07OP/esp8266";                    //发布消息所订阅的主题


 int main(void)
 {	 	
    u32 CT_data[2]={0};        //声明数组，CT_data[0]存湿度，[1]存温度
    volatile float  hum=0,tem=0;     
     

	
    delay_init();	    	 //延时函数初始化	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	     //串口1初始化为115200
    LED_Init();			     //LED端口初始化
    temphum_init();          //DTH20初始化    
	
	
	OLED_Init();             //OLED初始化
	
	printf("AT+RST\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"RST");    //重置
	
	printf("ATE0\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"E0");     //关闭回显
	
	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"1");      //esp选择station模式
		
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFINAME,WIFIPSWD);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"WIFI");           //连接wifi                    
	
    printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",CLINTID,USRNAME,PASSWRD);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"user");          //设置客户端信息
	
	printf("AT+MQTTCONN=0,\"%s\",1883,1\r\n",MQTSVER);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"CONN");         //连接腾讯云服务器
	
	float t,h;
	
	
	while(1)
	{
        AHT20_Read_CTdata(CT_data);       //读取AHT20的温度和湿度 

        hum = CT_data[0]*100*10/1024/1024;  //计算得到湿度值（放大10倍）
        tem = CT_data[1]*200*10/1024/1024-500;//计算得到温度值（放大10倍）

        
        
		
		
		OLED_ShowString(1,1,"hum");
		
		OLED_ShowNum(2,1,hum/10,2);
		OLED_ShowChar(2,3,'.');
		OLED_ShowNum(2,4,hum,1);		
		OLED_ShowChar(2,6,'%');
		
				
		OLED_ShowString(3,1,"tem");
		
		OLED_ShowNum(4,1,tem/10,2);
		OLED_ShowChar(4,3,'.');
		OLED_ShowNum(4,4,tem,1);
		OLED_ShowString(4,6,"^C");           //OLED显示温湿度
		
		t=tem/10.0f;
		h=hum/10.0f;
		
		
		
		printf("AT+MQTTPUBRAW=0,\"$thing/up/property/7HLNUK07OP/esp8266\",98,1,0\r\n");
		delay_ms(1000);
		printf("{\"method\":\"report\",\"clientToken\":\"123\",\"timestamp\":1628646783,\"params\":{\"temp\":%4.1f,\"humit\":%4.1f}}",t,h);
		delay_ms(1000);
		
		                                //发布消息，上传腾讯云平台
		
		
        LED=0;
        delay_ms(100);
        LED=1;
        delay_ms(100);                 //板载LED闪烁
	 }

}
