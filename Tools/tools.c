#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"
#include <string.h>

void delay_us(uint32_t xus){
	
	SysTick->LOAD = 72 * xus;
	SysTick->VAL = 0x00;
	SysTick->CTRL = 0x00000005;
	while(!(SysTick->CTRL & 0x00010000));
	SysTick->CTRL = 0x00000004;
}
void delay_ms(uint32_t xms){
	while(xms--){
		delay_us(1000);
	}
}
void delay_s(uint32_t xs){
	while(xs--){
		delay_ms(1000);
	}
}


#define TX_BUF_LEN  256     /* 发送缓冲区容量，根据需要进行调整 */
uint8_t TxBuf[TX_BUF_LEN];  /* 发送缓冲区                       */
void MyPrintf(const char *__format, ...)
{
  va_list ap;
  va_start(ap, __format);
    
  /* 清空发送缓冲区 */
  memset(TxBuf, 0x0, TX_BUF_LEN);
    
  /* 填充发送缓冲区 */
  vsnprintf((char*)TxBuf, TX_BUF_LEN, (const char *)__format, ap);
  va_end(ap);
  int len = strlen((const char*)TxBuf);
  
  /* 往串口发送数据 */
  for (int i = 0; i < len; i++)
  {
 while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);    
 USART_SendData(USART1, TxBuf[i]);
  }
}



