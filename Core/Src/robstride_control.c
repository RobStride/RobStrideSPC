#include "main.h"
#include "can.h"
#include "robstride_control.h"
#include <string.h>

motor_feedback_t mf;
/*******************************************************************************
* @功能     		: float浮点数转int型
* @参数1        : 需要转换的值
* @参数2        : x的最小值
* @参数3        : x的最大值
* @参数4        : 需要转换的进制数
* @返回值 			: 十进制的int型整数
* @概述  				: None
*******************************************************************************/
static int float_to_uint(float x, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	if(x > x_max) x=x_max;
	else if(x < x_min) x= x_min;
	return (int) ((x-offset)*((float)((1<<bits)-1))/span);
}

/*******************************************************************************
* @功能     		: uint16_t型转float型浮点数
* @参数1        : 需要转换的值
* @参数2        : x的最小值
* @参数3        : x的最大值
* @参数4        : 需要转换的进制数
* @返回值 			: 十进制的float型浮点数
* @概述  				: None
*******************************************************************************/
static float uint16_to_float(uint16_t x, float min, float max)
{
    return ((float)x) / 65535.0f * (max - min) + min;
}

/*******************************************************************************
* @功能     		: uint8_t数组转float浮点数
* @参数        	: 需要转换的数组
* @返回值 			: 十进制的float型浮点数
* @概述  				: None
*******************************************************************************/
float Byte_to_float(uint8_t* bytedata)  
{  
	uint32_t data = bytedata[7]<<24|bytedata[6]<<16|bytedata[5]<<8|bytedata[4];
	float data_float = *(float*)(&data);
  return data_float;  
}  

