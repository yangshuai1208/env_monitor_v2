# STM32 串口空闲中断接收不定长数据

## 功能
- 使用串口空闲中断（IDLE）接收任意长度的数据
- 接收完成后通过串口回显收到的数据
- 基于 HAL 库的 `HAL_UARTEx_ReceiveToIdle_IT` 和回调 `HAL_UARTEx_RxEventCallback`

## 硬件连接
- MCU: STM32F103C8T6
- 串口: USART1 (PA9 TX, PA10 RX)
- 波特率: 115200, 8N1

## 使用方法
1. 用 Keil 5 打开 `MDK-ARM/串口接收之空闲中断接收不定长数据.uvprojx`
2. 编译并下载到开发板
3. 打开串口助手（115200, 8N1），发送任意字符串（如 `Hello STM32`）
4. 开发板会回显相同的内容，并在串口助手中打印“已经接收到了不定长数据”

## 代码要点
- 初始化后调用 `HAL_UARTEx_ReceiveToIdle_IT(&huart1, RXBuff, LENGTH)` 使能空闲中断
- 当串口接收空闲时，自动触发 `HAL_UARTEx_RxEventCallback`
- 回调中获取实际接收长度 `Size`，并回显数据
- 重新使能空闲中断，准备下一次接收

## 注意事项
- 空闲中断要求串口必须有一小段空闲时间（约1个字节的传输时间）
- 接收缓冲区 `RXBuff` 大小需足够大（代码中定义为 64）
- 该功能需要 HAL 库支持 `HAL_UARTEx_ReceiveToIdle_IT`，STM32CubeMX 生成代码时需勾选 “UART Ex” 相关选项
