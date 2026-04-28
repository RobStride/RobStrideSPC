/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "robstride_control.h"


void SystemClock_Config(void);

uint8_t mode = 0;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_CAN1_Init();

  while (1)
  {
		switch (mode) 
		{
			// ===== 私有协议模式 =====	
			case 0://使能（私有协议）
				RobStride_motor_enable();
				break;
			case 1://失能（私有协议）
				RobStride_motor_reset();
				break;
			case 2://运控模式（私有协议）
				RobStrideMotor_move_control(1, 0, 1, 0.1, 0.1);
				break;
			case 3://PP位置模式（私有协议）
				RobStride_Motor_Pos_PP_control(6, 1, 2);
				break;
			case 4://CSP位置模式（私有协议）
				RobStride_Motor_Pos_CSP_control(4,1);
				break;
			case 5://速度模式（私有协议）
				RobStride_Motor_Speed_control(1, 1, 2);
				break;
			case 6://电流模式（私有协议）
				RobStride_Motor_current_control(0.5);
				break;
			case 7://设置零位（私有协议）
				RobStride_Set_ZreoPos();
				break;
			case 8://设置电机ID（私有协议）
				RobStride_Set_CAN_ID(0x01);
				break;
			case 9://参数保存（私有协议）
				RobStride_Motor_MotorDataSave();
				break;
			case 10://修改波特率（私有协议）
				RobStride_Motor_BaudRateChange(0);
				break;
			case 11://主动上报（私有协议）
				RobStride_Motor_ProtactiveEscalationSet(1);
				break;
			case 12://切换协议（私有协议）/（MIT协议）
				RobStride_Motor_MIT_ModeSet(1);
				break;
			case 13://使能（MIT协议）
				RobStride_Motor_MIT_enable();
				break;
			case 14://失能（MIT协议）
				RobStride_Motor_MIT_reset();
				break;
			case 15://清除错误及读取异常状态（MIT协议）
				RobStride_Motor_MIT_ClearOrCheckError(0xFF);
				break;
			case 16://MIT设置运行模式（MIT协议）
				RobStride_Motor_MIT_SetMotorType(1);
				break;
			case 17://MIT设置电机ID（MIT协议）
				RobStride_Motor_MIT_SetMotorId(1);
				break;
			case 18://MIT控制模式（MIT协议）
				RobStride_Motor_MIT_Control(1, 0, 1, 0.1, 0.1);
				break;
			case 19://MIT位置模式（MIT协议）
				RobStride_Motor_MIT_PositionControl(0,1);
				break;
			case 20://MIT速度模式（MIT协议）
				RobStride_Motor_MIT_SpeedControl(10,2);
				break;
			case 21://MIT零点设置（MIT协议）
				RobStride_Motor_MIT_SetZeroPos();
				break;
			case 22://MIT数据保存（MIT协议）（需升级到最新固件）
				RobStride_Motor_MIT_MotorDataSave();
				break;
			case 23://MIT主动上报（MIT协议）（需升级到最新固件）
				RobStride_Motor_MIT_ProtactiveEscalationSet(1);
				break;
			case 24://MIT读参数（MIT协议）（需升级到最新固件）
				Get_RobStride_Motor_MIT_parameter(0x7005);
				break;
			case 25://MIT写参数（MIT协议）（需升级到最新固件）
				Set_RobStride_Motor_MIT_parameter(0x7005,10);
				break;
		}
		HAL_Delay(100);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
