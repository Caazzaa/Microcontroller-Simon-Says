#include <avr/io.h>
#include <avr/interrupt.h>

void timer_init();
void pb_debounce(void);
void delay();
uint16_t durationPOT();