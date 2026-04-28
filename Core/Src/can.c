/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"
#include "robstride_control.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
CAN_HandleTypeDef   hcan1 = {0};


CAN_TxHeaderTypeDef txMsg = {
    .StdId = 0x00,              // 扩展帧不用标准ID
    .ExtId = 0xFF,        			// 29bit 扩展ID (0~0x1FFFFFFF)
    .IDE   = CAN_ID_EXT,        // 扩展帧
    .RTR   = CAN_RTR_DATA,      // 数据帧
    .DLC   = 8,                 // 数据长度 0~8
    .TransmitGlobalTime = DISABLE
};

CAN_TxHeaderTypeDef MITtxMsg = {
    .StdId = 0x00,              // 标准ID
    .ExtId = 0x00,        			// 29bit 扩展ID (0~0x1FFFFFFF)
    .IDE   = CAN_ID_STD,        // 标准帧
    .RTR   = CAN_RTR_DATA,      // 数据帧
    .DLC   = 8,                 // 数据长度 0~8
    .TransmitGlobalTime = DISABLE
};

CAN_RxHeaderTypeDef rxMsg = {0};    /* 接收参数句柄 */
CAN_RxHeaderTypeDef MITrxMsg = {0};    /* 接收参数句柄 */

uint8_t tx_data[8];
uint8_t rx_data[8];

uint32_t TxMessage;

/********************************************************************************************
* Function: 
********************************************************************************************/
/**
 * @brief       CAN 发送一组数据
 * @note        发送格式固定为: 标准ID, 数据帧
 * @param       len     :数据长度,取值范围：FDCAN_DLC_BYTES_0 ~ FDCAN_DLC_BYTES_64
 * @param       msg     :数据指针,最大为8个字节
 * @retval      发送状态 0, 成功; 1, 失败;
 */
uint8_t can_send_msg(uint8_t *msg, uint32_t len)
{ 
    txMsg.DLC = len;                               /* 数据长度 */ 
 
    if(HAL_CAN_AddTxMessage(&hcan1, &txMsg, msg, &TxMessage) != HAL_OK)
    {
        return 1; //
    }
		//delay_ms(1);	
    return 0;
}

uint8_t can_MIT_send_msg(uint8_t *msg, uint32_t len)
{ 
    MITtxMsg.DLC = len;                               /* 数据长度 */ 
 
    if(HAL_CAN_AddTxMessage(&hcan1, &MITtxMsg, msg, &TxMessage) != HAL_OK)
    {
        return 1; //
    }
		//delay_ms(1);	
    return 0;
}


uint8_t can_receive_msg(uint8_t *buf)
{
    if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxMsg, buf) != HAL_OK)    /* 读取数据 */
    {
        return 0;
    }
    return rxMsg.DLC; //
}

void can_txd(void)
{
	 can_send_msg(tx_data, 8);
}

void can_MIT_txd(void)
{
	 can_MIT_send_msg(tx_data, 8);
}

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
	/* 4. 配置过滤器（全接收） */
    CAN_FilterTypeDef canfilterconfig;
    canfilterconfig.FilterBank = 0;
    canfilterconfig.FilterIdHigh = 0x0000;
    canfilterconfig.FilterIdLow  = 0x0000;
    canfilterconfig.FilterMaskIdHigh = 0x0000;
    canfilterconfig.FilterMaskIdLow  = 0x0000;
    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
    if(HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig) != HAL_OK) 
		{
			Error_Handler();
		}
  /* 5. 启动 CAN 并启用中断 */
    if(HAL_CAN_Start(&hcan1) != HAL_OK) 
		{
			Error_Handler();
		}
    if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) 
		{	
			Error_Handler();
		}
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == CAN1)
    {
			/* 中断中读取数据 */
			can_receive_msg(rx_data);
			
			parse_motor_feedback();
		}
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
