#ifndef __KEYS_H__
#define __KEYS_H__

#ifdef __cplusplus

#define KEY0	bit(0)
#define KEY1	bit(1)
#define KEY2	bit(2)
#define KEY3	bit(3)
#define KEY4	bit(4)
#define KEY5	bit(5)
#define KEY6	bit(6)
#define KEY7	bit(7)

#define KEY_UNPRESED 0
#define KEY_PRESED	 1

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;     

typedef struct {
    uint8_t  pins[16];       /**< pin max 16*/
    uint8_t  nr;             /**< keys num. */
    uint16_t on;             /**< on state, 1 high or 0 low on. */
    uint16_t mode;       /*mode bit 1/0 exti/gpio*/
} BOTTON_TypeDef;


class BOTTONClass
{
  public:
    BOTTONClass();
    BOTTONClass(uint16_t mask, uint8_t k0,
			    uint8_t k1=0xff, uint8_t k2=0xff, uint8_t k3=0xff,
			    uint8_t k4=0xff, uint8_t k5=0xff, uint8_t k6=0xff, uint8_t k7=0xff,
			    uint8_t k8=0xff, uint8_t k9=0xff, uint8_t k10=0xff,uint8_t k11=0xff,
			    uint8_t k12=0xff,uint8_t k13=0xff,uint8_t k14=0xff,uint8_t k15=0xff);
	void setKey(uint8_t pin,uint16_t on,ButtonMode_TypeDef mode = BUTTON_MODE_GPIO);
    void Init();
    uint8_t  getKey(uint8_t key);
    uint16_t scan(uint8_t mode = 1);
	BOTTON_TypeDef *pdata = &bottons;
  private:
    BOTTON_TypeDef bottons;
};
#endif

#endif

