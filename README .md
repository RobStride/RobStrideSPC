# RobStride 电机控制库 / RobStride Motor Control Library

## 一、项目概述 / Overview

本项目为灵足时代（Lingzu Era）RobStride 系列电机的 STM32 控制参考实现。核心库文件 `robstride_control.h` / `robstride_control.c` 提供了完整的电机控制接口，支持灵足私有通信协议与 MIT 协议两种模式，涵盖电机使能/失能、位置控制、速度控制、电流控制、力矩控制、MIT 控制、参数配置、零点校准及协议切换等主流功能。

This project provides an STM32 control reference implementation for the RobStride series motors from Lingzu Era. The core library files `robstride_control.h` / `robstride_control.c` offer a complete set of motor control interfaces, supporting both the Lingzu proprietary protocol and the MIT protocol. The library covers motor enable/disable, position control, speed control, current control, torque control, MIT control, parameter configuration, zero calibration, and protocol switching.

---

## 二、环境要求 / Requirements

| 项目 / Item | 要求 / Requirement |
|---|---|
| 硬件平台 / Hardware | STM32F105RBT6 开发板 / Development Board |
| 依赖文件 / Dependencies | `main.h`, `can.h`, `gpio.h` |
| 通信接口 / Interface | CAN 总线，波特率需与电机配置一致 / CAN bus, baud rate must match motor settings |
| 库文件 / Library Files | `robstride_control.h`, `robstride_control.c` |

> **注意 / Note**：使用前请确保 CAN 总线已完成初始化，且波特率与电机端配置保持一致。  
> Ensure the CAN bus is initialized and the baud rate matches the motor configuration before use.

---

## 三、快速开始 / Quick Start

### 3.1 主循环结构 / Main Loop Structure

推荐采用以下主循环结构，通过 `mode` 变量实现不同控制场景的切换：

```c
uint8_t mode = 0;  // 外部赋值，用于切换控制模式 / External variable for mode switching

while (1)
{
    switch (mode)
    {
        case 0:  RobStride_motor_enable();                          break;  // 电机使能 / Enable motor
        case 1:  RobStride_motor_reset();                           break;  // 电机复位 / Reset motor
        case 2:  RobStrideMotor_move_control(1, 0, 1, 0.1, 0.1);  break;  // 运控模式 / Motion control
        case 3:  RobStride_Motor_Pos_PP_control(6, 1, 2);           break;  // PP 位置模式 / PP position mode
        case 4:  RobStride_Motor_Pos_CSP_control(4, 1);             break;  // CSP 位置模式 / CSP position mode
        case 5:  RobStride_Motor_Speed_control(1, 1, 2);            break;  // 速度模式 / Speed mode
        // 更多模式详见 main.c 示例 / Additional modes refer to main.c example
        // MIT 协议专用接口参见注意事项 / MIT protocol interfaces see Notes section
        default: break;
    }
    HAL_Delay(50);
}
```

### 3.2 电机型号配置 / Motor Model Configuration

通过修改 `robstride_control.h` 中的 `MOTOR_TYPE` 宏定义，可切换不同电机型号的系统参数：

```c
#define MOTOR_TYPE 0    // 0: 默认型号 / Default model; 1: 扩展型号 / Extended model

#define P_MIN -12.57f
#define P_MAX  12.57f

#if MOTOR_TYPE == 0
    #define V_MIN  -33.0f
    #define V_MAX   33.0f
    #define KP_MIN   0.0f
    #define KP_MAX 500.0f
    #define KD_MIN   0.0f
    #define KD_MAX   5.0f
    #define T_MIN  -14.0f
    #define T_MAX   14.0f

#elif MOTOR_TYPE == 1
    #define V_MIN  -44.0f
    #define V_MAX   44.0f
    #define KP_MIN   0.0f
    #define KP_MAX 500.0f
    #define KD_MIN   0.0f
    #define KD_MAX   5.0f
    #define T_MIN  -17.0f
    #define T_MAX   17.0f
#endif
```

> **参数说明 / Parameter Description**：
> - `P_MIN` / `P_MAX`: 位置限制 / Position limits (rad)
> - `V_MIN` / `V_MAX`: 速度限制 / Velocity limits (rad/s)
> - `KP_MIN` / `KP_MAX`: 位置环比例增益范围 / Position loop proportional gain range
> - `KD_MIN` / `KD_MAX`: 位置环微分增益范围 / Position loop derivative gain range
> - `T_MIN` / `T_MAX`: 力矩限制 / Torque limits (N·m)

