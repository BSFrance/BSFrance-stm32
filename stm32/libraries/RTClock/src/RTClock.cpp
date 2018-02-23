
#include <Arduino.h>
#include "RTClock.h"

extern "C" void _Error_Handler(char* file, uint32_t line);
RTC_HandleTypeDef hrtc;

#ifdef STM32F1
const uint8_t mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef* hrtc);
static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef* hrtc);
static HAL_StatusTypeDef RTC_SetCounter(RTC_HandleTypeDef* hrtc, uint32_t TimeCounter);
static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef* hrtc);
static HAL_StatusTypeDef  RTC_WriteAlarmCounter(RTC_HandleTypeDef* hrtc, uint32_t AlarmCounter);

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
static bool leapYear(uint16_t year)
{
    if (year % 100 == 0)
    {
      if (year % 400 == 0) return true;//如果以00结尾,还要能被400整除
    } 
	
	if (year % 4 == 0) //必须能被4整除
    		return true;

	return false;
}


static HAL_StatusTypeDef RTC_SetCounter(RTC_HandleTypeDef* hrtc, uint32_t TimeCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->CNTH, (TimeCounter >> 16));
    /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->CNTL, (TimeCounter & RTC_CNTL_RTC_CNT));

    /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

static uint32_t RTC_ReadTimeCounter(RTC_HandleTypeDef* hrtc)
{
  uint16_t high1 = 0, high2 = 0, low = 0;
  uint32_t timecounter = 0;

  high1 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc->Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  { /* In this case the counter roll over during reading of CNTL and CNTH registers,
      read again CNTL register then return the counter value */
    timecounter = (((uint32_t) high2 << 16 ) | READ_REG(hrtc->Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  { /* No counter roll over during reading of CNTL and CNTH registers, counter
      value is equal to first value of CNTL and CNTH */
    timecounter = (((uint32_t) high1 << 16 ) | low);
  }

  return timecounter;
}

static HAL_StatusTypeDef RTC_EnterInitMode(RTC_HandleTypeDef* hrtc)
{
  uint32_t tickstart = 0;

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_DISABLE(hrtc);


  return HAL_OK;
}

static HAL_StatusTypeDef RTC_ExitInitMode(RTC_HandleTypeDef* hrtc)
{
  uint32_t tickstart = 0;

  /* Disable the write protection for RTC registers */
  __HAL_RTC_WRITEPROTECTION_ENABLE(hrtc);

  tickstart = HAL_GetTick();
  /* Wait till RTC is in INIT state and if Time out is reached exit */
  while ((hrtc->Instance->CRL & RTC_CRL_RTOFF) == (uint32_t)RESET)
  {
    if ((HAL_GetTick() - tickstart) >  RTC_TIMEOUT_VALUE)
    {
      return HAL_TIMEOUT;
    }
  }

  return HAL_OK;
}

static HAL_StatusTypeDef RTC_writeAlarmCounter(RTC_HandleTypeDef* hrtc, uint32_t AlarmCounter)
{
  HAL_StatusTypeDef status = HAL_OK;

  /* Set Initialization mode */
  if (RTC_EnterInitMode(hrtc) != HAL_OK)
  {
    status = HAL_ERROR;
  }
  else
  {
    /* Set RTC COUNTER MSB word */
    WRITE_REG(hrtc->Instance->ALRH, (AlarmCounter >> 16));
    /* Set RTC COUNTER LSB word */
    WRITE_REG(hrtc->Instance->ALRL, (AlarmCounter & RTC_ALRL_RTC_ALR));

    /* Wait for synchro */
    if (RTC_ExitInitMode(hrtc) != HAL_OK)
    {
      status = HAL_ERROR;
    }
  }

  return status;
}

#endif
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
//u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表
//const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

HAL_StatusTypeDef RTClock::setDataTime(uint16_t syear, uint8_t smon, uint8_t sday, uint8_t hour, uint8_t min, uint8_t sec)
{

#ifdef STM32F1
  uint16_t t;
  uint32_t seccount = 0;
  if (syear < 2000 || syear > 2129)
    return HAL_ERROR;
  for (t = 2000; t < syear; t++) //°把所有年份的秒钟相加
  {
    if (leapYear(t))
      seccount += 31622400; //闰年的秒钟数
    else
      seccount += 31536000;     //平年的秒钟数
  }
  smon -= 1;
  for (t = 0; t < smon; t++) //把前面月份的秒钟数相加
  {
    seccount += (uint32_t)mon_table[t] * 86400; //月份秒钟数相加
    if (leapYear(syear) && t == 1)
      seccount += 86400; //闰年2月份增加一天的秒钟数
  }
  seccount += (uint32_t)(sday - 1) * 86400; //把前面日期的秒钟数相加
  seccount += (uint32_t)hour * 3600; //小时秒钟数
  seccount += (uint32_t)min * 60; //分钟秒钟数
  seccount += sec; //最后的秒钟加上去
  hrtc.State = HAL_RTC_STATE_BUSY;
  __HAL_LOCK(&hrtc);
  if (RTC_SetCounter(&hrtc, seccount) != HAL_OK)
  {
    hrtc.State = HAL_RTC_STATE_ERROR;
    return HAL_ERROR;
  }
  else
  {
    hrtc.State = HAL_RTC_STATE_READY;
  }
  __HAL_UNLOCK(&hrtc);
  return HAL_OK;
#elif defined(STM32F4)||defined(STM32L4)||defined(STM32L0)
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday April 14th 2015 */
  sdatestructure.Year = syear;
  sdatestructure.Month = smon;
  sdatestructure.Date = sday;
  sdatestructure.WeekDay = this->getWeek(((syear<100)?(syear+2000):syear), smon, sday);

  assert_param(IS_RTC_YEAR(syear));
  assert_param(IS_RTC_MONTH(smon));
  assert_param(IS_RTC_DATE(sday));
  assert_param(IS_RTC_HOUR24(RTC_Bcd2ToByte(hour)));
  assert_param(IS_RTC_MINUTES(RTC_Bcd2ToByte(min)));
  assert_param(IS_RTC_SECONDS(RTC_Bcd2ToByte(sec)));
 
  if(HAL_RTC_SetDate(&hrtc,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK) //RTC_FORMAT_BCD
  {
    /* Initialization Error */
	_Error_Handler((char *)__FILE__, __LINE__);
  } 
  
  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = hour;
  stimestructure.Minutes = min;
  stimestructure.Seconds = sec;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  if(HAL_RTC_SetTime(&hrtc,&stimestructure,RTC_FORMAT_BIN) != HAL_OK) //RTC_FORMAT_BCD
  {
    /* Initialization Error */
	_Error_Handler(__FILE__, __LINE__);
  }
  
  return HAL_OK;
#endif
}

HAL_StatusTypeDef RTClock::setAlarmTime(uint8_t hour, uint8_t min, uint8_t sec,uint8_t week)
{
  uint16_t syear;
  uint8_t smon;
  uint8_t sday;
  
  this->getDataTime();
  syear = calendar.year;
  smon = calendar.month;
  sday =calendar.day;
  
#ifdef STM32F1
  uint16_t t;
  uint32_t seccount = 0;
  if (syear < 100) syear +=2000;
  if (syear > 2129)
    return HAL_ERROR;
  for (t = 2000; t < syear; t++)
  {
    if (leapYear(t))
      seccount += 31622400; //
    else
      seccount += 31536000;     //
  }
  smon -= 1;
  for (t = 0; t < smon; t++) //
  {
    seccount += (uint32_t)mon_table[t] * 86400; //
    if ((syear) && t == 1)
      seccount += 86400; //
  }
  seccount += (uint32_t)(sday - 1) * 86400; //
  seccount += (uint32_t)hour * 3600; //
  seccount += (uint32_t)min * 60; //
  seccount += sec; //
  hrtc.State = HAL_RTC_STATE_BUSY;
  __HAL_LOCK(&hrtc);
  if (RTC_WriteAlarmCounter(&hrtc, seccount) != HAL_OK)
  {
    hrtc.State = HAL_RTC_STATE_ERROR;
    return HAL_ERROR;
  }
  else
  {
    hrtc.State = HAL_RTC_STATE_READY;
  }
  __HAL_LOCK(&hrtc);

#elif defined(STM32F4)||defined(STM32L4)||defined(STM32L0)
    /**Enable the Alarm A 
    */
  RTC_AlarmTypeDef sAlarm;
  
  sAlarm.AlarmTime.Hours = hour;
  sAlarm.AlarmTime.Minutes = min;
  sAlarm.AlarmTime.Seconds = sec;
  sAlarm.AlarmTime.SubSeconds = 0x56;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
  if(week == 0){
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  	sAlarm.AlarmDateWeekDay = 1;
  }else{
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
  	sAlarm.AlarmDateWeekDay = week;
  }	
	sAlarm.Alarm = RTC_ALARM_A;

  
  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm,RTC_FORMAT_BIN ); //RTC_FORMAT_BCD
  
#endif
  return HAL_OK;
}


void RTClock::getDataTime(void)
{
#ifdef STM32F1
  static uint16_t daycnt = 0;
  uint32_t timecount = 0;
  uint32_t temp = 0;
  uint16_t temp1 = 0;
  timecount = RTC_ReadTimeCounter(&hrtc);
  temp = timecount / 86400; //得到天数(秒钟数对应的)
  if (daycnt != temp) //超过一天了
  {
    daycnt = temp;
    temp1 = 2000; //从2000年开始
    while (temp >= 365)
    {
      if (leapYear(temp1)) //是闰年
      {
        if (temp >= 366)
          temp -= 366; //闰年的秒钟数
        else
        {
          temp1++;
          break;
        }
      }
      else
        temp -= 365;  //平年
      temp1++;
    }
    calendar.year = temp1; //得到年份
    temp1 = 0;
    while (temp >= 28) //超过了一个月
    {
      if (leapYear(calendar.year) && temp1 == 1) //当年是不是闰年/2月份
      {
        if (temp >= 29)
          temp -= 29; //闰年的秒钟数
        else
          break;
      }
      else
      {
        if (temp >= mon_table[temp1])
          temp -= mon_table[temp1]; //平年
        else
          break;
      }
      temp1++;
    }
    calendar.month = temp1 + 1; //得到月份
    calendar.day = temp + 1; //得到日期
  }
  temp = timecount % 86400;     //得到秒钟数
  calendar.hour = temp / 3600;  //小时
  calendar.minute = (temp % 3600) / 60; //分钟
  calendar.second = (temp % 3600) % 60; //秒钟
  calendar.week = this->getWeek(calendar.year, calendar.month, calendar.day); //获取星期
#elif defined(STM32F4)||defined(STM32L4)||defined(STM32L0)
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;	
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  calendar.hour = stimestructureget.Hours;  //小时
  calendar.minute =stimestructureget.Minutes ; //分钟
  calendar.second = stimestructureget.Seconds; //秒钟
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  calendar.year=2000 + sdatestructureget.Year;
  calendar.month = sdatestructureget.Month; //得到月份
  calendar.day = sdatestructureget.Date; //得到日期
  calendar.week = this->getWeek(calendar.year, calendar.month, calendar.day); //获取星期
#endif
}

void RTClock::writeBackupReg(uint32_t BackupRegister, uint32_t Data){
	HAL_RTCEx_BKUPWrite(&hrtc, BackupRegister,Data);
}

uint32_t RTClock::readBackupReg(uint32_t BackupRegister){
	return HAL_RTCEx_BKUPRead(&hrtc,BackupRegister);
}		

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许2000-2129年)
//输入参数：公历年月日
//返回值：星期号
uint8_t RTClock::getWeek(uint16_t year, uint8_t month, uint8_t day)
{
  int iweek = 0;
  uint8_t y, c;
  uint8_t m, d;
  if (month == 1 || month == 2)
  {
    c = (year - 1) / 100;
    y = (year - 1) % 100;
    m = month + 12;

  }
  else
  {
    c = year / 100;
    y = year % 100;
    m = month;
  }
  d = day;
  iweek = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + d - 1;
  iweek = iweek >= 0 ? (iweek % 7) : (iweek % 7 + 7);
  if (iweek == 0)
  {
    iweek = 7;
  }
  return iweek;
}

#if 0
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate, uint8_t *showweek)
{
  /* Display time Format : hh:mm:ss */
  sprintf((char *)showdate, "%2d年%2d月%2d日---", calendar.year, calendar.month, calendar.day);
  printf((char*)showdate);
  sprintf((char *)showtime, "%2d点%2d分%2d秒---", calendar.hour, calendar.minute, calendar.second);
  printf((char*)showtime);
  sprintf((char*)showweek, "星期%d", calendar.week);
  printf((char*)showweek);
  printf("\r\n");
  /* Display date Format : mm-dd-yy */

}
#endif

//#define RCC_RTCCLKSOURCE_NO_CLK  		0x00000000U   /*!< No clock */
//#define RCC_RTCCLKSOURCE_LSE      	0x00000100U   /*!< LSE oscillator clock used as RTC clock */
//#define RCC_RTCCLKSOURCE_LSI       	0x00000200U   /*!< LSI oscillator clock used as RTC clock */
//#define RCC_RTCCLKSOURCE_HSE_DIV128   0x00000300U   /*!< HSE oscillator clock divided by 128 used as RTC clock */

HAL_StatusTypeDef RTClock::rtcSource(uint32_t source){
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  switch (source) {
	case RTC_CLOCK_SOURCE_LSE: /*LSE*/
        /* Configue LSE as RTC clock soucre */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        RCC_OscInitStruct.LSEState = RCC_LSE_ON;
		RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
     	break;
	case RTC_CLOCK_SOURCE_LSI:  /*LSI*/
        /* Configue LSI as RTC clock soucre */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
        RCC_OscInitStruct.LSIState = RCC_LSI_ON;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
		break;
	case RTC_CLOCK_SOURCE_HSE:
        /* Configue HSE as RTC clock soucre */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE||RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
		RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;		
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE;
		break;
	default:
       	return HAL_ERROR;   //use  board default souce,inited in variant.c 
  }
  
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) == HAL_OK){
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    return HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
  }
  else
	 _Error_Handler(__FILE__, __LINE__);
  return HAL_ERROR;
}


