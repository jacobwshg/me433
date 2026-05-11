
#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/pwm.h" // CMakeLists.txt must have hardware_pwm in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

static const uint PWM_PIN = 16;

// 
// PWM freq ( 50Hz ) * wrap * div = clk freq ( 150MHz )
//
static const float DIV = 60.f; // must be between 1-255
static const uint16_t WRAP = 50000; // when to rollover, must be less than 65535

bool
timer_interrupt_function( __unused struct repeating_timer *t )
{
    // read the adc
    uint16_t result1 = adc_read();
    // print the voltage
    printf( "%f\r\n",( float )result1 / 4095 * 3.3 );
    return true;
}

uint16_t
deg_to_gpio_level( const float deg )
{
    static const float INV_180 = 0.055555556F;

    //
    // fraction of pulse duty cycle within PWM freq
    //
    static const float
        // conservative
        MIN_DUTY_FRAC = 0.05F,
        MAX_DUTY_FRAC = 0.10F;

    const float duty_frac_delta = 
        ( deg*INV_180 ) * ( MAX_DUTY_FRAC-MIN_DUTY_FRAC );
   
    const uint16_t lv = ( uint16_t )
        ( ( MIN_DUTY_FRAC + duty_frac_delta ) * WRAP );

    return lv;
}

static inline void
setangle( const uint angle )
{
    pwm_set_gpio_level(
        PWM_PIN,
        deg_to_gpio_level( ( float ) angle )
    );
}

int 
main()
{
    stdio_init_all();

    // turn on a timer interrupt
    struct repeating_timer timer;
    // -100 means call the function every 100ms
    // +100 would mean call the function 100ms after the function has ended
    add_repeating_timer_ms( -100, timer_interrupt_function, NULL, &timer );


    gpio_set_function( PWM_PIN, GPIO_FUNC_PWM ); // Set the Pin to be PWM
    const uint slice_num = pwm_gpio_to_slice_num( PWM_PIN ); // Get PWM slice number

    pwm_set_clkdiv( slice_num, DIV ); // sets the clock speed
    pwm_set_wrap( slice_num, WRAP ); 

    pwm_set_enabled( slice_num, true ); // turn on the PWM

    pwm_set_gpio_level( PWM_PIN, 0 ); // set the duty cycle to 50%

    // turn on the adc
    adc_init();
    adc_gpio_init( 26 ); // pin GP26 is pin ADC0
    adc_select_input( 0 ); // sample from ADC0

    static const uint
        MINANGLE = 10,
        MAXANGLE = 170;

    static const uint32_t SLEEP_DUR = 10;

    while ( true )
    {
        //tight_loop_contents(); // do nothing, the interrupt does the work

        for ( uint angle = MINANGLE; angle <= MAXANGLE; ++angle )
        {
            setangle( angle );
            sleep_ms( SLEEP_DUR );
        }
        sleep_ms( SLEEP_DUR );
        for ( uint angle = MAXANGLE; angle >= MINANGLE; --angle )
        {
            setangle( angle );
            sleep_ms( SLEEP_DUR );
        }

    }
}
