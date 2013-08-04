/******************** (C) DoWell������ ********************
 * �ļ���  ��main.c
 * ����    ��LCD FSMC Ӧ�ú����⡣
 *           ʵ�ֵĹ��ܣ����������㡢���ߡ���ʾ���֡��ַ�����ͼƬ������         
 * ʵ��ƽ̨������FSMC�ӿڵ�ַ��ΪA23�ߵ����п�����
 * ��汾  ��ST3.5.0
**********************************************************************************/
#include "drv_lcd.h"

//�����������궨�� , Һ���ӿڶ����Լ����ŷ�����
/*********************************************************************************
* ����д˺궨����Ϊ����ģʽ������Ϊ����ģʽ(��ֱɨ�� ����Ϊ����)
* 1������   0���ر�
*********************************************************************************/
#define Vertical 1   
/* ѡ��BANK1-BORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
 * FSMC_A23 ��LCD��DC(�Ĵ���/����ѡ��)��
 * 16 bit => FSMC[24:0]��ӦHADDR[25:1]
 * �Ĵ�������ַ = 0X60000000
 * RAM����ַ = 0x60000000+2^23*2
 * ��ѡ��ͬ�ĵ�ַ��ʱ����ַҪ���¼��㡣
 */
#define Bank1_LCD_D    ((u32)0x61000000)    //Disp Data ADDR
#define Bank1_LCD_C    ((u32)0x60000000)    //Disp Reg ADDR

#define BLACK 0X0000

u16 POINT_COLOR = BLACK;

static void LCD_SSD1289_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable the FSMC Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* config lcd gpio clock base on FSMC */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE);
     
    
    /* config tft data lines base on FSMC
	   * data lines,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	   */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* config tft control lines base on FSMC
     * PD4-FSMC_NOE  :LCD-RD
     * PD5-FSMC_NWE  :LCD-WR
     * PD7-FSMC_NE1  :LCD-CS
     * PE2-FSMC_A23  :LCD-DC
	   */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; 
    GPIO_Init(GPIOE, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* config tft rst gpio PE1*/
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ; 	 
//    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* tft control gpio init */	 
//    GPIO_ResetBits(GPIOE, GPIO_Pin_1);	          // RST = 0   
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		  // RD = 1  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		  // WR = 1 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		  // CS = 1 
}

