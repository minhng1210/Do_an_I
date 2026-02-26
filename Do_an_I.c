#include <REGX51.H>
#include <Do_an_I_LCD.H>
#include <Do_an_I_DS1307.H>

sbit PIR1 = P1^0;
sbit PIR2 = P1^1;

sbit ON = P1^4;
sbit OFF = P1^5;
sbit SET = P1^6;
sbit ADJUST = P1^7;

sbit LAMP1 = P2^0;
sbit LAMP2 = P2^1;
sbit LAMP3 = P2^2;

unsigned int hour, minute, second, day, date, month, year;

void Delay(unsigned int t){
    unsigned int i, j;
    for (i = 0; i < t; i++)
        for (j = 0; j < 123; j++);
}

void Display_Time(){
	LCD_Gotoxy(4,0);
	LCD_PutNumber(hour/10);
	LCD_PutNumber(hour%10);
	LCD_Puts(":");
	LCD_PutNumber(minute/10);
	LCD_PutNumber(minute%10);
	LCD_Puts(":");
	LCD_PutNumber(second/10);
	LCD_PutNumber(second%10);
	LCD_Gotoxy(1,1);
	if(day <= 1){
		LCD_Puts("CN ");
	}else{
	LCD_Puts("T");
	LCD_PutNumber(day);
	LCD_Puts(" ");
	}
	LCD_PutNumber(date/10);
	LCD_PutNumber(date%10);
	LCD_Puts("/");
	LCD_PutNumber(month/10);
	LCD_PutNumber(month%10);
	LCD_Puts("/");
	LCD_PutNumber(2);
	LCD_PutNumber(0);
	LCD_PutNumber(year/10);
	LCD_PutNumber(year%10);
}
void Flicker(unsigned int num, unsigned char x, unsigned char y){
	LCD_Gotoxy(x, y);
	LCD_Puts(" ");
	Delay(100);
	LCD_Gotoxy(x, y);
	LCD_PutNumber(num);
	Delay(100);
}
void Adjust_Time(unsigned int *time, unsigned char x, unsigned char y,  unsigned int max){
	unsigned int dozens = *time/10;
	unsigned int unit = *time%10;
	while (ADJUST){
		Flicker(dozens, x, y);
		if (!SET) break;
	}
	while(SET){
		if (ADJUST == 0){
		dozens += 1;
		}
		if (dozens > max/10){
			dozens = 0;
		}
		Flicker(dozens, x, y);
	}
	Delay(200);
	while(SET){
		if (dozens*10 + unit > max || unit > 9){
			unit = 0;
		}
		if (ADJUST == 0){
		unit += 1;
		}
		Flicker(unit, x+1, y);	
	}
	*time = dozens*10 + unit;
}
void Adjust_Time_Day(){
	while (ADJUST){
		LCD_Gotoxy(1, 1);
		LCD_Puts("  ");
		Delay(200);
		LCD_Gotoxy(1, 1);
		if(day <= 1){
			LCD_Puts("CN ");
		}else{
			LCD_Puts("T");
			LCD_PutNumber(day);
		}
		Delay(200);
		if (!SET) break;
	}
	while(SET){
		if (ADJUST == 0){
		day += 1;
		}
		if (day > 7){
			day = 1;
		}
		LCD_Gotoxy(1, 1);
		LCD_Puts("  ");
		Delay(200);
		LCD_Gotoxy(1, 1);
		if(day <= 1){
			LCD_Puts("CN ");
		}else{
			LCD_Puts("T");
			LCD_PutNumber(day);
		}
		Delay(200);
	}
}
void Set_Time(){
	int max;
	Adjust_Time(&hour, 4, 0, 23);
	Adjust_Time(&minute, 7, 0, 60);
	Adjust_Time(&second, 10, 0, 60);
	Adjust_Time_Day();
	Adjust_Time(&date, 4, 1, 31);
	Adjust_Time(&month, 7, 1, 12);
	Adjust_Time(&year, 12, 1, 99);
	switch (month) {
		case 4: case 6: case 9: case 11:
			max = 30;
			break;
		case 2:
			if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
				max = 29;
			}else{
				max = 28;
			}
			break;
		default:
			max = 31;
	}
	if (date > max){
		date = max;
	}
	DS1307_Write_Time(hour, minute, second, day, date, month, year);
}

void Delay_Lamp(){
	int m, n;
	for (m = 0; m < 40; m++){
		for (n = 0; n < 12500; n++);
		DS1307_Read_Time(&hour, &minute, &second, &day, &date, &month, &year);
		Display_Time();
		if (!ON || !OFF){
			break;
		}
	}
}

void main() {
	int m, n;
	int l2 = 0;
	DS1307_Init();
	LCD_Init();
	LAMP1 = LAMP2 = LAMP3 = 0;
	while(1){
		DS1307_Read_Time(&hour, &minute, &second, &day, &date, &month, &year);
		Display_Time();
		if (!SET){
			Set_Time();
			Delay(200);
		}
		if (!ON){
			LAMP1 = LAMP2 = LAMP3 = 1;
		}else if (!OFF){
			LAMP1 = LAMP2 = LAMP3 = 0;
		}else{
			if ((7 <= hour)&&(hour < 17)){
				LAMP1 = LAMP2 = LAMP3 = 0;
			}else{
				if (LAMP1 && LAMP2 && LAMP3){
					LAMP1 = LAMP2 = LAMP3 = 0;
				}
				if (!PIR1){
					LAMP1 = 1;
					l2 = 1;
					LAMP3 = 0;
				}
				if (!PIR2){
					LAMP1 = 0;
					l2 = 1;
					LAMP3 = 1;
				}
				if (l2){
					if (!LAMP2){
						Delay_Lamp();
						LAMP2 = 1;
					}
				}
				m = 20;
				while (PIR1 & PIR2 && m){
					for (n = 0; n < 12500; n++);
					DS1307_Read_Time(&hour, &minute, &second, &day, &date, &month, &year);
					Display_Time();
					if (!ON || !OFF){
						break;
					}
					m--;
				}
				if (!m){
					l2 = LAMP2 = 0;
					if (LAMP1){
						Delay_Lamp();
						LAMP1 = 0;
					}
					if (LAMP3){
						Delay_Lamp();
						LAMP3 = 0;
					}
				}
			}
		}
	}
}