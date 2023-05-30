#include "headers.h"

volatile uint8_t pb_debounced = 0xFF;
// volatile pb_debounced = 0xFF;

void buttons_init() {
    // Enable pull-up resistors for PBs
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    // Configure timer for PB sampling
    cli();
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;    // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 33333;                  // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;        // Enable
    sei(); 
}

void pb_debounce(void) {
    static uint8_t vcount0=0, vcount1=0;   //vertical counter bits
     
    uint8_t pb_sample = PORTA.IN;

    uint8_t pb_changed = (pb_sample ^ pb_debounced);

    //vertical counter update
    vcount1 = (vcount1 ^ vcount0) & pb_changed;  //vcount1 (bit 1 of vertical counter)
    vcount0 = ~vcount0 & pb_changed;             //vcount0 (bit 0 of vertical counter)

    pb_debounced ^= (vcount0 & vcount1);         //toggle pb_debounced
}