# Day05 学习笔记：定时器 + LED + 串口整合

## 今天完成了什么
今天把前几天学到的 GPIO、定时器中断、串口输出整合到了一个工程里，实现了：
- TIM1 周期中断
- LED_R 按周期翻转
- USART2 通过 `printf` 输出系统运行信息
- 按键 KEY1 切换闪烁速度

## 今天的核心思路
这次没有在定时器中断里直接处理所有功能，而是用了“中断置标志位，主循环处理任务”的方式：
1. 定时器中断到来
2. 在回调函数里把 `timer_flag` 置 1，同时 `timer_count++`
3. 主循环检测到 `timer_flag`
4. 在主循环里翻转 LED，并通过串口打印日志

这种写法比在中断里直接做大量事情更规范，也更适合后面继续学习 FreeRTOS。

## 关键知识点
### 1. 定时器中断只负责发通知
今天在 `HAL_TIM_PeriodElapsedCallback()` 中只做了两件事：
- `timer_flag = 1`
- `timer_count++`

这样可以避免在中断里做太多事情。

### 2. 标志位思想
通过 `timer_flag` 把中断事件传递给主循环处理，这是后面任务调度、队列通信思路的基础。

### 3. printf 重定向
通过重写 `fputc()`，把 `printf` 输出重定向到 USART2：

```c
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
