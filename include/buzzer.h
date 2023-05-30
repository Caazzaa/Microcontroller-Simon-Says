#include <stdint.h>

void pwm_init();
void seqTone(uint8_t step);
void seqToneStop();
void toneIncrement();
void toneDecrement();
void toneReset();