static void LCD_SSD1289_FSMC_Config(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p; 
  
  p.FSMC_AddressSetupTime = 0x01;	 //��ַ����ʱ��
  p.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ��
  p.FSMC_DataSetupTime = 0x02;		 //���ݽ���ʱ��
  p.FSMC_BusTurnAroundDuration = 0x00;
  p.FSMC_CLKDivision = 0x00;
  p.FSMC_DataLatency = 0x00;
  p.FSMC_AccessMode = FSMC_AccessMode_B;	 // һ��ʹ��ģʽB������LCD
  
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p; 
  
  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
  
  /* Enable FSMC Bank1_SRAM Bank */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

static void Delay(__IO u32 nCount)
{
    for(; nCount != 0; nCount--);
}

static void LCD_SSD1289_Rst(void)
{			
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    Delay(0xFFF);					   
    GPIO_SetBits(GPIOE, GPIO_Pin_1 );		 	 
    Delay(0xFFF);	
}

/********************************************************************************
 * ��������LCD_WR_REG
 * ����  ��SSD1289 д�Ĵ�������
 * ����  ��-index �Ĵ���
 * ����  ���ڲ�����
********************************************************************************/
static  void LCD_SSD1289_WR_REG(u16 index)
{
    *(__IO u16 *)(Bank1_LCD_C) = index;
}

/********************************************************************************
 * ��������LCD_WR_REG
 * ����  ����SSD1289�Ĵ���д����
 * ����  ��-index �Ĵ���
 *         -val   д�������
********************************************************************************/
static  void LCD_SSD1289_WR_CMD(u16 index, u16 val)
{	
    *(__IO u16 *)(Bank1_LCD_C) = index;	
    *(__IO u16 *)(Bank1_LCD_D) = val;
}

/********************************************************************************
 * ��������LCD_WR_Data
 * ����  ����SSD1289 GRAM д������
 * ����  ��-val д�������,16bit        
********************************************************************************/
/*
static  void LCD_WR_Data(unsigned int val)
{   
    *(__IO u16 *) (Bank1_LCD_D) = val; 	
}
 */

/********************************************************************************
 * ��������LCD_SSD1289_RD_data
 * ����  ���� SSD1289 RAM ����
 * ���  ����ȡ������,16bit *         
********************************************************************************/
static  u16 LCD_SSD1289_RD_data(void)
{
    u16 a = 0;
	
    a = (*(__IO u16 *)(Bank1_LCD_D)); 	//Dummy	
    a = *(__IO u16 *)(Bank1_LCD_D);     //L
    
    return(a);	
}

/*******************************************************************************

 * ��������LCD_SSD1289_ReadRegister
 * ����  ����SSD1289 GRAM ��������
 * ����  ��-val д�������,16bit        
*******************************************************************************/
unsigned short LCD_SSD1289_ReadRegister(void)
{  
  //д�����Ĵ���
  LCD_SSD1289_WR_REG(0x22);
  
  return(LCD_SSD1289_RD_data());
}
/*******************************************************************************

 *  ��������LCD_SSD1289_BGR2RGB
 *  ���ܣ�BGR��RGB��ת��
 *  ���룺BGR_Code��������BGR����
 *  �����ת�����RGB��ɫֵ
 
*******************************************************************************/
unsigned short LCD_SSD1289_BGR2RGB(unsigned short BGR_Code)
{
  unsigned short  r, g, b;

  r = BGR_Code & 0x1f;
  g = (BGR_Code>>5)  & 0x3f;
  b = (BGR_Code>>11) & 0x1f;
  
  return( (r<<11) + (g<<5) + (b<<0) );
}
/*******************************************************************************
 *  ��������LCD_SSD1289_GetPointRGB
 *  ���ܣ���ȡһ�����ص����ɫֵ
 *  ���룺x,y�������
 *  �����16bit��565��ɫֵ
*******************************************************************************/
unsigned short LCD_SSD1289_GetPointRGB(unsigned short Xpos,unsigned short Ypos)
{
  LCD_SSD1289_SetCursor(Xpos,Ypos);
  return(LCD_SSD1289_BGR2RGB(LCD_SSD1289_ReadRegister()));
}

/*******************************************************************************
 *  ��������RGB
 *  ���ܣ�RGB��ɫ��Ϻ���
 *  ���룺R 0-31,G 0-63,B 0-31
 *  �������Ϻ����ɫֵ
*******************************************************************************/
/*
static u16 RGB(u8 R,u8 G,u8 B)
{	
    return ( ( (u8)R<<11 ) | ( (u8)G<<5 & 0X07E0 ) | ( (u8)(B&0X1F) ) );
}

*/
/*******************************************************************************

 *  ��������LCD_SSD1289_SetCursor
 *  ���ܣ�����LCD������
 *  ���룺x��LCDˮƽ����
          y��LCD��ֱ����
 *  �������

*******************************************************************************/
void LCD_SSD1289_SetCursor(u16 Xpos, u16 Ypos)
{
#if Vertical             //��Ϊ������ʱ�����꣨0,0x013f���������Ͻ�
  u16 V_X,V_Y;   //��Ϊ����ģʽʱΪ��д�����ϰ�ߣ���Ҫת��һ��ʵ�ʵ�x��ˮƽ���꣩��  
  V_X = Ypos;               //y����ֱ���꣩��ϰ��ʹ�õ�x��y��Ϊ������ʱ��������y��ʱ��Ϊ������x
  V_Y = 0x013f - Xpos;      //������x��Ϊ������y����������ߵ�ת����ϵ��
  
  LCD_SSD1289_WR_CMD(0x4e, V_X);
  LCD_SSD1289_WR_CMD(0x4f, V_Y);  
#else             
  LCD_SSD1289_WR_CMD(0x4e, Xpos);
  LCD_SSD1289_WR_CMD(0x4f, Ypos);
#endif
  LCD_SSD1289_WR_REG(0x22);
}
/*******************************************************************************

 *  ��������LCD_SSD1289_WriteGRAM
 *  ���ܣ�д��һ�����ص�����
 *  ���룺RGB_Code�����ص����ɫֵ
         
 *  �������

*******************************************************************************/
void LCD_SSD1289_WriteGRAM(u16 RGB_Code)
{
  LCD_SSD1289_WR_CMD(0x22,RGB_Code);
}
/********************************************************************************
 *  ��������LCD_SSD1289_XorPixel
 *  ���ܣ���ָ����λ�û�һ���㣬����ɫΪColor��ԭ��������
 *  ���룺x,y������
          Color�����ص����ɫֵ
 *  �������
********************************************************************************/
void LCD_SSD1289_XorPixel(int x, int y,u16 Color)
{
  u16 Index = LCD_SSD1289_GetPointRGB(x,y);
  LCD_SSD1289_DrawPoint(x,y,Color-1-Index);
}
/********************************************************************************
 *  ��������LCD_SSD1289_OrPixel
 *  ���ܣ���ָ����λ�û�һ���㣬����ɫΪColor��ԭ����Ļ�
 *  ���룺x,y������
          Color�����ص����ɫֵ
 *  �������
********************************************************************************/
void LCD_SSD1289_OrPixel(int x, int y,u16 Color)
{
  u16 Index = LCD_SSD1289_GetPointRGB(x,y);
  LCD_SSD1289_DrawPoint(x,y,Color | Index);
}
/********************************************************************************
 *  ��������LCD_SSD1289_DrawPoint
 *  ���ܣ���ָ����λ�û�һ���㣬����ɫΪColor
 *  ���룺x,y������
          Color�����ص����ɫֵ
 *  �������
********************************************************************************/
void LCD_SSD1289_DrawPoint(u16 x, u16 y, u16 Color)
{
  LCD_SSD1289_SetCursor(x, y);
  LCD_SSD1289_WriteGRAM(Color);  
}

/********************************************************************************
 *  ��������LCD_SSD1289_DrawHLine
 *  ���ܣ�  ��һ��ˮƽֱ��
 *  ���룺x,y����ʼ����
          x1:  ˮƽ�յ�����
          Color�����ص����ɫֵ
 *  �������
********************************************************************************/
void LCD_SSD1289_DrawHLine(u16 x,u16 y,u16 x1,u16 Color)
{
  u16 i;
  for(i = x;i <= x1;i++)
  {
    LCD_SSD1289_SetCursor(i,y);
    LCD_SSD1289_WriteGRAM(Color);  
  }
}
/********************************************************************************
 *  ��������LCD_SSD1289_DrawVLine
 *  ���ܣ�  ��һ����ֱֱ��
 *  ���룺 x,y����ʼ����
           x1:  ��ֱ�յ�����
           Color�����ص����ɫֵ
 *  ����� ��
********************************************************************************/
void LCD_SSD1289_DrawVLine(u16 x,u16 y,u16 y1,u16 Color)
{
  u16 i;            
  for(i = y;i <= y1;i++)
  {
    LCD_SSD1289_SetCursor(x,i);
    LCD_SSD1289_WriteGRAM(Color);
  }
}
/*----------------------------------------����ΪLCDӦ�ú���----------------------------------------------*/
/********************************************************************************
 * ��������LCD_CLEAR
 * ����  ����x,yΪ������㣬�ڳ�len,��wid�ķ�Χ������
 * ����  ��-x -y -len -wid
 * ���  ���� 
********************************************************************************/
void LCD_SSD1289_CLEAR(u16 x,u16 y,u16 Wid,u16 High,u16 Color)
{                    
    u16 i,j;
    
    LCD_SSD1289_SetCursor(x,y);
    
    for(i = y;i < High;i++) //������ĸ߶� 
    {  
      LCD_SSD1289_SetCursor(x,y + i);
      for(j = x;j < Wid;j++)//������Ŀ��
        LCD_SSD1289_WriteGRAM(Color); 
    }
}
/********************************************************************************
 *  ��������LCD_SSD1289_FullScreen
 *  ���ܣ�  ��LCD��Ϊָ������ɫ��ȫ����
 *  ���룺  Color��LCD��Ļ����ɫ
 *  ����� ��
********************************************************************************/
void LCD_SSD1289_FullScreen(u16 Color)
{
#if Vertical 
  LCD_SSD1289_CLEAR(0,0,320,240,Color);
#else
  LCD_SSD1289_CLEAR(0,0,240,320,Color);
#endif
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayChar
 *  ���ܣ�  ��ʾһ��ASCII�ַ�
 *  ���룺 x,y��      ��ʼ����
           pAscii:    �������������
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺��������Ҫȡģ������֧��2MFlash�洢��ASCII��������
********************************************************************************/
void LCD_SSD1289_DisplayChar(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor)
{
  u8 i,j;
  u8 str;  
  for (i=0;i<16;i++)//��Ϊһ���ַ��ĸ߶�Ϊ16
  {
    LCD_SSD1289_SetCursor(x,y + i); 
    
    str = pAscii[i];//*(ASCII_Table + OffSet + i);
    for (j=0;j<8;j++)//һ���ַ����Ϊ8��һ�����ص�Ϊ16λ
    {
       if ( str & (0x80>>j) )//0x80>>j��ѡ���ַ���ɫ�����ݵ�ɫ�壩
       {
          LCD_SSD1289_WriteGRAM(WordColor);
       }
       else
       {
          LCD_SSD1289_WriteGRAM(BackColor);
       }               				
     }
  } 
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayChar_XY
 *  ���ܣ�  ��ʾһ��ASCII�ַ�
 *  ���룺 x,y��      ��ʼ����
           pAscii:    ASCII������
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺����������Ҫȡģ��ʹ�ñ������Դ���ASCII�������� ASCII_Table[]
********************************************************************************/
void LCD_SSD1289_DisplayChar_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor)
{
  u8 i,j;
  u8 str;
  u16 OffSet;  
  OffSet = (*pAscii - 32)*16;//Ѱ��AsciiLib[]����Ӧ�ַ��ĵ�������
        /*pAscii���������Ǹ��ַ���ASCII�����ֱ�ʾ���ο�ASCII�����ϸ�
        ˳���ʾ������32����Ϊ��0~31���ǿ����ַ�����32��Ϊ���ո��ַ�
        ����16����Ϊ��AsciiLib[]�е�ÿ���ַ�����Ϊ16���ֽڱ�ʾ*/
  for (i=0;i<16;i++)//��Ϊһ���ַ��ĸ߶�Ϊ16
  {
    LCD_SSD1289_SetCursor(x,y + i); 
    
//    str = ASCII_Table[OffSet + i];//*(ASCII_Table + OffSet + i);
    for (j=0;j<8;j++)//һ���ַ����Ϊ8��һ�����ص�Ϊ16λ
    {
       if ( str & (0x80>>j) )//0x80>>j��ѡ���ַ���ɫ�����ݵ�ɫ�壩
       {
          LCD_SSD1289_WriteGRAM(WordColor);
       }
       else
       {
          LCD_SSD1289_WriteGRAM(BackColor);
       }               				
     }
  } 
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayChar_NoXY
 *  ���ܣ�  ��ʾһ��ASCII�ַ���������ָ��
 *  ���룺 pAscii:    ASCII�����ݵ�ַ
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺����������Ҫȡģ��ʹ�ñ������Դ���ASCII�������� ASCII_Table[]
********************************************************************************/
void LCD_SSD1289_DisplayChar_NoXY(const u8 *pAscii,u16 WordColor,u16 BackColor)
{
  u8 i,j;
  u8 str;
  u16 OffSet;  
  OffSet = (*pAscii - 32)*16;//Ѱ��AsciiLib[]����Ӧ�ַ��ĵ�������
        /*pAscii���������Ǹ��ַ���ASCII�����ֱ�ʾ���ο�ASCII�����ϸ�
        ˳���ʾ������32����Ϊ��0~31���ǿ����ַ�����32��Ϊ���ո��ַ�
        ����16����Ϊ��AsciiLib[]�е�ÿ���ַ�����Ϊ16���ֽڱ�ʾ*/
  for (i=0;i<16;i++)//��Ϊһ���ַ��ĸ߶�Ϊ16
  {
//    str = ASCII_Table[OffSet + i];//*(ASCII_Table + OffSet + i);
    for (j=0;j<8;j++)//һ���ַ����Ϊ8��һ�����ص�Ϊ16λ
    {
       if ( str & (0x80>>j) )//0x80>>j��ѡ���ַ���ɫ�����ݵ�ɫ�壩
       {
          LCD_SSD1289_WriteGRAM(WordColor);
       }
       else
       {
          LCD_SSD1289_WriteGRAM(BackColor);
       }               				
     }
  } 
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayString_XY
 *  ���ܣ�  ��ָ����괦��ʾ�ַ��������Զ�����
 *  ���룺 x,y��      ��ʼ����
           pAscii:    ASCII�����ݵ�ַ
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺����������Ҫȡģ��ʹ�ñ������Դ���ASCII�������� ASCII_Table[]
********************************************************************************/
void LCD_SSD1289_DisplayString_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor)
{
#if Vertical 
  while(*pAscii != '\0')
  {
    if(x == 320)//��һ����ʾ�ַ�����40���ַ�ʱ����һ�н�����ʾ
    {
      x = 0;
      y = y+16;
    }
    LCD_SSD1289_DisplayChar_XY(x,y,pAscii,WordColor,BackColor);
    x+=8;//������һ���ַ�
    pAscii+=1;
  }
#else
  while(*pAscii != '\0')
  {
    if(x==240)//��һ����ʾ�ַ�����30���ַ�ʱ����һ�н�����ʾ
    {
      x=0;
      y=y+16;
    }
    LCD_SSD1289_DisplayChar_XY(x,y,pAscii,WordColor,BackColor);
    
    x+=8;//������һ���ַ�
    pAscii+=1;
  }  
#endif
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayString_NoXY
 *  ���ܣ�  ��ʾ�ַ�������ָ��λ�ã����Զ�����
 *  ���룺 pAscii:    ASCII������
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺����������Ҫȡģ��ʹ�ñ������Դ���ASCII�������� ASCII_Table[]
********************************************************************************/
void LCD_SSD1289_DisplayString_NoXY(const u8 *pAscii,u16 WordColor,u16 BackColor)
{
#if Vertical 
  while(*pAscii != '\0')
  {
    LCD_SSD1289_DisplayChar_NoXY(pAscii,WordColor,BackColor);
    pAscii+=1;
  }
#else
  while(*pAscii != '\0')
  {
    LCD_SSD1289_DisplayChar_NoXY(pAscii,WordColor,BackColor);

    pAscii+=1;
  }  
#endif
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayHanZi_XY
 *  ���ܣ�  ��ʾ����
 *  ���룺 x,y��      ��ʼ���� 
           pHz:       ���������������ַ��Ӧ��Ϊ32�ֽڣ�
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺��������Ҫ����ȡģ
********************************************************************************/
void LCD_SSD1289_DisplayHanZi_XY(u16 x,u16 y,const u8 *pHz,u16 WordColor,u16 BackColor)
{
  u8 i,j;
  u16 str;
  for (i=0;i<16;i++)//��Ϊһ���ַ��ĸ߶�Ϊ16
  {
    LCD_SSD1289_SetCursor(x,y+i);
    str = ((u16)(*(pHz + i*2)<<8))|(*(pHz+i*2+1));
    for (j=0;j<16;j++)//һ�����ֿ��Ϊ16��һ�����ص�Ϊ16λ
    {
      if ( str & (0x8000>>j) )//0x80>>j��ѡ���ַ���ɫ�����ݵ�ɫ�壩
      {
        LCD_SSD1289_WriteGRAM(WordColor);
      }
      else
      {
        LCD_SSD1289_WriteGRAM(BackColor);
      }               				
    }
  }
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayHanZiString_XY
 *  ���ܣ�  ��ʾһ������
 *  ���룺 x,y��      ��ʼ���� 
           pHz:       ���������������ַ��Ӧ��Ϊ32 * N�ֽڣ�NΪ���ֵĸ���
           WordColor��������ɫ
           BackColor��������ɫ
 *  ����� ��
 *  ע�⣺��������Ҫ����ȡģ
********************************************************************************/
void LCD_SSD1289_DisplayHanZiString_XY(u16 x,u16 y,const u8 *pHz,u16 WordColor,u16 BackColor)
{

#if Vertical 
  u8 i = 0;
  while(i < 224)
  {
    if(x == 320)//��һ����ʾ�ַ�����40���ַ�ʱ����һ�н�����ʾ
    {
      x = 0;
      y = y+16;
    }
    LCD_SSD1289_DisplayHanZi_XY(x,y,&pHz[i],WordColor,BackColor);
    x+=16;//������һ���ַ�
    i+=32;
  }
#else
  u8 i = 0;
  while(i < 224)
  {
    if(x==240)//��һ����ʾ�ַ�����30���ַ�ʱ����һ�н�����ʾ
    {
      x=0;
      y=y+16;
    }
    LCD_SSD1289_DisplayHanZi_XY(x,y,&pHz[i],WordColor,BackColor);
    
    x+=16;//������һ���ַ�
    i+=32;
  }  
#endif
}
/********************************************************************************
 *  ��������LCD_SSD1289_Col_ColorBar
 *  ���ܣ�  ��ɫ��������ʾ����(6�������ƣ� Vertical  1 ����  0 ����
 *  ���룺 ��
 *  ����� ��
********************************************************************************/
void LCD_SSD1289_Col_ColorBar(void)
{
  u16 j,k;
#if Vertical  
  LCD_SSD1289_SetCursor(0,0x013f);
  for(k=0;k<240;k++)
  {
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Blue);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Red);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Magenta);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Green);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Cyan);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Yellow);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Blue);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Red);
    }    
  }
