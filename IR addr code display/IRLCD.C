#include <reg51.h>
#include <intrins.h>
#include "1602.h"

#define uchar unsigned char
#define uint  unsigned int
#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};

void delay(uchar x);  //x*0.14MS
void delay1(int ms);
void beep();

sbit IRIN = P3^2;         //���������������
sbit BEEP = P1^5;         //������������
sbit RELAY= P1^4;         //�̵���������

uchar IRCOM[7];


uchar code  cdis1[ ] = {" Red Control "};
uchar code  cdis2[ ] = {"  IR-CODE: --H  "};


/*******************************************************************/
main()
{
   uchar m;
    
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

   while(1)	;

} //end main
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

   IRCOM[5]=IRCOM[0] & 0x0F;     //ȡ����ĵ���λ
   IRCOM[6]=IRCOM[0] >> 4;       //����4�Σ�����λ��Ϊ����λ

   if(IRCOM[5]>9)
    { IRCOM[5]=IRCOM[5]+0x37;}
   else
	  IRCOM[5]=IRCOM[5]+0x30;

   if(IRCOM[6]>9)
    { IRCOM[6]=IRCOM[6]+0x37;}
   else
	  IRCOM[6]=IRCOM[6]+0x30;

     lcd_pos(0x4b);             
     lcd_wdat(IRCOM[6]);        //��һλ����ʾ 
     lcd_pos(0x4c);             
     lcd_wdat(IRCOM[5]);        //�ڶ�λ����ʾ

     beep();
     EX0 = 1; 
} 

/**********************************************************/
void beep()
{
  unsigned char i;
  for (i=0;i<100;i++)
   {
   delay(4);
   BEEP=!BEEP;                 //BEEPȡ��
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
