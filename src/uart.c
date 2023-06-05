#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "buzzer.h"
#include "timer.h"
#include "headers.h"

// extern volatile State state;
extern volatile uint8_t pb_released;
extern volatile gameState pb_state;
extern volatile seqState state;
extern volatile uint32_t student_number;
extern volatile uint32_t new_number;
extern volatile char name[20];
volatile Serial_State SERIAL_STATE = AWAITING_COMMAND;
volatile uint8_t chars_received = 0;
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE);

int uart_putc_printf(char c, FILE *stream)
{
    uart_putc(c);
    return 0;
}

void uart_init(void)
{
    stdout = &mystdout;
    PORTB.DIRSET = PIN2_bm; // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 1389;     // 9600 baud @ 3.3 MHz
    USART0.CTRLA = USART_RXCIE_bm;
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
}

uint8_t uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ; // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ; // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

void uart_puts(char *string)
{

    while (*string)
    {
        uart_putc(*string++);
    }
}

uint8_t hexchar_to_int(char c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'f')
        return 10 + c - 'a';
    else
        return 16; // Invalid
}

ISR(USART0_RXC_vect)
{

    static uint16_t payload = 0;
    static uint8_t payload_valid = 1;

    uint8_t rx_data = USART0.RXDATAL;
    switch (SERIAL_STATE)
    {
    case AWAITING_COMMAND:
        switch (rx_data)
        {
        case '1':
        case 'q':
            if (pb_state == Wait)
            {
                pb_released = 1;
                pb_state = button1;
            }
            break;
        case '2':
        case 'w':
            if (pb_state == Wait)
            {
                pb_released = 1;
                pb_state = button2;
            }
            break;
        case '3':
        case 'e':
            if (pb_state == Wait)
            {
                pb_released = 1;
                pb_state = button3;
            }
            break;
        case '4':
        case 'r':
            if (pb_state == Wait)
            {
                pb_released = 1;
                pb_state = button4;
            }
            break;
        case ',':
        case 'k':
            toneIncrement();
            break;
        case '.':
        case 'l':
            toneDecrement();
            break;
        case '0':
        case 'p':
            seqToneStop(); 
            toneReset();
            if(new_number){
                student_number = new_number;
            }
            pb_state = Restart;       
            break;
        case '9':
        case 'o':
            payload_valid = 1;
            chars_received = 0;
            payload = 0;
            SERIAL_STATE = AWAITING_PAYLOAD;
            break;
        default:
            break;
        }
        break;
    case AWAITING_PAYLOAD:
    {
        uint8_t parsed_result = hexchar_to_int((char)rx_data);
        if (parsed_result != 16)
            payload = (payload << 4) | parsed_result;
        else
            payload_valid = 0;

        if (++chars_received == 8)
        {
            new_number = payload;
            SERIAL_STATE = AWAITING_COMMAND;
        }
        break;
    }
    case AWAITING_NAME:
        if (rx_data == '\n' || rx_data == '\r')
        {
            state = seqSetName;
            SERIAL_STATE = AWAITING_COMMAND;
        }
        else
        {
            name[chars_received] = rx_data;
            chars_received++;
            elapsed_time = 0;
        }
        break;
    default:
        break;
    }
}