#else
  LCD_SSD1289_SetCursor(0,0);
  for(k=0;k<320;k++)
  {
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Blue);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Red);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Magenta);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Green);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Cyan);
    }
    for(j=0;j<40;j++)
    {
      LCD_SSD1289_WriteGRAM(Yellow);
    }
  }
#endif  
}
/********************************************************************************
 *  ��������LCD_SSD1289_DisplayHanZiString_XY
 *  ���ܣ�  ��ʾָ����С��ͼƬ��ͼƬ��͸����Ϊż����
 *  ���룺 x,y��        ��ʼ���� 
           Width,Height: ͼƬ�Ŀ�Ⱥ͸߶�
           pBMP��       ͼƬ��������ַ
 *  ����� ��
 *  ע�⣺��������Ҫ����ȡģ
********************************************************************************/
void LCD_SSD1289_DrawBMP(u16 x, u16 y, u16 Width, u16 Height, const u8 *pBMP)
{
  u32 m = 0;
  u16 i, j;

  LCD_SSD1289_SetCursor(x, y);

  for (i=0; i<Width; i++)  //������
  {
     for (j=0; j<Height; j++) //������
     {
       LCD_SSD1289_WriteGRAM((((uint16_t)(pBMP[m]))<<8) | pBMP[m + 1]);
       m = m + 2;
     }
  }
}

