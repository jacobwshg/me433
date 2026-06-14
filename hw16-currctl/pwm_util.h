
#ifndef PWM_UTIL_H__
#define PWM_UTIL_H__

#include "pin.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <cstdio>


/*
 * decreasing chan A level spins CC,
 * decreasing chan B level spins C
 */

namespace PWMUtil
{
    static constexpr uint WRAP { 2400 };
    static constexpr uint DUTY_LOW { 2200 };


    static constexpr uint FREQ_KHZ { 20 };
    static constexpr uint FREQ_HZ  { FREQ_KHZ * 1000 };

    static constexpr float CLKDIV
    {
        150'000'000.0F / ( static_cast< float >( FREQ_HZ ) * WRAP )
    };


    static const uint SLICE { pwm_gpio_to_slice_num( Pin::PWM_A ) };

    static inline void init( void );

    static inline void set_chan_ab(
        const uint duty_a, const uint duty_b
    );

    static inline void halt( void );
}

static inline void
PWMUtil::init( void )
{
    //sleep_ms( 10000 );
    // std::printf(
    //     "%d %d %d \n%d %d %d\n",
    //     Pin::PWM_A, pwm_gpio_to_slice_num( Pin::PWM_A ), pwm_gpio_to_channel( Pin::PWM_A ),
    //     Pin::PWM_B, pwm_gpio_to_slice_num( Pin::PWM_B ), pwm_gpio_to_channel( Pin::PWM_B )
    //     );

    gpio_set_function( Pin::PWM_A, GPIO_FUNC_PWM );
    //gpio_pull_up( Pin::PWM_A );

    gpio_set_function( Pin::PWM_B, GPIO_FUNC_PWM );
    //gpio_pull_up( Pin::PWM_B );

    pwm_set_chan_level( SLICE, PWM_CHAN_A, 0 );
    pwm_set_chan_level( SLICE, PWM_CHAN_B, 0 );


    // std::printf(
    //     "%f %d\n", CLKDIV, WRAP
    // );

    pwm_config cfg { pwm_get_default_config() };
    pwm_config_set_clkdiv( &cfg, CLKDIV );
    pwm_config_set_wrap( &cfg, WRAP );


    pwm_init( SLICE, &cfg, true );
    pwm_set_enabled( SLICE, true );
}


static inline void
PWMUtil::set_chan_ab( const uint duty_a, const uint duty_b )
{
    pwm_set_both_levels( SLICE, duty_a, duty_b );
}

static inline void
PWMUtil::halt( void )
{
    set_chan_ab( WRAP, WRAP );
}

#endif
