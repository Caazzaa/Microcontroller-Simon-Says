#include <stdint.h>
#include <stdio.h>

void uart_init(void);
uint8_t uart_getc(void);
void uart_putc(uint8_t c);
void uart_puts(char* string);
int uart_putc_printf(char c, FILE *stream);