/*******************************************************************************
* @功能     		: RobStride电机使能 （通信类型3）
* @参数         : None
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_motor_enable(void)
{
	txCanIdEx.mode = MOTOR_IN;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	memset(tx_data, 0, 8);
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机失能 （通信类型4）
* @参数         : 是否清除错误位（0不清除 1清除）
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_motor_reset(void)
{
	txCanIdEx.mode = MOTOR_RESET;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	for(uint8_t i=0;i<8;i++)
	{
		tx_data[i]=0;
	}
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机写入参数 （通信类型18）
* @参数1        : 参数地址
* @参数2        : 参数数值
* @参数3        : 选择是传入控制模式 还是其他参数 （Set_mode设置控制模式 Set_parameter设置参数）
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void Set_RobStride_Motor_parameter(uint16_t Index, float Value, char Value_mode)
{
	txCanIdEx.mode = MOTOR_PARAWRITE;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	tx_data[0] = Index;
	tx_data[1] = Index>>8;
	tx_data[2] = 0x00;
	tx_data[3] = 0x00;	
	if (Value_mode == 'p')
	{
		memcpy(&tx_data[4],&Value,4);
	}
	else if (Value_mode == 'j')
	{
		mf.motorMode=move_control_mode;
		tx_data[4] = (uint8_t)Value;
		tx_data[5] = 0x00;	
		tx_data[6] = 0x00;	
		tx_data[7] = 0x00;	
	}
  can_txd();
	HAL_Delay(1);
}

/*******************************************************************************
* @功能     		: RobStride电机单个参数读取 （通信类型17）
* @参数         : 参数地址
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void Get_RobStride_Motor_parameter(uint16_t Index)
{
	txCanIdEx.mode = MOTOR_PARAREAD;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	tx_data[0] = Index;
	tx_data[1] = Index>>8;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机运控模式  （通信类型1）
* @参数1        : 力矩（-4Nm~4Nm）
* @参数2        : 目标角度(-4π~4π)
* @参数3        : 目标角速度(-30rad/s~30rad/s)
* @参数4        : Kp(0.0~500.0)
* @参数5        : Kp(0.0~5.0)
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStrideMotor_move_control(float torque, float MechPosition,float speed, float kp, float kd)
{
	if(mf.mms==running)
	{
		RobStride_motor_reset();
	}
	Set_RobStride_Motor_parameter(0x7005, move_control_mode,Set_mode);
	RobStride_motor_enable();
	txCanIdEx.mode = MOTOR_CTRL;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = float_to_uint(torque,T_MIN,T_MAX,16);
	txMsg.DLC = 8;
	tx_data[0]=float_to_uint(MechPosition,P_MIN,P_MAX,16)>>8;
	tx_data[1]=float_to_uint(MechPosition,P_MIN,P_MAX,16);
	tx_data[2]=float_to_uint(speed,V_MIN,V_MAX,16)>>8;
	tx_data[3]=float_to_uint(speed,V_MIN,V_MAX,16);
	tx_data[4]=float_to_uint(kp,KP_MIN,KP_MAX,16)>>8;
	tx_data[5]=float_to_uint(kp,KP_MIN,KP_MAX,16);
	tx_data[6]=float_to_uint(kd,KD_MIN,KD_MAX,16)>>8;
	tx_data[7]=float_to_uint(kd,KD_MIN,KD_MAX,16);
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机速度模式 
* @参数1        : 目标角速度(-30rad/s~30rad/s)
* @参数2        : 目标电流限制(0~23A)
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_Motor_Speed_control(float limit_cur,float Speed_acc,float Speed)
{
	if(mf.mms==running)
	{
		RobStride_motor_reset();
	}
	Set_RobStride_Motor_parameter(0X7005, Speed_control_mode, Set_mode);		//设置电机模式
	RobStride_motor_enable();
		
	Set_RobStride_Motor_parameter(0X7018, limit_cur, Set_parameter);
	Set_RobStride_Motor_parameter(0X7022, Speed_acc, Set_parameter);	
	Set_RobStride_Motor_parameter(0X700A, Speed, Set_parameter);
}

/*******************************************************************************
* @功能     		: RobStride电机位置模式(PP插补位置模式控制)
* @参数1        : 目标角速度(-30rad/s~30rad/s)
* @参数2        : 目标角度(-4π~4π)
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_Motor_Pos_PP_control(float Speed, float Speed_acc, float Angle)
{
	if(mf.mms==running)
	{
		RobStride_motor_reset();
	}
	Set_RobStride_Motor_parameter(0X7005, Pos_PP_control_mode, Set_mode);		//设置电机模式
	RobStride_motor_enable();
		
	Set_RobStride_Motor_parameter(0X7024, Speed, Set_parameter);
	Set_RobStride_Motor_parameter(0X7025, Speed_acc, Set_parameter);	
	Set_RobStride_Motor_parameter(0X7016, Angle, Set_parameter);
}

/*******************************************************************************
* @功能     		: RobStride电机位置模式(CSP位置模式控制)
* @参数1        : 目标角度(-4π~4π)
* @参数2        : 目标角速度(0rad/s~44rad/s)
* @返回值 				: void
* @概述  				: None
*******************************************************************************/
void RobStride_Motor_Pos_CSP_control(float Speed, float Angle)
{
	if(mf.mms==running)
	{
		RobStride_motor_reset();
	}
	Set_RobStride_Motor_parameter(0X7005, Pos_CSP_control_mode, Set_mode);		//设置电机模式
	RobStride_motor_enable();
		
	Set_RobStride_Motor_parameter(0X7017, Speed, Set_parameter);
	Set_RobStride_Motor_parameter(0X7016, Angle, Set_parameter);
}

