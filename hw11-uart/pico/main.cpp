
#include "timer_callback.h"

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#include <cstdio>

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_PORT uart0
#define BAUD_RATE 115200

// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 0
#define UART_RX_PIN 1


int main()
{
    stdio_init_all();

    // Set up our UART
    uart_init( UART_PORT, BAUD_RATE );
    
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_PORT, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_PORT, UART_RX_PIN));

    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    //uart_puts( UART_PORT, " Hello, UART!\r\n" );
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    static struct repeating_timer t {};
    add_repeating_timer_ms( 10, &TimerCallback::operator(), nullptr, &t );

    while ( true )
    {
        if ( !TimerCallback::expired ) { continue; }
        TimerCallback::expired = false;

        int pc_byte {};
        if ( PICO_ERROR_TIMEOUT != ( pc_byte = getchar_timeout_us( 0 ) ) )
        {
            uart_putc_raw( UART_PORT, ( uint8_t ) pc_byte );
        }

        if ( uart_is_readable( UART_PORT ) )
        {
            putchar_raw( uart_getc( UART_PORT ) );
        }
        stdio_flush();

    }
}