---

## 四、API 接口说明 / API Reference

| 函数 / Function | 功能 / Description |
|---|---|
| `RobStride_motor_enable()` | 电机使能 / Enable motor |
| `RobStride_motor_reset()` | 电机复位 / Reset motor |
| `RobStrideMotor_move_control(...)` | 运控模式（位置+速度+力矩混合）/ Motion control mode |
| `RobStride_Motor_Pos_PP_control(...)` | PP 位置模式 / Profile Position mode |
| `RobStride_Motor_Pos_CSP_control(...)` | CSP 周期同步位置模式 / Cyclic Synchronous Position mode |
| `RobStride_Motor_Speed_control(...)` | 速度模式 / Speed control mode |
| `RobStride_Motor_Current_control(...)` | 电流模式 / Current control mode |
| `RobStride_Motor_MIT_control(...)` | MIT 协议控制模式 / MIT protocol control mode |

> 完整接口定义及参数说明请参见 `robstride_control.h` 头文件。  
> For complete interface definitions and parameter descriptions, refer to the `robstride_control.h` header file.

---

## 五、常见问题 / Troubleshooting

### Q1: PP 位置模式下电机使能但不转动？

**A:** 调用 `RobStride_Motor_Pos_control(speed, angle)` 前，请确保已设置合适的目标速度（建议 2~5 rad/s），并验证速度参数是否通过寄存器 `0x7018` 正确写入。同时确保延时配置充分。

**Motor is enabled in PP mode but does not rotate?**  
Ensure an appropriate target velocity is set (recommended 2–5 rad/s) before calling `RobStride_Motor_Pos_control()`, and verify the velocity parameter is correctly written via register `0x7018`. Delay settings must also be sufficiently configured.

---

### Q2: MIT 协议下 CAN ID 发送异常？

**A:** 请确保 CAN_ID 不超过 `0x7F`。若出现 `0x47F` 等异常值，请检查 ID 赋值逻辑及掩码处理（详见代码实现）。

**Incorrect ID sent via MIT protocol?**  
Ensure the CAN_ID does not exceed `0x7F`. If abnormal values such as `0x47F` appear, verify the ID assignment logic and mask handling (refer to the code implementation for details).

---

### Q3: 电机无响应？

**A:** 请依次排查以下事项：

1. CAN 硬件连接是否正常 / Check CAN hardware connectivity
2. 波特率是否一致 / Verify baud rate consistency
3. 电源供应是否正常 / Check power supply
4. 主控与电机的 ID 及协议配置是否匹配 / Verify ID and protocol matching between controller and motor

---

## 六、参考资料 / References

- 《RS01 电机通信协议说明书》/ *RS01 Motor Communication Protocol Specification*
- 本库源码及 `main.c` 示例 / Source code of this library and the `main.c` example

---

## 七、许可与声明 / License & Disclaimer

本项目为灵足时代官方提供的 STM32 控制参考实现，仅供学习与技术交流使用。使用前请仔细阅读电机官方文档，确保操作符合安全规范。

This project is an official STM32 control reference implementation provided by Lingzu Era for educational and technical exchange purposes. Please read the official motor documentation carefully and ensure operations comply with safety regulations.

---

---

---

# English Version

## 1. Overview

This project provides an STM32 control reference implementation for the RobStride series motors from Lingzu Era. The core library files `robstride_control.h` / `robstride_control.c` offer a complete set of motor control interfaces, supporting both the Lingzu proprietary protocol and the MIT protocol. The library covers motor enable/disable, position control, speed control, current control, torque control, MIT control, parameter configuration, zero calibration, and protocol switching.

---

## 2. Requirements

| Item | Requirement |
|---|---|
| Hardware | STM32F105RBT6 Development Board |
| Dependencies | `main.h`, `can.h`, `gpio.h` |
| Interface | CAN bus, baud rate must match motor settings |
| Library Files | `robstride_control.h`, `robstride_control.c` |

> **Note**: Ensure the CAN bus is initialized and the baud rate matches the motor configuration before use.

---

## 3. Quick Start