/*******************************************************************************
* @功能     		: RobStride电机电流模式
* @参数         : 目标电流(-23~23A)
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_Motor_current_control(float cuttent)
{
	if(mf.mms==running)
	{
		RobStride_motor_reset();
	}
	Set_RobStride_Motor_parameter(0X7005, Elect_control_mode, Set_mode);		//设置电机模式
	RobStride_motor_enable();
		
	Set_RobStride_Motor_parameter(0X7006, cuttent, Set_parameter);
}

/*******************************************************************************
* @功能     		: RobStride电机设置机械零点 （通信类型6）
* @参数         : None
* @返回值 			: void
* @概述  				: 会把当前电机位置设为机械零位， 会先失能电机, 再使能电机
*******************************************************************************/
void RobStride_Set_ZreoPos(void)
{
	RobStride_motor_reset();
	txCanIdEx.mode = MOTOR_ZERO;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	tx_data[0] = 1;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机设置CAN_ID （通信类型7）
* @参数         : 修改后（预设）CANID
* @返回值 			: void
* @概述  				: None
*******************************************************************************/
void RobStride_Set_CAN_ID(uint8_t Set_CAN_ID)
{
	RobStride_motor_reset();
	txCanIdEx.mode = MOTOR_ID;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = Set_CAN_ID<<8|MASTER_ID;
	txMsg.DLC = 8;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机数据保存 （通信类型22）
* @参数      		: None
* @返回值 				: void
* @概述  				: 会把当前单参数写入表中的数据写为默认值，重新上电后参数保持为此指令运行时的参数
*******************************************************************************/
void RobStride_Motor_MotorDataSave(void)
{
	txCanIdEx.mode = MOTOR_DataSave;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	tx_data[0] = 0x01;
	tx_data[1] = 0x02;
	tx_data[2] = 0x03;
	tx_data[3] = 0x04;
	tx_data[4] = 0x05;
	tx_data[5] = 0x06;
	tx_data[6] = 0x07;
	tx_data[7] = 0x08;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机波特率修改 （通信类型23）
* @参数      		: 波特率模式:	 01（1M）
									02（500K）
									03（250K）
									04（125K）
* @返回值 				: void
* @概述  				: 将电机波特率修改为对应的值，例如参数为01，波特率修改为1M
*******************************************************************************/
void RobStride_Motor_BaudRateChange(uint8_t F_CMD)
{
	txCanIdEx.mode = MOTOR_BAUD;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	tx_data[0] = 0x01;
	tx_data[1] = 0x02;
	tx_data[2] = 0x03;
	tx_data[3] = 0x04;
	tx_data[4] = 0x05;
	tx_data[5] = 0x06;
	tx_data[6] = F_CMD;
	tx_data[7] = 0x08;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机主动上报设置 （通信类型24）
* @参数      		: 上报模式：	00（关闭）
														01（开启）
* @返回值 				: void
* @概述  				: 开启/关闭 电机主动上报，默认上报周期为10ms
*******************************************************************************/
void RobStride_Motor_ProtactiveEscalationSet(uint8_t F_CMD)
{
	txCanIdEx.mode = MOTOR_Proactive;
	txCanIdEx.id = CAN_ID;
	txCanIdEx.res = 0;
	txCanIdEx.data = MASTER_ID;
	txMsg.DLC = 8;
	tx_data[0] = 0x01;
	tx_data[1] = 0x02;
	tx_data[2] = 0x03;
	tx_data[3] = 0x04;
	tx_data[4] = 0x05;
	tx_data[5] = 0x06;
	tx_data[6] = F_CMD;
	tx_data[7] = 0x08;
	can_txd();
}

/*******************************************************************************
* @功能     		: RobStride电机协议修改 （通信类型25）
* @参数      		: 协议类型：		00（私有协议）
										01（Canopen）
										02（MIT协议）
* @返回值 				: void
* @概述  				: None
*******************************************************************************/
void RobStride_Motor_MIT_ModeSet(uint8_t F_CMD)
{
	if (F_CMD == 0)//MIT协议换成私有协议切
	{
		mittxCanIdEx.id = CAN_ID;
		MITtxMsg.DLC = 8;
		tx_data[0] = 0xFF;
		tx_data[1] = 0xFF;
		tx_data[2] = 0xFF;
		tx_data[3] = 0xFF;
		tx_data[4] = 0xFF;
		tx_data[5] = 0xFF;
		tx_data[6] = F_CMD;
		tx_data[7] = 0xFD;
		can_MIT_txd();
	}
	else if(F_CMD == 2)//私有协议切换成MIT协议
	{
		txCanIdEx.mode = MOTOR_MODESTE;
		txCanIdEx.id = CAN_ID;
		txCanIdEx.res = 0;
		txCanIdEx.data = MASTER_ID;
		txMsg.DLC = 8;
		tx_data[0] = 0x01;
		tx_data[1] = 0x02;
		tx_data[2] = 0x03;
		tx_data[3] = 0x04;
		tx_data[4] = 0x05;
		tx_data[5] = 0x06;
		tx_data[6] = F_CMD;
		tx_data[7] = 0x08;
		can_txd();
	}
}

//MIT模式使能
void RobStride_Motor_MIT_enable(void)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = 0xFF;
	tx_data[7] = 0xFC;
	can_MIT_txd();
}

//MIT模式失能
void RobStride_Motor_MIT_reset(void)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = 0xFF;
	tx_data[7] = 0xFD;
	can_MIT_txd();
}

//清除错误及读取异常状态
void RobStride_Motor_MIT_ClearOrCheckError(uint8_t F_CMD)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = F_CMD;//其中 F_CMD 字节为 0xFF 时，表示消除当前的异常；为其他任何数值时，将在回复中的 BYTE1 中回传错误值
	tx_data[7] = 0xFD;
	can_MIT_txd();
}

//MIT设置运行模式
void RobStride_Motor_MIT_SetMotorType(uint8_t F_CMD)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = F_CMD;//其中 F_CMD 字节为运行模式其中 0 为 MIT 模式（默认）1 为位置模式2 为速度模式
	tx_data[7] = 0xFC;
	can_MIT_txd();
}

