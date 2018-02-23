#include <UserMain.h>

int main(void)
{
  init();
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(115200);
	for (;;) {
    digitalToggle(LED_BUILTIN); //		loop();
    delay(1000);
    Serial.println("ok");
	}
	return 0;
}
