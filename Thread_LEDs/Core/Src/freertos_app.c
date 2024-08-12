/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
#include "notif_queue.h"
#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GREEN_LED_NUMBER 1
#define BLUE_LED_NUMBER 2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Definitions for Task1 */
osThreadId_t Task1Handle;
uint32_t Task1Buffer[ 128 ];
osStaticThreadDef_t Task1ControlBlock;
const osThreadAttr_t Task1_attributes = {
  .name = "Task1",
  .cb_mem = &Task1ControlBlock,
  .cb_size = sizeof(Task1ControlBlock),
  .stack_mem = &Task1Buffer[0],
  .stack_size = sizeof(Task1Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task2 */
osThreadId_t Task2Handle;
uint32_t Task2Buffer[ 128 ];
osStaticThreadDef_t Task2ControlBlock;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .cb_mem = &Task2ControlBlock,
  .cb_size = sizeof(Task2ControlBlock),
  .stack_mem = &Task2Buffer[0],
  .stack_size = sizeof(Task2Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task3 */
osThreadId_t Task3Handle;
uint32_t myTask3Buffer[ 128 ];
osStaticThreadDef_t myTask3ControlBlock;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .cb_mem = &myTask3ControlBlock,
  .cb_size = sizeof(myTask3ControlBlock),
  .stack_mem = &myTask3Buffer[0],
  .stack_size = sizeof(myTask3Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};

Queue queue1 = {.Front = NULL, .Rear = NULL, .size = 0};
Queue queue2 = {.Front = NULL, .Rear = NULL, .size = 0};

int green_state = 0;
int blue_state = 0;

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartTask1(void *argument);
void StartTask2(void *argument);
void StartTask3(void *argument);

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void RTOS_Init(){
	  /* Create the thread(s) */
	  /* creation of Task1 */
	  Task1Handle = osThreadNew(StartTask1, NULL, &Task1_attributes);

	  /* creation of Task2 */
	  Task2Handle = osThreadNew(StartTask2, NULL, &Task2_attributes);

	  /* creation of Task3 */
	  Task3Handle = osThreadNew(StartTask3, NULL, &Task3_attributes);
}

void StartTask1(void *argument)
{
  /* init code for USB_DEVICE */
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	int green_value = HAL_GPIO_ReadPin(Green_BUTTON_GPIO_Port, Green_BUTTON_Pin);

	int blue_value = HAL_GPIO_ReadPin(Blue_BUTTON_GPIO_Port, Blue_BUTTON_Pin);

	while (HAL_GPIO_ReadPin(Blue_BUTTON_GPIO_Port, Blue_BUTTON_Pin) || HAL_GPIO_ReadPin(Green_BUTTON_GPIO_Port, Green_BUTTON_Pin)){
		osDelay(5);
	}

	Notif notif_green = {.number = GREEN_LED_NUMBER};
	Notif notif_blue = {.number = BLUE_LED_NUMBER};

	notif_green.state = green_value ? 1 : 0;

	notif_blue.state = blue_value ? 1 : 0;

	queue_push(&queue1, &notif_green);
	queue_push(&queue1, &notif_blue);

	osDelay(50);
  }

  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask2 */
/**
* @brief Function implementing the Task2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask2 */
void StartTask2(void *argument)
{
  /* USER CODE BEGIN StartTask2 */
  /* Infinite loop */
  for(;;)
  {
	  Notif n_temp;

	  if (!queue_get(&queue1, &n_temp)){
		  osDelay(20);
		  continue;
	  }

	  if (n_temp.number == GREEN_LED_NUMBER){
		  green_state = n_temp.state ? !green_state : green_state;
		  n_temp.state = green_state;
	  }

	  if (n_temp.number == BLUE_LED_NUMBER){
		  blue_state = n_temp.state ? !blue_state : blue_state;
		  n_temp.state = blue_state;
	  }

	  queue_push(&queue2, &n_temp);
  }
  /* USER CODE END StartTask2 */
}

/* USER CODE BEGIN Header_StartTask3 */
/**
* @brief Function implementing the Task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask3 */
void StartTask3(void *argument)
{
  /* USER CODE BEGIN StartTask3 */
  /* Infinite loop */
  for(;;)
  {
	  Notif n_temp;
	  char data[30];

	  if (!queue_get(&queue2, &n_temp)){
		  osDelay(15);
		  continue;
	  }

	  if (n_temp.number == GREEN_LED_NUMBER){
		  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, n_temp.state);

		  snprintf(data, (sizeof(data) / sizeof(char)), "Green LED state is: %d\n", n_temp.state);
		  CDC_Transmit_FS((uint8_t *) data, strlen(data));
	  }
	  if (n_temp.number == BLUE_LED_NUMBER){
		  HAL_GPIO_WritePin(Blue_LED_GPIO_Port, Blue_LED_Pin, n_temp.state);

		  snprintf(data, (sizeof(data) / sizeof(char)), "Blue LED state is: %d\n", n_temp.state);
		  CDC_Transmit_FS((uint8_t *) data, strlen(data));
	  }

  }
  /* USER CODE END StartTask3 */
}
/* USER CODE END Application */