/********************************************************************************
 *  ��������H_V_Type
 *  ���ܣ�  �趨������������
 *  ���룺  Type��  ��ʾģʽ  0 ����  1 ���� 
 *  �����  ��
********************************************************************************/
void H_V_Type(u8 Type)
{
  if(!Type)
    LCD_SSD1289_WR_CMD(0x0011,0x6070); //�趨Ϊ����ģʽ
  else
    LCD_SSD1289_WR_CMD(0x0011,0x6058); //�趨Ϊ����ģʽ
}

void rt_hw_lcd_init(void)
{
  unsigned long i;
	
  LCD_SSD1289_GPIO_Config();
  LCD_SSD1289_FSMC_Config();		
//  LCD_SSD1289_Rst();
  
  LCD_SSD1289_WR_CMD(0x0000,0x0001);    Delay(50);  //�򿪾���
  LCD_SSD1289_WR_CMD(0x0003,0xA8A4);    Delay(50);   //0xA8A4
  LCD_SSD1289_WR_CMD(0x000C,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x000D,0x080C);    Delay(50);
  LCD_SSD1289_WR_CMD(0x000E,0x2B00);    Delay(50);
  LCD_SSD1289_WR_CMD(0x001E,0x00B0);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0001,0x2B3F);    Delay(50);   //�����������320*240  0x6B3F  293f  2b3f 6b3f
  LCD_SSD1289_WR_CMD(0x0002,0x0600);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0010,0x0000);    Delay(50);
