/*
   RTClock lib  demo for F1  only  by huaweiwx@sina.com 2017.12
*/

#include <LED.h>
#include <RTClock.h>

RTClock  rtc;

void setup() {
  Led.Init();
  Led.flash(10, 320, 5);

  // initialize serial communication at 115200 bits per second:
  Serial.begin(115200);

  rtc.Init(RTC_CLOCK_SOURCE_LSE);/* can select: RTC_CLOCK_SOURCE_LSE/RTC_CLOCK_SOURCE_LSI*/

  if (rtc) {

    /*for RTC_CLOCK_SOURCE_LSE, run once only 当选择低频晶振且装有后备电池时仅需运行一次*/
    rtc.setDataTime(17, 12, 18, 16, 25, 10);
    rtc.attachEventInterrupt(&showDataTime);
  }

  else
    Serial.println("\nRTC init false!");

  Serial.println("\nSnoring...");
}

// the loop routine runs over and over again forever:
void loop() {
  if (Led.availablePwm())
    Led.fade(1000);
  else
    Led.flash(500, 500, 1);

  //  showDataTime();
}

const char* weekStr[7] = {
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday",
};

void showDataTime(void)
{
  rtc.getDataTime();
  Serial << "data: " << rtc.pCalendar->year << "-"  <<  rtc.pCalendar->month << "-" <<  rtc.pCalendar->day ;
  Serial << "   time: " <<  rtc.pCalendar->hour << ":"  <<  rtc.pCalendar->minute << ":" <<  rtc.pCalendar->second << "  " <<  weekStr[ rtc.pCalendar->week - 1] << "\n";
}