//MIT设置电机ID
void RobStride_Motor_MIT_SetMotorId(uint8_t F_CMD)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = F_CMD;//其中 F_CMD 字节为目标修改的电机 id
	tx_data[7] = 0xFA;
	can_MIT_txd();
}

//MIT控制模式
void RobStride_Motor_MIT_Control(float Angle, float Speed, float Kp, float Kd, float Torque)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = float_to_uint(Angle, P_MIN,P_MAX, 16)>>8;
	tx_data[1] = float_to_uint(Angle, P_MIN,P_MAX, 16);
	tx_data[2] = float_to_uint(Speed, V_MIN,V_MAX, 12)>>4;
	tx_data[3] = float_to_uint(Speed, V_MIN,V_MAX, 12)<<4 | float_to_uint(Kp, KP_MIN, KP_MAX, 12)>>8;
	tx_data[4] = float_to_uint(Kp, KP_MIN, KP_MAX, 12);
	tx_data[5] = float_to_uint(Kd, KD_MIN, KD_MAX, 12)>>4;
	tx_data[6] = float_to_uint(Kd, KD_MIN, KD_MAX, 12)<<4 | float_to_uint(Torque, T_MIN, T_MAX, 12)>>8;
	tx_data[7] = float_to_uint(Torque, T_MIN, T_MAX, 12);
	can_MIT_txd();
}

//MIT位置模式
void RobStride_Motor_MIT_PositionControl(float position_rad, float speed_rad_per_s)
{
	mittxCanIdEx.id = (1 << 8) | CAN_ID;
	MITtxMsg.DLC = 8;
	memcpy(&tx_data[0], &position_rad, 4); 	//将位置数据复制到发送数据数组中
	memcpy(&tx_data[4], &speed_rad_per_s, 4); 	//将速度数据复制到发送数据数组中
	can_MIT_txd();
}

//MIT速度模式
void RobStride_Motor_MIT_SpeedControl(float speed_rad_per_s, float current_limit)
{
	mittxCanIdEx.id = (2 << 8) | CAN_ID;
	MITtxMsg.DLC = 8;
	memcpy(&tx_data[0], &speed_rad_per_s, 4); 	//将位置数据复制到发送数据数组中
	memcpy(&tx_data[4], &current_limit, 4); 	//将速度数据复制到发送数据数组中
	can_MIT_txd();
}

//MIT零点设置
void RobStride_Motor_MIT_SetZeroPos(void)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = 0xFF;
	tx_data[7] = 0xFE;
	can_MIT_txd();
}

//MIT数据保存（需要更新到最新固件）
void RobStride_Motor_MIT_MotorDataSave(void)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = 0xFF;
	tx_data[7] = 0xF8;
	can_MIT_txd();
}

