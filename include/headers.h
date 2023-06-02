#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "sequence.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "buttons.h"
#include "buzzer.h"
#include "potentiometer.h"

typedef enum
{
    AWAITING_COMMAND,
    AWAITING_PAYLOAD,
    AWAITING_NAME
} Serial_State;

typedef enum
{
    ACK,
    NACK,
    WAITING
} Command_Response;

typedef enum
{
    Pause,
    Wait,
    button1,
    button2,
    button3,
    button4,
    Success,
    Fail,
    Restart
} gameState;

extern volatile uint16_t elapsed_time;
extern volatile uint16_t playback_time ;
extern volatile uint16_t new_playback_time;
extern volatile uint8_t allow_updating_playback_delay;