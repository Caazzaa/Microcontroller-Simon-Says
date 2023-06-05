#include "headers.h"
#include "timer.h"
#include "spi.h"

extern uint8_t digit1, digit2;
extern uint8_t segs[];
volatile uint16_t elapsed_time = 0;
volatile uint16_t playback_time = 500;
volatile uint16_t new_playback_time = 500;
volatile uint8_t allow_updating_playback_delay = 0;

#define ADC8bit 6.8359375

void pb_debounce(void);

void timer_init(void)
{
    cli();

    //    TCB0.CTRLA = TCB_CLKSEL_DIV2_gc;    // Configure CLK_PER/2
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 33333;               // Set interval for 10ms (33333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;      // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;      // Enable
    TCB1.CTRLB = TCB_CNTMODE_INT_gc;
    TCB1.CCMP = 3333;     
    TCB1.INTCTRL = TCB_CAPT_bm;      // CAPT interrupt enable
    TCB1.CTRLA = TCB_ENABLE_bm;      // Enable

    sei();
}

ISR(TCB1_INT_vect)
{
    elapsed_time++;

    if (allow_updating_playback_delay)
    {
        playback_time = durationPOT();
        allow_updating_playback_delay = 0;
    }

    TCB1.INTFLAGS = TCB_CAPT_bm;
}

void delay(uint16_t time){
    elapsed_time = 0;
    while(elapsed_time < time){}
    return;
}

uint16_t durationPOT(){
    uint32_t result = ADC0.RESULT;
    uint16_t duration = (result * ADC8bit) + ((int16_t)(result * ADC8bit) >> 8) + 250;
    return duration;
}

ISR(TCB0_INT_vect)
{
    pb_debounce();
    static uint8_t digit = 0;

    if (digit)
        spi_write(segs[0] | (0x01 << 7));
    else
        spi_write(segs[1]);
    digit = !digit;

    TCB0.INTFLAGS = TCB_CAPT_bm;
}