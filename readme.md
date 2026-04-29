RobStride 电机控制库使用说明 / RobStride Motor Control Library README

 一、项目简介 Project Introduction

本工程为灵足时代RobStride 电机 STM32 控制例程，核心为 `robstride_control.h`/`robstride_control.c` 电机类库，支持灵足私有协议与MIT协议两种控制方式，实现了电机各主流控制模式（使能、失能、位置/速度/电流/运控/MIT模式、参数设置、零点、协议切换等）。

 This project is a STM32 control demo for RS01/RobStride motors from Lingzu Era. The core is the `robstride_control.h`/`robstride_control.c` motor library, supporting both Lingzu private and MIT protocols, implementing enable/disable, position/speed/current/torque/MIT control, parameter settings, zeroing, and protocol switching.

---

二、使用说明 Getting Started

硬件平台 Hardware Board： STM32F105RBT6 开发板 
 `robstride_control.h`, `robstride_control.c` 添加到项目
需包含 `main.h`, `can.h`, `gpio.h`
CAN总线已初始化，波特率与电机设置一致

 1. 主循环用法 Main Loop Usage

核心主循环推荐如下结构（建议通过变量`mode`实现控制场景切换，每个`case`对应一种典型功能，具体见 main.c 示例）：

```c
uint8_t mode = 0; // 外部赋值，决定当前控制功能

 while (1)
  {
	switch (mode) 
	{
	case 0:RobStride_motor_enable();break;
	case 1:RobStride_motor_reset();break;
	case 2:RobStrideMotor_move_control(1, 0, 1, 0.1, 0.1);break;
	case 3RobStride_Motor_Pos_PP_control(6, 1, 2);break;
	case 4:RobStride_Motor_Pos_CSP_control(4,1);break;
	case 5:RobStride_Motor_Speed_control(1, 1, 2);break;
        // ... 其他case见main.c
        // MIT协议专用接口见后面注意事项
        default: break;
    }
    HAL_Delay(50);
}
```

2. 宏定义对应关系 Macro Definition Mappings

不同电机型号对应不同的系统参数（用过修改宏定义`MOTOR_TYPE`实现电机型号的切换，具体见robstride_control.h）
```h
#define MOTOR_TYPE 0
#define P_MIN -12.57f
#define P_MAX 12.57f
#if MOTOR_TYPE == 0
        #define V_MIN -33.0f
        #define V_MAX 33.0f
        #define KP_MIN 0.0f
        #define KP_MAX 500.0f
        #define KD_MIN 0.0f
        #define KD_MAX 5.0f
        #define T_MIN -14.0f
        #define T_MAX 14.0f
#elif MOTOR_TYPE == 1
        #define V_MIN -44.0f 
        #define V_MAX 44.0f
        #define KP_MIN 0.0f
        #define KP_MAX 500.0f
        #define KD_MIN 0.0f
        #define KD_MAX 5.0f
        #define T_MIN -17.0f
        #define T_MAX 17.0f

 三、常见问题 FAQ

Q：PP位置模式电机使能但不转？
  A：务必调用 `RobStride_Motor_Pos_control(speed, angle)` 前设置合适的目标速度（如 2\~5 rad/s），并检查速度参数是否通过 `0x7018` 写入。延时设置亦需充分。
Q：MIT接口下ID发错？
  A：请保证 CAN\_ID 不高于 0x7F，若出现 0x47F 这类问题请检查 ID 赋值与掩码（详见代码实现）。
Q：电机无响应？
  A：请检查 CAN 硬件连通性、波特率、电源，及主控和电机 ID/协议是否一致。

---

 四、参考资料 Reference

- 《RS01电机通信协议说明书》
- 本库源码及 main.c 示例



-----------------------------------------------------------------------------------------


RobStride Motor Control Library README

1. Project Introduction

 This project is a STM32 control demo for RS01/RobStride motors from Lingzu Era. The core is the `robstride_control.h`/`robstride_control.c` motor library, supporting both Lingzu private and MIT protocols, implementing enable/disable, position/speed/current/torque/MIT control, parameter settings, zeroing, and protocol switching.

---

2.Getting Started

 Hardware Board： STM32F105RBT6 Development Board
 `robstride_control.h`, `robstride_control.c` Add to Project
Must include `main.h`, `can.h`, `gpio.h`
The CAN bus has been initialized; the baud rate matches the motor settings.

2. 1. Main Loop Usage Main Loop Usage

The following structure is recommended for the core main loop (it is suggested that scene switching be controlled via the `mode` variable, with each `case` corresponding to a specific typical function; for details, please refer to the `main.c` example):

```c
uint8_t mode = 0; // External assignment determines the current control function.

 while (1)
  {
	switch (mode) 
	{
	case 0:RobStride_motor_enable();break;
	case 1:RobStride_motor_reset();break;
	case 2:RobStrideMotor_move_control(1, 0, 1, 0.1, 0.1);break;
	case 3RobStride_Motor_Pos_PP_control(6, 1, 2);break;
	case 4:RobStride_Motor_Pos_CSP_control(4,1);break;
	case 5:RobStride_Motor_Speed_control(1, 1, 2);break;
        // ... See main.c for other cases.
	// See the "Notes" section below for interfaces specific to the MIT License.
        default: break;
    }
    HAL_Delay(50);
}
```

2.2. Macro Definition Mappings Macro Definition Mappings

Different motor models correspond to different system parameters (switching between motor models is achieved by modifying the `MOTOR_TYPE` macro definition; for details, please refer to `robstride_control.h`).
```h
#define MOTOR_TYPE 0
#define P_MIN -12.57f
#define P_MAX 12.57f
#if MOTOR_TYPE == 0
        #define V_MIN -33.0f
        #define V_MAX 33.0f
        #define KP_MIN 0.0f
        #define KP_MAX 500.0f
        #define KD_MIN 0.0f
        #define KD_MAX 5.0f
        #define T_MIN -14.0f
        #define T_MAX 14.0f
#elif MOTOR_TYPE == 1
        #define V_MIN -44.0f 
        #define V_MAX 44.0f
        #define KP_MIN 0.0f
        #define KP_MAX 500.0f
        #define KD_MIN 0.0f
        #define KD_MAX 5.0f
        #define T_MIN -17.0f
        #define T_MAX 17.0f

3.Frequently Asked Questions FAQ

Q：Is the motor enabled in PP (Position) mode but not rotating?
  A：Must be called `RobStride_Motor_Pos_control(speed, angle)` First, set an appropriate target velocity (e.g., 2–5 rad/s), and verify that the velocity parameter has been written via `0x7018`. The delay settings must also be sufficiently configured.。
Q：Was the ID sent incorrectly via the MIT interface?
  A：Please ensure that the CAN_ID does not exceed 0x7F. If issues such as 0x47F arise, please verify the ID assignment and the mask (refer to the code implementation for details).
Q：Motor Unresponsive？
  A：Please check the CAN hardware connectivity, baud rate, and power supply, as well as whether the IDs and protocols of the main controller and the motor are consistent.

---

4. References

- *RS01 Motor Communication Protocol Specification*
- Source code for this library and the `main.c` example
