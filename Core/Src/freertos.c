/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */
typedef StaticQueue_t osStaticMessageQDef_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



#define UNUSED_TASK_OS_DELAY 8000


// TODO verife heap water mark

// all sizes are in Bytes
#define Main_Control_Task_Buffer_size 256
osThreadId_t Main_ControlHandle;
uint32_t Main_Control_Task_Buffer[Main_Control_Task_Buffer_size];
osStaticThreadDef_t Main_Control_Task_ControlBlock;
const osThreadAttr_t Main_Control_Task_attributes = {
  .name = "Main_Control_Task",
  .cb_mem = &Main_Control_Task_ControlBlock,
  .cb_size = sizeof(Main_Control_Task_ControlBlock),
  .stack_mem = &Main_Control_Task_Buffer[0],
  .stack_size = sizeof(Main_Control_Task_Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};


#define LED_Driver_Task_Buffer_size 256
osThreadId_t LED_DriverHandle;
uint32_t LED_Driver_Task_Buffer[LED_Driver_Task_Buffer_size];
osStaticThreadDef_t LED_DriverControlBlock;
const osThreadAttr_t LED_Driver_Task_attributes = {
  .name = "LED_Driver_Task",
  .cb_mem = &LED_DriverControlBlock,
  .cb_size = sizeof(LED_DriverControlBlock),
  .stack_mem = &LED_Driver_Task_Buffer[0],
  .stack_size = sizeof(LED_Driver_Task_Buffer),
  .priority = (osPriority_t) osPriorityRealtime,
};


#define User_Buttons_Task_Buffer_size 64
osThreadId_t User_ButtonsHandle;
uint32_t User_Buttons_Task_Buffer[User_Buttons_Task_Buffer_size];
osStaticThreadDef_t User_ButtonControlBlock;
const osThreadAttr_t User_Buttons_Task_attributes = {
  .name = "User_Buttons_Task",
  .cb_mem = &User_ButtonControlBlock,
  .cb_size = sizeof(User_ButtonControlBlock),
  .stack_mem = &User_Buttons_Task_Buffer[0],
  .stack_size = sizeof(User_Buttons_Task_Buffer),

  .priority = (osPriority_t) osPriorityBelowNormal,
};


#define IMU_Gesture_Task_Buffer_size 256
osThreadId_t IMU_GestureHandle;
uint32_t IMU_GestureBuffer[IMU_Gesture_Task_Buffer_size];
osStaticThreadDef_t IMU_GestureControlBlock;
const osThreadAttr_t IMU_Gesture_Task_attributes = {
  .name = "IMU_Gesture_Task",
  .cb_mem = &IMU_GestureControlBlock,
  .cb_size = sizeof(IMU_GestureControlBlock),
  .stack_mem = &IMU_GestureBuffer[0],
  .stack_size = sizeof(IMU_GestureBuffer),
  .priority = (osPriority_t) osPriorityLow,
};


#define SoC_Estimation_Task_Buffer_size 256
osThreadId_t SoC_EstimationHandle;
uint32_t SoC_Estimation_Buffer[IMU_Gesture_Task_Buffer_size];
osStaticThreadDef_t SoC_Estimation_ControlBlock;
const osThreadAttr_t SoC_Estimation_Task_attributes = {
  .name = "SoC_Estimation_Task",
  .cb_mem = &SoC_Estimation_ControlBlock,
  .cb_size = sizeof(SoC_Estimation_ControlBlock),
  .stack_mem = &SoC_Estimation_Buffer[0],
  .stack_size = sizeof(SoC_Estimation_Buffer),

  .priority = (osPriority_t) osPriorityLow,
};

/* Definitions for Button_State */
osMessageQueueId_t Button_StateHandle;
uint8_t Button_State_Buffer[ 5 * sizeof( uint16_t ) ];
osStaticMessageQDef_t Button_State_ControlBlock;
const osMessageQueueAttr_t Button_State_attributes = {
  .name = "Button_State",
  .cb_mem = &Button_State_ControlBlock,
  .cb_size = sizeof(Button_State_ControlBlock),
  .mq_mem = &Button_State_Buffer,
  .mq_size = sizeof(Button_State_Buffer)
};

/* Definitions for Set_Current_Queue */
osMessageQueueId_t Set_Current_QueueHandle;
uint8_t Set_Current_QueueBuffer[ 5 * sizeof( uint16_t ) ];
osStaticMessageQDef_t Set_Current_QueueControlBlock;
const osMessageQueueAttr_t Set_Current_Queue_attributes = {
  .name = "Set_Current_Queue",
  .cb_mem = &Set_Current_QueueControlBlock,
  .cb_size = sizeof(Set_Current_QueueControlBlock),
  .mq_mem = &Set_Current_QueueBuffer,
  .mq_size = sizeof(Set_Current_QueueBuffer)
};





/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 64 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for xButtonSemaphore */
osSemaphoreId_t xButtonSemaphoreHandle;
osStaticSemaphoreDef_t xButtonSemaphoreControlBlock;
const osSemaphoreAttr_t xButtonSemaphore_attributes = {
  .name = "xButtonSemaphore",
  .cb_mem = &xButtonSemaphoreControlBlock,
  .cb_size = sizeof(xButtonSemaphoreControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of xButtonSemaphore */
  xButtonSemaphoreHandle = osSemaphoreNew(1, 1, &xButtonSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
#pragma GCC diagnostic pop
  /* definition and creation of State_of_Charge */
  /* definition and creation of State_of_Charge */



  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(UNUSED_TASK_OS_DELAY);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
