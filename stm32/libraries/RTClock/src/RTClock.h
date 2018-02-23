
#ifndef _RTCLOCK_H_
#define _RTCLOCK_H_


#define RTC_ASYNCH_PREDIV_LSE       (0x7f)   /* (127+1)*(255+1)=32768 1s */
#define RTC_SYNCH_PREDIV_LSE        (0xff)

#if defined(STM32F0)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)   /* (127+1)*(288+1)=37000 1s*/
	#define RTC_SYNCH_PREDIV_LSI    (0x120)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (9)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV32
#elif defined(STM32F1)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)  /* (127+1)*(249+1)=32000 */
	#define RTC_SYNCH_PREDIV_LSI    (0xf9)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (9)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV128
#elif defined(STM32F2)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)  /* (127+1)*(249+1)=32000 */
	#define RTC_SYNCH_PREDIV_LSI    (0xf9)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (9)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV31
#elif defined(STM32F3)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)   /* (127+1)*(311+1)=40000*/
	#define RTC_SYNCH_PREDIV_LSI    (0x137)
	#define RTC_ASYNCH_PREDIV_HSE   (49)
	#define RTC_SYNCH_PREDIV_HSE    (4)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV32
#elif defined(STM32F4)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)  /* (127+1)*(249+1)=32000 */
	#define RTC_SYNCH_PREDIV_LSI    (0xf9)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (9)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV31
#elif defined(STM32F7)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)  /* (127+1)*(249+1)= 32000 */
	#define RTC_SYNCH_PREDIV_LSI    (0xf9)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (99)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV31
#elif defined(STM32L0)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)   /* (127+1)*(288+1)=37000 */
	#define RTC_SYNCH_PREDIV_LSI    (0x120)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (99)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV16
#elif defined(STM32L1)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)   /* (127+1)*(288+1)=37000 */
	#define RTC_SYNCH_PREDIV_LSI    (0x120)
	#define RTC_ASYNCH_PREDIV_HSE   (99)
	#define RTC_SYNCH_PREDIV_HSE    (99)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV16
#elif defined(STM32L4)
	#define RTC_ASYNCH_PREDIV_LSI   (0x7f)  /* (127+1)*(249+1)= 32000 */
	#define RTC_SYNCH_PREDIV_LSI    (0xf9)
	#define RTC_ASYNCH_PREDIV_HSE   (49)
	#define RTC_SYNCH_PREDIV_HSE    (99)
	#define RCC_RTCCLKSOURCE_HSE	RCC_RTCCLKSOURCE_HSE_DIV32
#else
#error !mcu serial line undef!	
#endif

typedef enum RTC_clockSouce {
	RTC_CLOCK_SOURCE_NO_CLK,
	RTC_CLOCK_SOURCE_LSE,
	RTC_CLOCK_SOURCE_LSI,
	RTC_CLOCK_SOURCE_HSE,
}RTC_clockSouce;

/* USER CODE BEGIN Includes */
typedef struct
{
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t  week;
} RTC_calendar_TypeDef;


#ifdef __cplusplus


class RTClock {
 public:
 	RTClock(){
	};
	void Init(uint32_t source);
	HAL_StatusTypeDef
		setDataTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec),
		setAlarmTime(uint8_t hour, uint8_t min, uint8_t sec,uint8_t week=0);
	void getDataTime(void);
	uint8_t getWeek(uint16_t year, uint8_t month, uint8_t day);
	
	void writeBackupReg(uint32_t BackupRegister, uint32_t Data);
	uint32_t readBackupReg(uint32_t BackupRegister);
	
	operator bool(){
         return (this->status)?true:false;
	};

	void attachEventInterrupt(void(*callback)(void)); 
	void detachEventInterrupt(void);
	
    void attachAlarmInterrupt(void(*callback)(void)); 
	void detachAlarmInterrupt(void);
	
    void attachTamperInterrupt(void(*callback)(void)); 
	void detachTamperInterrupt(void);

	RTC_calendar_TypeDef *pCalendar =&calendar;
	
 private:
	HAL_StatusTypeDef rtcSource(uint32_t source);
    uint8_t status = false;
	RTC_calendar_TypeDef calendar;
};

#endif //__cplusplus

#endif //_RTCLOCK_H_
