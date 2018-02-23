#ifndef __WIRESOFT_H
#define __WIRESOFT_H
//IIC software simulate driver by huaweiwx@sina.com 2017.6.28
# ifdef __cplusplus

typedef struct {
    uint8_t   sda; //sda pin
	uint8_t   scl; //scl pin
#if defined(__arm__)
	uint32_t  status;
#else
    uint8_t   status;	//for avr
#endif	
} TWOWIRESOFT_TypeDef;

class TwoWireSoft {
 public:
    TwoWireSoft();
    TwoWireSoft(uint8_t  sda, uint8_t scl);
	void setPins(uint8_t sda,uint8_t scl),
		begin(void),                //初始化IIC的IO口				 
		start(void),
		stop(void),	  			//发送IIC停止信号
		sendByte(uint8_t txd),			//IIC发送一个字节
		ack(void),					//IIC发送ACK信号
		nAck(void),				//IIC不发送ACK信号
		writeOneByte(uint8_t daddr,uint8_t addr,uint8_t data);
	uint8_t readByte(unsigned char ack), //IIC读取一个字节
		waitAck(void), 				//IIC等待ACK信号
		readOneByte(uint8_t daddr,uint8_t addr);
	TWOWIRESOFT_TypeDef *pdev = &sdev;	
 private:
	TWOWIRESOFT_TypeDef sdev;
};
# endif //__cplusplus
#endif
















