/**
  @page ADC_RegularConversion_Interrupt conversion using Interrupt
  * Modified 6-May-2017 RP for Arduino / STM32GENERIC platform
  @verbatim
  ******************** (C) COPYRIGHT 2017 STMicroelectronics *******************
  * @file    ADC/ADC_RegularConversion_Interrupt/readme.txt 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    17-February-2017
  * @brief   Description of the ADC RegularConversion Interrupt example.
  ******************************************************************************
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  @endverbatim
@par Hardware and Software environment 

  - This example runs on STM32F407xx devices under Arduino and has been optimised for the STM32GENERIC codeset: https://github.com/danieleff/STM32GENERIC
  
@par Example Description 

This example describes how to 
- use the ADC in interrupt mode to convert data through the HAL API.
- use ADC1 and Interrupt to convert continuously  data from ADC1 channel1.

The user needs to supply some way to vary the ADC1 channel1 input value. Three options are
- a potentiometer between 3.3v and GND, 
- a signal generator with DC coupled output of 0 < input < 3.3v
- using a DAC to generate a signal


Each time an end of conversion occurs an interrupt is generated and the converted 
data of ADC1 DR register is affected to the uhADCxConvertedValue variable in the 
ADC conversion complete call back function.

*** Given the overhead of HAL interrupt handling, reducing the sampling time below ADC_SAMPLETIME_56CYCLES does not allow any time for processing the main loop().


STM32 board's LEDs can be used to monitor the transfer status:
  - LED3 is ON when there are an error in initialization.

@note Care must be taken when using Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
