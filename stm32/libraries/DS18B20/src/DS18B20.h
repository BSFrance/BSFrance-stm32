#ifndef __DS18B20_H
#define __DS18B20_H 

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */ 

void DS18B20_SetPin(uint8_t pin);
uint8_t DS18B20_Init(void);			//初始化DS18B20
int DS18B20_GetTemp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_WriteByte(uint8_t dat);//写入一个字节
uint8_t DS18B20_ReadByte(void);		//读出一个字节
uint8_t DS18B20_ReadBit(void);		//读出一个位
uint8_t DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20    


#ifdef __cplusplus
 } // extern "C"
//C++ function here 
class DS18B20Class
{
   public:	
        DS18B20Class(){};
		void setPin(uint8_t pin){DS18B20_SetPin(pin);};
		uint8_t Init(void){return DS18B20_Init();}; 
		int getTemp(void){return DS18B20_GetTemp();};	//获取温度
		void  start(void){DS18B20_Start();};		//开始温度转换
		void  writeByte(uint8_t dat){DS18B20_WriteByte(dat);};//写入一个字节
		uint8_t  readByte(void){return DS18B20_ReadByte();};		//读出一个字节
		uint8_t  readBit(void){return DS18B20_ReadBit();};		//读出一个位
		uint8_t  check(void){return DS18B20_Check();};			//检测是否存在DS18B20
		void  rst(void){DS18B20_Rst();};			//复位DS18B20    
	private:
}; 
#endif /* __cplusplus */

#endif