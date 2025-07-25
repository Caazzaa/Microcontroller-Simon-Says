#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>

typedef enum
{
    seqBegin,
    seqCheck,
    seqSuccess,
    seqFail,
    seqName,
    seqSetName
} seqState;

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

struct userScore {
    char name[20];
    uint32_t score;
} board[5];

extern volatile uint16_t elapsed_time;
extern volatile uint16_t playback_time ;
extern volatile uint16_t new_playback_time;
extern volatile uint8_t allow_updating_playback_delay;