/* RTC init function */
void RTClock::Init(uint32_t source)
{
  /**Initialize RTC Only
  */
    if(rtcSource(source) != HAL_OK)	 _Error_Handler(__FILE__, __LINE__);
	hrtc.Instance = RTC;
	
#if defined(STM32F4)||defined(STM32L4)||defined(STM32L0)   /*F0/F2/F3/F4/F7/L0/L1/L4*/
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	
	if(source == RTC_CLOCK_SOURCE_LSE){
		hrtc.Init.AsynchPrediv =RTC_ASYNCH_PREDIV_LSE; //RTC_ASYNCH_PREDIV;
		hrtc.Init.SynchPrediv = RTC_SYNCH_PREDIV_LSE;  //RTC_SYNCH_PREDIV;
	}else if(source == RTC_CLOCK_SOURCE_LSI){	
		hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV_LSI;//RTC_ASYNCH_PREDIV;
		hrtc.Init.SynchPrediv   = RTC_SYNCH_PREDIV_LSI; //RTC_SYNCH_PREDIV;
	}else if(source == RTC_CLOCK_SOURCE_HSE){	
		hrtc.Init.AsynchPrediv = RTC_ASYNCH_PREDIV_HSE;//RTC_ASYNCH_PREDIV;
		hrtc.Init.SynchPrediv   = RTC_SYNCH_PREDIV_HSE; //RTC_SYNCH_PREDIV;
	}
	
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	__HAL_RTC_RESET_HANDLE_STATE(&hrtc);
	if (HAL_RTC_Init(&hrtc) != HAL_OK) 	_Error_Handler(__FILE__, __LINE__);

    /* Disable the write protection for RTC registers */
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);

    /* Disable the Wake-up Timer */
    __HAL_RTC_WAKEUPTIMER_DISABLE(&hrtc);

    /* In case of interrupt mode is used, the interrupt source must disabled */ 
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hrtc,RTC_IT_WUT);
 
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2) {
		this->setDataTime(17,12,19,8,18,58);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
	}
	    /**Enable the Alarm A 
    */
	RTC_AlarmTypeDef sAlarm;
	
	sAlarm.AlarmTime.Hours = 0x8;
	sAlarm.AlarmTime.Minutes = 0x8;
	sAlarm.AlarmTime.Seconds = 0x30;
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}
	
	/**Enable the Alarm B 
	*/
	sAlarm.AlarmDateWeekDay = 0x1;
	sAlarm.Alarm = RTC_ALARM_B;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}
	
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
  
