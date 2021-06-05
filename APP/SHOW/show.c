#include "show.h"

extern const unsigned char F16X16[];
extern const unsigned char F6X8[];
extern const unsigned char F8X16[];
extern const unsigned char BMP1[];
void Show_OLED(void)
{
	int i;

//	OLED_Fill(0xff);
	//DelayS(2);
	//OLED_CLS();
	//for(i=0; i<8; i++)//通过点阵显示汉字 -- i表示子表中汉字的位置
		//{
//			uOLED_16x16CN(i*16,0,2);
//		 	uOLED_16x16CN(i*16,2,i+8);
//		 	uOLED_16x16CN(i*16,4,i+16);
//		 	uOLED_16x16CN(i*16,6,i+24);
		//}
	 uOLED_6x8Str(0,0,"Helloe World*3.0");
	 uOLED_6x8Str(0,2,"Helloe World*3.0");
	 uOLED_8x16Str(0,4,"Helloe World*3.0");
		DelayS(2);
		//OLED_CLS();
	//uOLED_BMP(0,0,128,8,(unsigned char *)BMP1);

}