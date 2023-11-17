#ifndef __LED_H__
#define __LED_H__
void LED_Init(void);
void LED_TASK(void);
void Led_Control(uint8_t index, uint8_t status, uint8_t flag);
#endif