#elif defined(STM32F1)   /*F1*/
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&hrtc) != HAL_OK) return;
	HAL_RTCEx_SetSecond_IT(&hrtc);
	if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2) {
		this->setDataTime(17,12,19,8,18,58);
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
	}
#endif

	this->getDataTime();//更新时间
	this->status = 1;

}


static void (*EventInterruptCallBack)(void) = NULL;
static void (*AlarmInterruptCallBack)(void) = NULL;
static void (*TamperEventCallback)(void) = NULL;

void RTClock::attachEventInterrupt(void (*func)(void)){
	EventInterruptCallBack = func;
}

void RTClock::detachEventInterrupt(void){
	EventInterruptCallBack = NULL;
}

void RTClock::attachAlarmInterrupt(void(*func)(void)){
	AlarmInterruptCallBack =func;
}
 
void RTClock::detachAlarmInterrupt(void){
	AlarmInterruptCallBack =NULL;
}

void RTClock::attachTamperInterrupt(void(*func)(void)){
	TamperEventCallback =func;
}

void RTClock::detachTamperInterrupt(void){
	TamperEventCallback =NULL;
}

extern "C" {
#ifdef STM32F1
	void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef* hrtc __unused){
		if(EventInterruptCallBack) EventInterruptCallBack();
	}
