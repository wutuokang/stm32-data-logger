#ifndef __LED_H
#define __LED_H

void led_Init(void);
//因为项目限制这个函数只能开启pin4、5、6、7(i范围0~3)
void ON_led_PA_1234(uint8_t i);
//因为项目限制这个函数只能关闭pin4、5、6、7(i范围0~3)
void OFF_led_PA_1234(uint8_t i);

#endif
