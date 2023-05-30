#include "headers.h"

extern volatile gameState pb_state;

struct userScore {
    char name[50];
    uint32_t score;
} board[5];

// uint32_t score[] = {
//     NULL, NULL, NULL, NULL, NULL};

typedef enum
{
    seqBegin,
    seqCheck,
    seqSuccess,
    seqFail,
    seqRestart
} seqState;

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
    seqState state = seqBegin;

    while (1)
    {
        switch (state)
        {
        case seqBegin:
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
            // printf("Good Job!\n");
            // printf("Your Score Is: %d\n", len);
            len++;
            pb_state = Wait;
            state = seqBegin;
            break;
        case seqFail:
            len--;
            printf("Your Score Was: %d\n", len);
            if(scoreboard(len)){
                printScoreboard();
            }
            printf("Restarting Game...\n");
            delay(500);
            len = 1;
            pb_state = Wait;
            state = seqBegin;
            break;
        }
    }
}

int scoreboard(uint16_t len)
{
    for (int i = 0; i < 5; i++)
    {
        if (len > board[i].score)
        {
            for (int j = 4; j > i; j--)
            {
                board[j].score = board[j-1].score;
            }
            board[i].score = len;
            return 1;
            break;
        }
    }
    return 0;
}

int printScoreboard(){
    for (int i = 0; i < 5; i++)
    {
        printf("%lu\n", board[i].score);
    }
}