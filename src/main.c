#include "headers.h"

extern volatile gameState pb_state;
extern volatile Serial_State SERIAL_STATE;
extern volatile uint32_t new_number;
volatile seqState state = seqBegin;
extern volatile uint8_t chars_received;
volatile char name[20];

// uint32_t score[] = {
//     NULL, NULL, NULL, NULL, NULL};

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
            seqStart(len);
            state = seqCheck;
            break;
        case seqCheck:
            if (seqRun(len) == 0)
            {
                state = seqFail;
            }
            else
            {
                state = seqSuccess;
            }
            break;
        case seqSuccess:
            len++;
            pb_state = Wait;
            state = seqBegin;
            break;
        case seqFail:
            for (uint8_t i = 0; i < 5; i++)
            {
                if (len > board[i].score)
                {
                    printf("Enter name: ");
                    SERIAL_STATE = AWAITING_NAME;
                    state = seqName;
                    break;
                }
                // if (len <= board[i].score)
                // {
                //     printScoreboard();
                //     printf("Restarting Game...\n===================\n");
                //     state = seqBegin;
                //     break;
                // }
            }
            if (state != seqName)
            {
                printScoreboard();
                printf("Restarting Game...\n===================\n");
                state = seqBegin;
            }
            break;
        case seqName:
            if (elapsed_time > 5000)
            {
                SERIAL_STATE = AWAITING_COMMAND;
                state = seqSetName;
            }
            break;
        case seqSetName:
            name[chars_received] = '\0';
            scoreboard(len);
            printScoreboard();
            printf("Restarting Game...\n===================\n");
            chars_received = 0;
            len = 1;
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