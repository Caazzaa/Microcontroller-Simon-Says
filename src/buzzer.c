#include "headers.h"
#include "sequence.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "buttons.h"
#include "potentiometer.h"

int i;
const uint32_t toneOG[] = {
    10229, 12164, 7663, 20457}; //keeps original tones to use for reset
uint32_t tone[] = {
    10229, 12164, 7663, 20457};
volatile int octave = 0;

void pwm_init()
{
    cli();
    PORTB.DIRSET |= PIN0_bm;
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm;
    sei();
}

void seqTone(uint8_t step)
{
    TCA0.SINGLE.PERBUF = tone[step]; //sets the frequency at wich the buzzer will play at
    TCA0.SINGLE.CMP0BUF = tone[step] >> 1; //sets to 50% duty cycle
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm; //enables the buzzer turning it on
}

void seqToneStop()
{
    TCA0.SINGLE.CMP0BUF = 0; //stops tone from playing
}

void toneIncrement()
{
    if (octave < 2)
    {
        octave++;
        for (i = 0; i < 4; i++) //loops throug each tone in the list
        {
            tone[i] >>= 1; //bitshifts right each tone by one
            if (tone[i] > 55000){ //if the tone is above 55000 (human hearing) cap it at 55000
                tone[i] = 55000;
            }
        }
    }
}

void toneDecrement()
{
    if (octave > -2)
    {
        octave--;
        for (i = 0; i < 4; i++)
        {
            tone[i] <<= 1; //bitshifts left each tone by one
        }
    }
}

void toneReset()
{
    octave = 0;
    for (i = 0; i < 4; i++)
    {
        tone[i] = toneOG[i]; //resets tone back to default
    }
}