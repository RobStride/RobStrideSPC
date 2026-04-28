/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


struct exCanIdInfo{
        uint32_t id:8;
        uint32_t data:16;
				uint32_t mode:5;
        uint32_t res:3;
};

struct mitexCanIdInfo{
				uint32_t id:11;
};

#define txCanIdEx (*((struct exCanIdInfo*)&(txMsg.ExtId)))
#define rxCanIdEx (*((struct exCanIdInfo*)&(rxMsg.ExtId))) //将扩展帧id 解析为自定义数据结构
	
#define mittxCanIdEx (*((struct mitexCanIdInfo*)&(MITtxMsg.StdId)))
#define mitrxCanIdEx (*((struct mitexCanIdInfo*)&(MITrxMsg.StdId))) //将标准帧id 解析为自定义数据结构

extern CAN_HandleTypeDef   hcan1;
extern CAN_TxHeaderTypeDef txMsg ;    /* 发送参数句柄 */
extern CAN_RxHeaderTypeDef rxMsg ;

extern CAN_TxHeaderTypeDef MITtxMsg ;    /* 发送参数句柄 */
extern CAN_RxHeaderTypeDef MITrxMsg ;

extern uint8_t tx_data[8];
extern uint8_t rx_data[8];

void MX_CAN1_Init(void);
void can_txd(void);
void can_MIT_txd(void);
uint8_t can_receive_msg(uint8_t *buf);
uint8_t can_send_msg(uint8_t *msg, uint32_t len);
uint8_t can_MIT_send_msg(uint8_t *msg, uint32_t len);
extern void motor_enable(uint8_t id, uint16_t master_id);
extern void motor_controlmode(uint8_t id, float torque, float MechPosition,float speed, float kp, float kd);
extern void parse_motor_feedback(void);


#ifdef __cplusplus
}

#endif

#endif /* __CAN_H__ */




