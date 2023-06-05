#include "uart.h"
#include "timer.h"

void spi_init(void){
    cli();
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc;
    PORTC.DIR |= (PIN0_bm | PIN2_bm);   
    PORTA.OUTSET = PIN1_bm;     // DISP_LATCH initiall high
    PORTA.DIRSET = PIN1_bm; 

    SPI0.CTRLA = SPI_MASTER_bm;
    SPI0.CTRLB = SPI_SSD_bm;
    SPI0.INTCTRL = SPI_IE_bm;
    SPI0.CTRLA |= SPI_ENABLE_bm;
    sei();
}

void spi_write(uint8_t b){
    SPI0.DATA = b;
}

ISR(SPI0_INT_vect){
    //rising edge on DISP_LATCH
    PORTA.OUTCLR = PIN1_bm;
    PORTA.OUTSET = PIN1_bm;  
    SPI0.INTFLAGS = SPI_IF_bm;
}