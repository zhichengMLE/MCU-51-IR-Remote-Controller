#include <reg51.h>
#include <intrins.h>
#include "1602.h"
//#include "key.h"
#define uchar unsigned char
#define uint  unsigned int
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};
void keyscan();
void delay(uchar x);  //x*0.14MS
void delay1(int ms);
void beep();
void IR_IN();
sbit IRIN = P3^2;         //���������������
sbit BEEP = P1^5;         //������������
sbit RELAY= P1^4;         //�̵���������
unsigned char code LED7Code[] = {~0x3F,~0x06,~0x5B,~0x4F,~0x66,~0x6D,~0x7D,~0x07,~0x7F,~0x6F,~0x77,~0x7C,~0x39,~0x5E,~0x79,~0x71};

uchar IRCOM[8];
uchar IRAddr[2];
uchar IRRev[8];
uint key=10;

uchar code  cdis1[ ] = {"  IR-addr: --H  "};
uchar code  cdis2[ ] = {"  IR-ctrl: --H  "};


/*******************************************************************/
main()
{
 uchar m;
    P2=0xff;
    IRIN=1;                    //I/O�ڳ�ʼ��
    BEEP=1;
    RELAY=1; 
	
    delay1(10);                 //��ʱ
    lcd_init();                //��ʼ��LCD             
        
    lcd_pos(0);                //������ʾλ��Ϊ��һ�еĵ�1���ַ�

     m = 0;
    while(cdis1[m] != '\0')
     {                         //��ʾ�ַ�
       lcd_wdat(cdis1[m]);
       m++;
     }

    lcd_pos(0x40);             //������ʾλ��Ϊ�ڶ��е�1���ַ�
     m = 0;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]);      //��ʾ�ַ�
       m++;
     }


	IE = 0x81;                 //�������ж��ж�,ʹ�� INT0 �ⲿ�ж�
	TCON = 0x01;               //������ʽΪ���帺���ش���
  while(1)
	{
	
		keyscan();
 
	delay(10);
    	switch(key)								   //������Ӧֵ
		{
					
			case 1:P1= 0x08;break;
			case 2:P1= 0x3f;break;
			case 3:P1= 0x2d;break;
			case 4:P1= 0x08;break;
			case 5:P1= 0x10;break;
			case 6:P1= 0x20;break;
			case 7:P1= 0x40;break;
			case 8:P1= 0x80;break;
			default:break; 
		 
		}  
}
}//end main
/**********************************************************/
void IR_IN() interrupt 0 using 0
{
  unsigned char j,k,N=0;
     EX0 = 0;   
	 delay(15);
	 if (IRIN==1) 
     { EX0 =1;
	   return;
	  } 
                           //ȷ��IR�źų���
  while (!IRIN)            //��IR��Ϊ�ߵ�ƽ������9ms��ǰ���͵�ƽ�źš�
    {delay(1);}

 for (j=0;j<4;j++)         //�ռ���������
 { 
  for (k=0;k<8;k++)        //ÿ��������8λ
  {
   while (IRIN)            //�� IR ��Ϊ�͵�ƽ������4.5ms��ǰ���ߵ�ƽ�źš�
     {delay(1);}
    while (!IRIN)          //�� IR ��Ϊ�ߵ�ƽ
     {delay(1);}
     while (IRIN)           //����IR�ߵ�ƽʱ��
      {
    delay(1);
    N++;           
    if (N>=30)
	 { EX0=1;
	 return;}                  //0.14ms���������Զ��뿪��
      }                        //�ߵ�ƽ�������                
     IRCOM[j]=IRCOM[j] >> 1;                  //�������λ����0��
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //�������λ����1��
     N=0;
  }//end for k
 }//end for j
   
    if (IRCOM[0]!=~IRCOM[1])
   { EX0=1;
     return; }
 
 
   if (IRCOM[2]!=~IRCOM[3])
   { EX0=1;
     return; }

   IRCOM[4]=IRCOM[0] & 0x0F;     //ȡ����ĵ���λ
   IRCOM[5]=IRCOM[0] >> 4;       //����4�Σ�����λ��Ϊ����λ
    IRCOM[6]=IRCOM[2] & 0x0F; 
   IRCOM[7]=IRCOM[2] >> 4;     
		 if(IRCOM[4]>9)
    { IRCOM[4]=IRCOM[4]+0x37;}
   else
	  IRCOM[4]=IRCOM[4]+0x30;

   if(IRCOM[5]>9)
    { IRCOM[5]=IRCOM[5]+0x37;}
   else
	  IRCOM[5]=IRCOM[5]+0x30;

	 if(IRCOM[6]>9)
    { IRCOM[6]=IRCOM[6]+0x37;}
   else
	  IRCOM[6]=IRCOM[6]+0x30;

   if(IRCOM[7]>9)
    { IRCOM[7]=IRCOM[7]+0x37;}
   else
	  IRCOM[7]=IRCOM[7]+0x30;
     
	   lcd_pos(0x0b);             
     lcd_wdat(IRCOM[5]);        //��һλ����ʾ 
     lcd_pos(0x0c);             
     lcd_wdat(IRCOM[4]);        //�ڶ�λ����ʾ

     lcd_pos(0x4b);             
     lcd_wdat(IRCOM[7]);        //��һλ����ʾ 
     lcd_pos(0x4c);             
     lcd_wdat(IRCOM[6]);        //�ڶ�λ����ʾ
     
     beep();
	   IRAddr[0]=IRCOM[4];
	   IRAddr[1]=IRCOM[5];
	 
  
     EX0 = 1; 
} 

/**********************************************************/
void beep()
{
  unsigned char i;
  for (i=0;i<100;i++)
   {
   delay(4);
   BEEP=!BEEP;                 
   } 
  BEEP=1;                     
}
/**********************************************************/
void delay(unsigned char x)    //x*0.14MS
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<13; i++) {}
 }
}

/**********************************************************/

void keyscan()							 //������⺯��
{
	if(P2== 0xfe)
		key= 1;
	if(P2== 0xfd)
		key= 2;
	if(P2== 0xfb)
		key= 3;
	if(P2== 0xf7)
		key= 4;
	if(P2== 0xef)
		key= 5;
	if(P2== 0xdf)
		key= 6;
	if(P2== 0xbf)
		key= 7;
	if(P2== 0x7f)
		key= 8;

}