#elif defined(STM32F4)||defined(STM32L4)
	void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef* hrtc __unused){
		if(EventInterruptCallBack) EventInterruptCallBack();
	}

#else
	void HAL_RTCEx_RTCIRQHandler(RTC_HandleTypeDef* hrtc __unused){
		if(EventInterruptCallBack) EventInterruptCallBack();		
	}
#endif	

	void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef* hrtc __unused){
		if(AlarmInterruptCallBack) AlarmInterruptCallBack();
	}

    void HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef* hrtc __unused){
		if(TamperEventCallback) TamperEventCallback();		
	}
	
	void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc __unused){
		if (hrtc->Instance == RTC)
		{
#if defined(STM32F1)||defined(STM32L0)
			HAL_PWR_EnableBkUpAccess();
			__HAL_RCC_BKP_CLK_ENABLE();
			__HAL_RCC_RTC_ENABLE();
 #if __has_include("FreeRTOS.h")  //huawei (huaweiwx@sina.com)
			HAL_NVIC_SetPriority(RTC_IRQn, TickPriority, 0);
#else
			HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
#endif	
			HAL_NVIC_EnableIRQ(RTC_IRQn);


#elif defined(STM32F4)||defined(STM32L4)
			__HAL_RCC_PWR_CLK_ENABLE();
			HAL_PWR_EnableBkUpAccess();
			__HAL_RCC_RTC_ENABLE(); 
# if __has_include("FreeRTOS.h")  //huawei (huaweiwx@sina.com)
			HAL_NVIC_SetPriority(RTC_WKUP_IRQn, TickPriority, 0);
			HAL_NVIC_SetPriority(RTC_Alarm_IRQn, TickPriority, 0);
# else
			HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0, 0);
			HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);

# endif	
			HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
			HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
#else
	
#endif	
		}
	}
#if defined(STM32F1)||defined(STM32L0)	
	void RTC_IRQHandler(void){HAL_RTCEx_RTCIRQHandler(&hrtc);};
#elif defined(STM32F4)||defined(STM32L4)
	void RTC_WKUP_IRQHandler(void){HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);};
	void RTC_Alarm_IRQHandler(void){HAL_RTC_AlarmIRQHandler(&hrtc);};
#else
	
#error please add me!	
#endif	
} //extern "C"

