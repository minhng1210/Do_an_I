#include <REGX51.H>
#include <Do_an_I_DS1307.H>

#define SCL P0_0
#define SDA P0_1

void Delay(){
	unsigned int i, j;
	for (i = 0; i < 1; i++)
		for (j = 0; j < 12; j++);
}
void I2C_Init(){	
	SCL=1;
	SDA=1;
}
void I2C_Start(){
	SCL = 1;	Delay();
	SDA = 0;	Delay();
	SCL = 0;
}
bit I2C_Get_Ack(){
	bit result;
	SDA = 1;	Delay();
	SCL = 1;	Delay();
	result = SDA;
	SCL = 0;
	return result;
}
bit I2C_Write(unsigned char dat){
	unsigned char i;
	for(i=0; i < 8; i++)
	{
		SDA = (bit)(dat&0x80);	
		SCL = 1;	Delay();
		SCL = 0;
		dat<<=1;
	}
	return(I2C_Get_Ack());
}
void I2C_Ack(){
	SDA = 0;	Delay();
	SCL = 1;	Delay();
	SCL = 0;
}
void I2C_Nak(){
	SDA = 1;	Delay();
	SCL = 1;	Delay();
	SCL = 0;
}
unsigned char I2C_Read(bit ack){
	unsigned char i, dat=0;
	for(i = 0; i < 8; i++)
	{
		SDA = 1;	Delay();
		SCL = 1;	Delay();
		dat <<= 1;
		if(SDA)
		{
			dat |= 0x01;
		}
		SCL = 0;
	}
	if(ack)
	{
		I2C_Ack();
	}
	else
	{
		I2C_Nak();
	}
	return dat;
}
void I2C_Stop(){ 	
	SDA = 0;	Delay();
	SCL = 1;	Delay();
	SDA = 1;
}
void DS1307_Init(){
	unsigned char tmp;
	I2C_Init();
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x00);
	I2C_Start();
	I2C_Write(0xD1);
	tmp = I2C_Read(0);
	I2C_Stop();
	tmp &= 0x7F;
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x00);
	I2C_Write(tmp);
	I2C_Stop();
}

void DS1307_Read_Time(unsigned int *hour, unsigned int *minute, unsigned int *second, unsigned int *day, unsigned int *date, unsigned int *month, unsigned int *year)	{ 
	I2C_Start();
	I2C_Write(0xD0);     
	I2C_Write(0x00);    
	I2C_Start(); 
	I2C_Write(0xD1); 
	*second = (unsigned int)I2C_Read(1);    
	*minute = (unsigned int)I2C_Read(1);
	*hour   = (unsigned int)I2C_Read(1);
	*day    = (unsigned int)I2C_Read(1);
	*date   = (unsigned int)I2C_Read(1);
	*month  = (unsigned int)I2C_Read(1);
	*year   = (unsigned int)I2C_Read(0);
	I2C_Stop();
	#define BCD_TO_DEC(x) (((x) >> 4) * 10 + ((x) & 0x0F))
	*hour   = BCD_TO_DEC(*hour & 0x3F);
	*minute = BCD_TO_DEC(*minute & 0x7F);
	*second = BCD_TO_DEC(*second & 0x7F);
	*date   = BCD_TO_DEC(*date & 0x3F);
	*month  = BCD_TO_DEC(*month & 0x1F);
	*year   = BCD_TO_DEC(*year);
}
void DS1307_Write_Time(unsigned int hour, unsigned int minute, unsigned int second, unsigned char day, unsigned char date, unsigned char month, unsigned char year){
	#define DEC_TO_BCD(x) (((x) / 10) << 4 | ((x) % 10))
	second = DEC_TO_BCD(second);
	minute = DEC_TO_BCD(minute);
	hour   = DEC_TO_BCD(hour);
	date   = DEC_TO_BCD(date);
	month  = DEC_TO_BCD(month);
	year   = DEC_TO_BCD(year);
	if (day < 1 || day > 7) day = 1;
	I2C_Start();
	I2C_Write(0xD0);
	I2C_Write(0x00); 
	I2C_Write(second); 
	I2C_Write(minute);
	I2C_Write(hour);
	I2C_Write(day); 
	I2C_Write(date);
	I2C_Write(month);
	I2C_Write(year);
	I2C_Stop();
}