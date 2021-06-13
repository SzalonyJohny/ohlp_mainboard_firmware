/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
void SystemClock_Config(void);
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void _Error_Handler(char *file, int line);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW2_Pin GPIO_PIN_13
#define SW2_GPIO_Port GPIOC
#define SW2_EXTI_IRQn EXTI15_10_IRQn
#define BQ_INT_Pin GPIO_PIN_14
#define BQ_INT_GPIO_Port GPIOC
#define ADC_VD1_FB_Pin GPIO_PIN_0
#define ADC_VD1_FB_GPIO_Port GPIOA
#define ADC_VD2_FB_Pin GPIO_PIN_1
#define ADC_VD2_FB_GPIO_Port GPIOA
#define ADC_VD3_FB_Pin GPIO_PIN_2
#define ADC_VD3_FB_GPIO_Port GPIOA
#define ADC_I_IN_Pin GPIO_PIN_3
#define ADC_I_IN_GPIO_Port GPIOA
#define ADC_ID1_FB_Pin GPIO_PIN_4
#define ADC_ID1_FB_GPIO_Port GPIOA
#define ADC_ID2_FB_Pin GPIO_PIN_5
#define ADC_ID2_FB_GPIO_Port GPIOA
#define ADC_ID3_FB_Pin GPIO_PIN_6
#define ADC_ID3_FB_GPIO_Port GPIOA
#define EN_3V3_Pin GPIO_PIN_7
#define EN_3V3_GPIO_Port GPIOA
#define SMPS_EN_Pin GPIO_PIN_1
#define SMPS_EN_GPIO_Port GPIOB
#define SW1_Pin GPIO_PIN_2
#define SW1_GPIO_Port GPIOB
#define SW1_EXTI_IRQn EXTI2_IRQn
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_15
#define LED4_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOH
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
