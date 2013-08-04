#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__

#include "stm32f10x.h"

//��ɫ�ĺ궨��
#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

/*
 * ���� Put_ascii_chinese_string ��������Ӣ�����ʾ�����������Ƚ����ã����õĻ�����
 * ���ùܣ�����Ȥ�Ļ�Ҳ���Կ��£�д�ñȽ���^_^������
 * ���þ��� Put_ascii_chinese_string(0, 0,����ӭʹ��Ұ��Stm32�����塱, 0, 1);
 * �ڵ��ú���Put_ascii_chinese_string();֮ǰһ��Ҫ���ú���void LCD_Init();
 * ����ʼ��Һ����  
 */
void Put_ascii_chinese_string(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint16_t Color,u8 mode);

void LCD_SSD1289_SetCursor(u16 Xpos, u16 Ypos);
void LCD_SSD1289_WriteGRAM(u16 RGB_Code);
void LCD_SSD1289_DisplayCharXor(u16 x,u16 y,const u8 *pAscii,u16 WordColor);
void LCD_SSD1289_DisplayChar(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor);
void LCD_SSD1289_DisplayChar_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor);
void LCD_SSD1289_DisplayChar_NoXY(const u8 *pAscii,u16 WordColor,u16 BackColor);

void LCD_SSD1289_DisplayStringOr_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor);
void LCD_SSD1289_DisplayStringXor_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor);
void LCD_SSD1289_DisplayString_XY(u16 x,u16 y,const u8 *pAscii,u16 WordColor,u16 BackColor);
void LCD_SSD1289_DisplayString_NoXY(const u8 *pAscii,u16 WordColor,u16 BackColor);

void LCD_SSD1289_DisplayHanZi_XY(u16 x,u16 y,const u8 *pHz,u16 WordColor,u16 BackColor);
void LCD_SSD1289_DisplayHanZiString_XY(u16 x,u16 y,const u8 *pHz,u16 WordColor,u16 BackColor);

void LCD_SSD1289_Col_ColorBar(void);
void H_V_Type(unsigned char Type);
void LCD_SSD1289_DrawBMP(u16 x,u16 y,u16 Width,u16 Height,const u8 *pBMP);
void LCD_SSD1289_DrawBMP_GUIBit(u16 x,u16 y,u16 Width,u16 Height,const u16 *pBMP);
void LCD_SSD1289_CLEAR(u16 x,u16 y,u16 len,u16 wid,u16 Color);
void LCD_SSD1289_FullScreen(u16 Color);
void rt_hw_lcd_init(void);

void LCD_SSD1289_XorPixel(int x, int y,u16 Color);
void LCD_SSD1289_DrawPoint(u16 x,u16 y,u16 Color);
void LCD_SSD1289_DrawHLine(u16 x,u16 y,u16 x1,u16 Color);
void LCD_SSD1289_DrawVLine(u16 x,u16 y,u16 y1,u16 Color);
unsigned short LCD_SSD1289_GetPointRGB(unsigned short Xpos,unsigned short Ypos);

#endif

