#include "headers.h"

#define SEGS_EF 0b00111110
#define SEGS_BC 0b01101011
#define SEGS_G 0b01110111
#define SEGS_ON 0b00000000
#define SEGS_OFF 0b01111111

int i;
uint16_t duration;

volatile uint32_t new_number;
volatile uint32_t student_number = 0x11243635;
extern volatile uint8_t pb_debounced;
volatile gameState pb_state = Wait;
volatile uint8_t pb_released = 0;
volatile uint8_t segs[] = {SEGS_OFF, SEGS_OFF};
volatile uint8_t scoreSEGS[] = {
    0x08, 0x6B, 0x44, 0x41, 0x23, 0x11, 0x10, 0x4B, 0x00, 0x01};

void display_score(uint16_t len)
{
    segs[0] = SEGS_OFF;
    if (len > 9)
        segs[0] = scoreSEGS[len % 100 / 10];
    segs[1] = scoreSEGS[len % 10];
}

int seqGenerator(uint32_t *state_lfsr)
{
    uint8_t bit = *state_lfsr & 1;
    *state_lfsr >>= 1;
    if (bit == 1)
    {
        *state_lfsr ^= 0xE2023CAB;
    }
    return *state_lfsr & 0b11;
}

int seqRun(uint16_t len)
{
    uint16_t count = 0;
    uint32_t state_lfsr = student_number;
    uint8_t step = seqGenerator(&state_lfsr);

    uint8_t pb_sample = 0xFF;
    uint8_t pb_sample_r = 0xFF;
    uint8_t pb_changed, pb_rising, pb_falling;

    while (1)
    {
        pb_sample_r = pb_sample;
        pb_sample = pb_debounced;
        pb_changed = pb_sample ^ pb_sample_r;

        pb_falling = pb_changed & pb_sample_r;
        pb_rising = pb_changed & pb_sample;

        allow_updating_playback_delay = 1;
        new_playback_time = durationPOT();

        switch (pb_state)
        {
        case Pause:
            pb_released = 0;
            break;
        case Wait:
            allow_updating_playback_delay = 0;
            elapsed_time = 0;
            pb_released = 0;

            segs[0] = SEGS_OFF;
            segs[1] = SEGS_OFF;

            if (pb_falling & PIN4_bm)
                pb_state = button1;
            else if (pb_falling & PIN5_bm)
                pb_state = button2;
            else if (pb_falling & PIN6_bm)
                pb_state = button3;
            else if (pb_falling & PIN7_bm)
                pb_state = button4;

            break;
        case button1:
            seqTone(0);
            segs[0] = SEGS_EF;
            if (!pb_released)
            {
                if (pb_rising & PIN4_bm)
                {
                    pb_released = 1;
                }
            }
            else
            {
                if (elapsed_time >= playback_time)
                {
                    seqToneStop();
                    allow_updating_playback_delay = 1;
                    pb_state = step == 0 ? Success : Fail;
                }
            }
            break;
        case button2:
            seqTone(1);
            segs[0] = SEGS_BC;
            if (!pb_released)
            {
                if (pb_rising & PIN5_bm)
                    pb_released = 1;
            }
            else
            {
                if (elapsed_time >= playback_time)
                {
                    seqToneStop();
                    allow_updating_playback_delay = 1;
                    pb_state = step == 1 ? Success : Fail;
                }
            }
            break;
        case button3:
            seqTone(2);
            segs[1] = SEGS_EF;
            if (!pb_released)
            {
                if (pb_rising & PIN6_bm)
                    pb_released = 1;
            }
            else
            {
                if (elapsed_time >= playback_time)
                {
                    seqToneStop();
                    allow_updating_playback_delay = 1;
                    pb_state = step == 2 ? Success : Fail;
                }
            }
            break;
        case button4:
            seqTone(3);
            segs[1] = SEGS_BC;
            if (!pb_released)
            {
                if (pb_rising & PIN7_bm)
                    pb_released = 1;
            }
            else
            {
                if (elapsed_time >= playback_time)
                {
                    seqToneStop();
                    allow_updating_playback_delay = 1;
                    pb_state = step == 3 ? Success : Fail;
                }
            }
            break;
        case Success:
            allow_updating_playback_delay = 1;
            seqToneStop();
            count++;
            if (len == count)
            {
                display_score(len);
                // Success pattern

                segs[0] = SEGS_ON;
                segs[1] = SEGS_ON;
                delay(durationPOT());

                delay(durationPOT() >> 1);
                segs[0] = SEGS_OFF;
                segs[1] = SEGS_OFF;
                delay(durationPOT() >> 1);

                pb_state = Pause;
                return 1;
            }
            else
            {
                step = seqGenerator(&state_lfsr);
                pb_state = Wait;
            }
            break;
        case Fail:
            display_score(len - 1);

            // Fail pattern
            seqToneStop();
            segs[0] = SEGS_G;
            segs[1] = SEGS_G;
            delay(durationPOT());

            delay(durationPOT() >> 1);
            segs[0] = SEGS_OFF;
            segs[1] = SEGS_OFF;
            delay(durationPOT() >> 1);

            count++;
            for (; count < len; count++)
            {
                seqGenerator(&state_lfsr);
            }

            student_number = state_lfsr;
            pb_state = Pause;
            return 0;
        case Restart:

            segs[0] = SEGS_OFF;
            segs[1] = SEGS_OFF;

            pb_state = Wait;
            return 0;
        default:
            pb_state = Wait;
            break;
        }
    }
    return 1;
}

int seqStart(uint16_t len)
{
    uint32_t state_lfsr = student_number;
    for (i = 0; i < len; i++)
    {
        uint8_t step = seqGenerator(&state_lfsr);
        switch (step)
        {
        case 0:
            seqTone(0);
            segs[0] = SEGS_EF;
            // delay(durationPOT() >> 1);
            // seqToneStop();
            // segs[0] = SEGS_OFF;
            // delay(durationPOT() >> 1);
            break;
        case 1:
            seqTone(1);
            segs[0] = SEGS_BC;
            // delay(durationPOT() >> 1);
            // seqToneStop();
            // segs[0] = SEGS_OFF;
            // delay(durationPOT() >> 1);
            break;
        case 2:
            seqTone(2);
            segs[1] = SEGS_EF;
            break;
        case 3:
            seqTone(3);
            segs[1] = SEGS_BC;
            break;
        default:
            break;
        }
        delay(durationPOT() >> 1);
        seqToneStop();
        segs[0] = SEGS_OFF;
        segs[1] = SEGS_OFF;
        delay(durationPOT() >> 1);
    }
    return 1;
}