//MIT主动上报（需要更新到最新固件）
void RobStride_Motor_MIT_ProtactiveEscalationSet(uint8_t F_CMD)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = 0xFF;
	tx_data[1] = 0xFF;
	tx_data[2] = 0xFF;
	tx_data[3] = 0xFF;
	tx_data[4] = 0xFF;
	tx_data[5] = 0xFF;
	tx_data[6] = F_CMD;//其中 F_CMD 字节为电机协议类型其中 0 为不上报（默认）1 为上报
	tx_data[7] = 0xF9;
	can_MIT_txd();
}

//MIT读参数
void Get_RobStride_Motor_MIT_parameter(uint16_t Index)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = Index;
	tx_data[1] = Index>>8;
	can_MIT_txd();
}

//MIT写参数
void Set_RobStride_Motor_MIT_parameter(uint16_t Index, float Value)
{
	mittxCanIdEx.id = CAN_ID;
	MITtxMsg.DLC = 8;
	tx_data[0] = Index;
	tx_data[1] = Index>>8;
	tx_data[2] = 0x00;
	tx_data[3] = 0x00;	
	memcpy(&tx_data[4],&Value,4);	
  can_MIT_txd();
}

/*******************************************************************************
* @功能     	: 接收处理函数		（通信类型2 17应答帧 0应答帧）
* @参数1        : 
* @返回值 		: None
* @概述  		: drw只有通过通信17发送以后才有值
*******************************************************************************/
void parse_motor_feedback(void)
{
	if (rxMsg.ExtId != 0)
	{
		uint8_t motor_id = rxCanIdEx.data & 0xFF;

		mf.uncalibrated        = ((rxCanIdEx.data >> 13) & 0x01) ? 1 : 0;
		mf.StallOverloadFault  = ((rxCanIdEx.data >> 12) & 0x01) ? 1 : 0;
		mf.MagneticEncoderFault= ((rxCanIdEx.data >> 11) & 0x01) ? 1 : 0;
		mf.OverTemperatureFault= ((rxCanIdEx.data >> 10) & 0x01) ? 1 : 0;
		mf.DriveFault          = ((rxCanIdEx.data >> 9)  & 0x01) ? 1 : 0;
		mf.UnderVoltageFault   = ((rxCanIdEx.data >> 8)  & 0x01) ? 1 : 0;

		uint8_t mode_tmp = (uint8_t)((rxCanIdEx.data >> 14) & 0x03);
		if (mode_tmp > running) mode_tmp = rest;   // 范围保护
		mf.mms = (enum ModeStatus)mode_tmp;

		uint16_t angle_raw  = ((uint16_t)rx_data[0] << 8) | rx_data[1];
		uint16_t speed_raw  = ((uint16_t)rx_data[2] << 8) | rx_data[3];
		uint16_t torque_raw = ((uint16_t)rx_data[4] << 8) | rx_data[5];
		uint16_t temp_raw   = ((uint16_t)rx_data[6] << 8) | rx_data[7];

		txCanIdEx.id=motor_id;
		txCanIdEx.data=rxCanIdEx.id;

		mf.angle       = uint16_to_float(angle_raw, P_MIN, P_MAX);
		mf.speed       = uint16_to_float(speed_raw, V_MIN, V_MAX);
		mf.torque      = uint16_to_float(torque_raw, T_MIN, T_MAX);
		mf.temperature = temp_raw / 10.0f;
	}
	else if(rxMsg.StdId !=0)
	{
		uint16_t angle_raw  = ((uint16_t)rx_data[1] << 8) | rx_data[2];
		uint16_t speed_raw  = ((uint16_t)rx_data[3] << 8) | rx_data[4];
		uint16_t torque_raw = ((uint16_t)(rx_data[4]  << 4) << 8) | rx_data[5];
		uint16_t temp_raw   = ((uint16_t)rx_data[6] << 8) | rx_data[7];
		
		mf.angle       = uint16_to_float(angle_raw, P_MIN, P_MAX);
		mf.speed       = uint16_to_float(speed_raw, V_MIN, V_MAX);
		mf.torque      = uint16_to_float(torque_raw, T_MIN, T_MAX);
		mf.temperature = temp_raw / 10.0f;
	}
	
}


