#include <REGX51.H>
#include <Do_an_I_LCD.h>

#define LCD_RS  P3_0
#define LCD_RW  P3_1
#define LCD_EN  P3_2
#define LCD_D4  P3_4
#define LCD_D5  P3_5
#define LCD_D6  P3_6
#define LCD_D7  P3_7

void delay_us(unsigned int t) {
    while (t--);
}

void delay_ms(unsigned int t) {
    while (t--)
        delay_us(125);
}
void LCD_Enable(void){
        LCD_EN =1;
        delay_us(3);
        LCD_EN = 0;
        delay_us(50); 
}
void LCD_Send4Bit(unsigned char Data){
        LCD_D4=Data & 0x01;
        LCD_D5=(Data>>1)&1;
        LCD_D6=(Data>>2)&1;
        LCD_D7=(Data>>3)&1;
}
void LCD_SendCommand(unsigned char command){
        LCD_Send4Bit(command >>4);/* Send 4 bit high */
        LCD_Enable();
        LCD_Send4Bit(command); /* Send 4 bit low*/
        LCD_Enable();
}
void LCD_Init(){
        LCD_Send4Bit(0x00);	  //turn on LCD
        delay_ms(20);
        LCD_RS=0;
        LCD_RW=0;
        LCD_Send4Bit(0x03);	  //function setting
        LCD_Enable();
        delay_ms(5);
        LCD_Enable();
        delay_us(100);
        LCD_Enable();
        LCD_Send4Bit(0x02);	  //di chuyen con tro ve dau man hnh
        LCD_Enable();
        LCD_SendCommand( 0x28 ); //lua chon che do 4 bit
        LCD_SendCommand( 0x0c);  // bat hien thi va tat con tro di
        LCD_SendCommand( 0x06 );
        LCD_SendCommand(0x01);
}
void LCD_Gotoxy(unsigned char x, unsigned char y) {
	LCD_SendCommand((y ? 0xC0 : 0x80) + x);
	delay_us(50);
}
void LCD_PutChar(unsigned char Data){
	LCD_RS=1;
	LCD_SendCommand(Data);
	LCD_RS=0;
}
void LCD_Puts(char *s){
	while (*s){
		LCD_PutChar(*s);
		s++;
	}
}
void LCD_PutNumber(int number) {
  char buffer[6];
	char *ptr = buffer + sizeof(buffer) - 1;
	*ptr = '\0';
	if (number == 0) {
		LCD_PutChar('0');
		return;
	}
	if (number < 0) {  
		LCD_PutChar('-');  
		number = -number;  
	}
	while (number > 0) {
		*(--ptr) = (number % 10) + '0';
		number /= 10;
	}
	LCD_Puts(ptr);
}