#if Vertical  
  H_V_Type(1);   //  LCD_WR_CMD(0x0011,0x6070);//0x4030 //�������ݸ�ʽ  16λɫ  ���� 0x6058	 6078
#else
  H_V_Type(0);  
#endif
	Delay(50);
  LCD_SSD1289_WR_CMD(0x0005,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0006,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0016,0xEF1C);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0017,0x0003);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0007,0x0233);    Delay(50);        //0x0233
  LCD_SSD1289_WR_CMD(0x000B,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x000F,0x0000);    Delay(50);        //ɨ�迪ʼ��ַ
  LCD_SSD1289_WR_CMD(0x0041,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0042,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0048,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0049,0x013F);    Delay(50);
  LCD_SSD1289_WR_CMD(0x004A,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x004B,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0044,0xEF00);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0045,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0046,0x013F);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0030,0x0707);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0031,0x0204);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0032,0x0204);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0033,0x0502);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0034,0x0507);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0035,0x0204);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0036,0x0204);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0037,0x0502);    Delay(50);
  LCD_SSD1289_WR_CMD(0x003A,0x0302);    Delay(50);
  LCD_SSD1289_WR_CMD(0x003B,0x0302);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0023,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0024,0x0000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x0025,0x8000);    Delay(50);
  LCD_SSD1289_WR_CMD(0x004f,0);         Delay(30);//����ַ0
  LCD_SSD1289_WR_CMD(0x004e,0);         Delay(30);//����ַ0

  for (i=0; i<76800; i++)		   //320*240=76800		
  {
      LCD_SSD1289_WriteGRAM(Blue);	
  }    
}