### 3.1 Main Loop Structure

The following structure is recommended for the core main loop. Scene switching is controlled via the `mode` variable, with each `case` corresponding to a specific typical function.

```c
uint8_t mode = 0;  // External variable for mode switching

while (1)
{
    switch (mode)
    {
        case 0:  RobStride_motor_enable();                          break;  // Enable motor
        case 1:  RobStride_motor_reset();                           break;  // Reset motor
        case 2:  RobStrideMotor_move_control(1, 0, 1, 0.1, 0.1);  break;  // Motion control
        case 3:  RobStride_Motor_Pos_PP_control(6, 1, 2);           break;  // PP position mode
        case 4:  RobStride_Motor_Pos_CSP_control(4, 1);             break;  // CSP position mode
        case 5:  RobStride_Motor_Speed_control(1, 1, 2);            break;  // Speed mode
        // Additional modes refer to main.c example
        // MIT protocol interfaces see Notes section
        default: break;
    }
    HAL_Delay(50);
}
```

### 3.2 Motor Model Configuration

Switching between motor models is achieved by modifying the `MOTOR_TYPE` macro definition in `robstride_control.h`.

```c
#define MOTOR_TYPE 0    // 0: Default model; 1: Extended model

#define P_MIN -12.57f
#define P_MAX  12.57f

#if MOTOR_TYPE == 0
    #define V_MIN  -33.0f
    #define V_MAX   33.0f
    #define KP_MIN   0.0f
    #define KP_MAX 500.0f
    #define KD_MIN   0.0f
    #define KD_MAX   5.0f
    #define T_MIN  -14.0f
    #define T_MAX   14.0f

#elif MOTOR_TYPE == 1
    #define V_MIN  -44.0f
    #define V_MAX   44.0f
    #define KP_MIN   0.0f
    #define KP_MAX 500.0f
    #define KD_MIN   0.0f
    #define KD_MAX   5.0f
    #define T_MIN  -17.0f
    #define T_MAX   17.0f
#endif
```

> **Parameter Description**:
> - `P_MIN` / `P_MAX`: Position limits (rad)
> - `V_MIN` / `V_MAX`: Velocity limits (rad/s)
> - `KP_MIN` / `KP_MAX`: Position loop proportional gain range
> - `KD_MIN` / `KD_MAX`: Position loop derivative gain range
> - `T_MIN` / `T_MAX`: Torque limits (N·m)

---

## 4. API Reference

| Function | Description |
|---|---|
| `RobStride_motor_enable()` | Enable motor |
| `RobStride_motor_reset()` | Reset motor |
| `RobStrideMotor_move_control(...)` | Motion control mode (position + speed + torque) |
| `RobStride_Motor_Pos_PP_control(...)` | Profile Position mode |
| `RobStride_Motor_Pos_CSP_control(...)` | Cyclic Synchronous Position mode |
| `RobStride_Motor_Speed_control(...)` | Speed control mode |
| `RobStride_Motor_Current_control(...)` | Current control mode |
| `RobStride_Motor_MIT_control(...)` | MIT protocol control mode |

> For complete interface definitions and parameter descriptions, refer to the `robstride_control.h` header file.

---

## 5. Troubleshooting

### Q1: Motor is enabled in PP mode but does not rotate?

**A:** Ensure an appropriate target velocity is set (recommended 2–5 rad/s) before calling `RobStride_Motor_Pos_control()`, and verify the velocity parameter is correctly written via register `0x7018`. Delay settings must also be sufficiently configured.

---

### Q2: Incorrect ID sent via MIT protocol?

**A:** Ensure the CAN_ID does not exceed `0x7F`. If abnormal values such as `0x47F` appear, verify the ID assignment logic and mask handling (refer to the code implementation for details).

---

### Q3: Motor unresponsive?

**A:** Please check the following in order:

1. Check CAN hardware connectivity
2. Verify baud rate consistency
3. Check power supply
4. Verify ID and protocol matching between controller and motor

---

## 6. References

- *RS01 Motor Communication Protocol Specification*
- Source code of this library and the `main.c` example

---

## 7. License & Disclaimer

This project is an official STM32 control reference implementation provided by Lingzu Era for educational and technical exchange purposes. Please read the official motor documentation carefully and ensure operations comply with safety regulations.
