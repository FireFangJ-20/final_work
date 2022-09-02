#include "stm32f10x.h"                  // Device header
#include "led.h"
#include "usart.h"
#include "temhum.h"


#include "Delay_OLED.h"
#include "OLED.h"


/*       ���ӷ���

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

const char* WIFINAME ="Redmi K40";                  //��Ҫ���ӵ�wifi����
const char* WIFIPSWD ="";                  //wifi����
const char* CLINTID  ="7HLNUK07OPesp8266";          //�ͻ���ID
const char* USRNAME  ="7HLNUK07OPesp8266;12010126;CUUXJ;1662459201";              //��Ѷ�Ʊ�׼�û���
const char* PASSWRD  ="e741954d40edf0776269de464182a7030b80183373f96479f170be53d72410d7;hmacsha256";     //��Ѷ�Ʊ�׼����
const char* MQTSVER  ="7HLNUK07OP.iotcloud.tencentdevices.com";                   //��Ѷ�Ʒ�����
const char* PUBTOPC  ="$thing/up/property/7HLNUK07OP/esp8266";                    //������Ϣ�����ĵ�����


 int main(void)
 {	 	
    u32 CT_data[2]={0};        //�������飬CT_data[0]��ʪ�ȣ�[1]���¶�
    volatile float  hum=0,tem=0;     
     

	
    delay_init();	    	 //��ʱ������ʼ��	  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	     //����1��ʼ��Ϊ115200
    LED_Init();			     //LED�˿ڳ�ʼ��
    temphum_init();          //DTH20��ʼ��    
	
	
	OLED_Init();             //OLED��ʼ��
	
	printf("AT+RST\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"RST");    //����
	
	printf("ATE0\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"E0");     //�رջ���
	
	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"1");      //espѡ��stationģʽ
		
	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFINAME,WIFIPSWD);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"WIFI");           //����wifi                    
	
    printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",CLINTID,USRNAME,PASSWRD);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"user");          //���ÿͻ�����Ϣ
	
	printf("AT+MQTTCONN=0,\"%s\",1883,1\r\n",MQTSVER);
	delay_ms(1000);
	OLED_Clear();
	OLED_ShowString(1,1,"CONN");         //������Ѷ�Ʒ�����
	
	float t,h;
	
	
	while(1)
	{
        AHT20_Read_CTdata(CT_data);       //��ȡAHT20���¶Ⱥ�ʪ�� 

        hum = CT_data[0]*100*10/1024/1024;  //����õ�ʪ��ֵ���Ŵ�10����
        tem = CT_data[1]*200*10/1024/1024-500;//����õ��¶�ֵ���Ŵ�10����

        
        
		
		
		OLED_ShowString(1,1,"hum");
		
		OLED_ShowNum(2,1,hum/10,2);
		OLED_ShowChar(2,3,'.');
		OLED_ShowNum(2,4,hum,1);		
		OLED_ShowChar(2,6,'%');
		
				
		OLED_ShowString(3,1,"tem");
		
		OLED_ShowNum(4,1,tem/10,2);
		OLED_ShowChar(4,3,'.');
		OLED_ShowNum(4,4,tem,1);
		OLED_ShowString(4,6,"^C");           //OLED��ʾ��ʪ��
		
		t=tem/10.0f;
		h=hum/10.0f;
		
		
		
		printf("AT+MQTTPUBRAW=0,\"$thing/up/property/7HLNUK07OP/esp8266\",98,1,0\r\n");
		delay_ms(1000);
		printf("{\"method\":\"report\",\"clientToken\":\"123\",\"timestamp\":1628646783,\"params\":{\"temp\":%4.1f,\"humit\":%4.1f}}",t,h);
		delay_ms(1000);
		
		                                //������Ϣ���ϴ���Ѷ��ƽ̨
		
		
        LED=0;
        delay_ms(100);
        LED=1;
        delay_ms(100);                 //����LED��˸
	 }

}
