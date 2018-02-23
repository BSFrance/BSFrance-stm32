#ifndef VARIANT_H
#define VARIANT_H

//On-board LED pin number           PIN  // Arduino Pin Number         
#define BOARD_NR_LED            1
#define LED_BUILTIN             PC13

#define LED_BUILTIN_MASK        0xFF   //ON 1
#define STM32_LED_BUILTIN_ACTIVE_HIGH

#define BOARD_NR_KEY               0
#define KEY0_BUILTIN             PA0
#define KEY0_BUILTIN_MASK        0x01    //PRESED 1

//On-board user button
//#define USER_BTN                  PA0  // 2


// Connected to on board LIS3DSH accelerometer, CS: PE3
#define MOSI                        PA7
#define MISO                        PA6
#define SCK                         PA5
#define SS                          PA4

// Connected to on board CS43L22 audio
#define SDA                         PB9
#define SCL                         PB6

#define I2C1_100KHZ_TIMING 0x2000090E
#define I2C2_100KHZ_TIMING 0x2000090E
#define I2C3_100KHZ_TIMING 0x2000090E
#define I2C4_100KHZ_TIMING 0x2000090E

//USB

//serial bootloader address
# define SERIAL_LOAD_RAM 0x20001800U


#endif
