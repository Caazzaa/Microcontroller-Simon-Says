#include "headers.h"
#include "sequence.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "buttons.h"
#include "buzzer.h"
#include "potentiometer.h"

extern volatile gameState pb_state;
extern volatile Serial_State SERIAL_STATE;
extern volatile uint32_t new_number;
volatile seqState state = seqBegin;
extern volatile uint8_t chars_received;
volatile char name[20];

int main(void)
{
    spi_init();
    timer_init();
    uart_init();
    buttons_init();
    pwm_init();
    pb_debounce();
    adc_init();
    durationPOT();
    uint16_t len = 1;

    PORTB.OUTSET |= PIN5_bm;
    PORTB.OUTCLR |= PIN5_bm;
    PORTB.DIRSET |= PIN5_bm;

    while (1)
    {
        switch (state)
        {
        case seqBegin:
            pb_state = Wait;
            seqStart(len); //runs function in sequence.c that starts the program
            state = seqCheck; //sets state to check
            break;
        case seqCheck:
            if (seqRun(len) == 0) //in seqRun a 0 is returned if the button press is wrong resulting in state being set to seqFail
            {
                state = seqFail;
            }
            else //else a 1 is returned changing state to seqSuccess
            {
                state = seqSuccess;
            }
            break;
        case seqSuccess:
            len++; //increase length by 1 so that there is one more step in the sequence
            pb_state = Wait;
            state = seqBegin; //go back to seqBegin
            break;
        case seqFail:
            for (uint8_t i = 0; i < 5; i++) //loop though length of scoreboard
            {
                if (len > board[i].score) //if length is greater than the score enter name and put it on the scoreboard
                {
                    printf("Enter name: ");
                    SERIAL_STATE = AWAITING_NAME;
                    state = seqName; //after name is entered set state to seqName
                    break;
                }
            }
            if (state != seqName) //if state is not set to seqName (caused by not getting a high enough score) print scorebaord
            {
                printScoreboard();
                printf("Restarting Game...\n===================\n");
                state = seqBegin;
            }
            break;
        case seqName:
            if (elapsed_time > 5000) //if nothing is inputted in 5 seconds set what has been entered as the name
            {
                SERIAL_STATE = AWAITING_COMMAND;
                state = seqSetName;
            }
            break;
        case seqSetName:
            name[chars_received] = '\0'; //set last character to NUL terminator
            scoreboard(len); //sort the scorebaord depending on score (len)
            printScoreboard(); //print the scorebaord
            printf("Restarting Game...\n===================\n");
            chars_received = 0; //set chars recieved back to 0
            len = 1; //set len back to 1 because of fail
            state = seqBegin;
            break;
        default:
            break;
        }
    }
}

void scoreboard(uint16_t len)
{
    for (int i = 0; i < 5; i++)
    {
        if (len > board[i].score)
        {
            for (int j = 4; j > i; j--)
            {
                board[j].score = board[j - 1].score;
            }
            for (uint8_t j = 0; j < 20; j++)
            {
                board[i].name[j] = name[j];
            }
            board[i].score = len;
            break;
        }
    }
}

void printScoreboard()
{
    for (int i = 0; i < 5; i++)
    {
        if (board[i].score == 0)
        {
            break;
        }
        printf("%s %d\n", board[i].name, board[i].score);
    }
}