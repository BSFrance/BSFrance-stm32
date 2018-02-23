#include "stm32_build_defines.h"
#include "stm32_def.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"

#if defined(USE_HSI)
void SystemClock_Config(void) {

    LL_RCC_HSI_Enable();
	while(LL_RCC_HSI_IsReady() != 1)
	{
	}
    LL_RCC_HSI_SetCalibTrimming(16);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);

    LL_RCC_PLL_Enable();

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL);

    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    SysTick_Config(48000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(48000000);

    /* SysTick_IRQn interrupt configuration */
#if __has_include("FreeRTOS.h")  //huawei (huaweiwx@sina.com)
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
#else
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
#endif
}

#else   //HSE
//HSE
void SystemClock_Config(void) {

    LL_RCC_HSE_Enable();
//    LL_RCC_HSE_EnableBypass();

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);

    LL_RCC_PLL_Enable();

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /*

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
*/
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);
/*
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
*/
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    SysTick_Config(72000000);

    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

    LL_SetSystemCoreClock(72000000);

    //HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    //HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
//#if __has_include("FreeRTOS.h")  //huawei (huaweiwx@sina.com)
//  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
//  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
//#else
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
//#endif
}

#endif
