
#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/pwm.h" // CMakeLists.txt must have hardware_pwm in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

static const uint PWM_PIN = 14;

// 
// PWM freq ( 50Hz ) * wrap * div = clk freq ( 150MHz )
//
static const float DIV = 150.0F; // must be between 1-255
static const uint16_t WRAP = 20000; // when to rollover, must be less than 65535

typedef int angle_t;

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
	static const float INV_180 = 1.0F / 180.0F;

	//
	// fraction of pulse duty cycle within PWM freq
	//
	static const float
		// conservative
		//MIN_DUTY_FRAC = 0.05F,
		//MAX_DUTY_FRAC = 0.10F;

		MIN_DUTY_FRAC = 0.025F,
		MAX_DUTY_FRAC = 0.125F;

	static const float
		MIN_DUTY = WRAP * MIN_DUTY_FRAC,
		MAX_DUTY = WRAP * MAX_DUTY_FRAC;

	const float duty_delta = 
		( deg * INV_180 ) * ( MAX_DUTY-MIN_DUTY );
   
	const uint16_t lv = ( uint16_t )
		( MIN_DUTY + duty_delta );

	return lv;
}

static inline void
setangle( const angle_t angle )
{
	pwm_set_gpio_level(
		PWM_PIN,
		deg_to_gpio_level( ( float ) angle )
	);
}

int 
main()
{
	static const angle_t
		MINANGLE = 10,
		MAXANGLE = 170;

	static const uint32_t SLEEP_DUR = 10;

	stdio_init_all();

	// turn on a timer interrupt
	struct repeating_timer timer;
	// -100 means call the function every 100ms
	// +100 would mean call the function 100ms after the function has ended
	//add_repeating_timer_ms( -100, timer_interrupt_function, NULL, &timer );

	gpio_set_function( PWM_PIN, GPIO_FUNC_PWM ); // Set the Pin to be PWM
	const uint slice_num = pwm_gpio_to_slice_num( PWM_PIN ); // Get PWM slice number

	//pwm_config pwm_cfg = pwm_get_default_config();
	//pwm_config_set_clkdiv( &pwm_cfg, DIV ); // sets the clock speed
	//pwm_config_set_wrap  ( &pwm_cfg, WRAP ); 
	//pwm_init( slice_num, &pwm_cfg, true );

	pwm_set_clkdiv( slice_num, DIV );
	pwm_set_wrap( slice_num, WRAP );
	pwm_set_enabled( slice_num, true ); // turn on the PWM

	setangle( MINANGLE );
	sleep_ms( 100 );

	// turn on the adc
	//adc_init();
	//adc_gpio_init( 26 ); // pin GP26 is pin ADC0
	//adc_select_input( 0 ); // sample from ADC0

	while ( true )
	{
		//tight_loop_contents(); // do nothing, the interrupt does the work

		for ( angle_t angle = MINANGLE; angle <= MAXANGLE; ++angle )
		{
			setangle( angle );
			sleep_ms( SLEEP_DUR );
		}
		for ( angle_t angle = MAXANGLE; angle >= MINANGLE; --angle )
		{
			setangle( angle );
			sleep_ms( SLEEP_DUR );
		}

	}
}
