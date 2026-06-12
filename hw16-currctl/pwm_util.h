
#ifndef PWM_UTIL_H__
#define PWM_UTIL_H__

#include "pin.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <cstdio>

namespace PWMUtil
{
    static constexpr uint WRAP { 2400 };

    static constexpr uint FREQ_KHZ { 20 };
    static constexpr uint FREQ_HZ  { FREQ_KHZ * 1000 };

    static constexpr float CLKDIV
    {
        150'000'000.0F / ( static_cast< float >( FREQ_HZ ) * WRAP )
    };


    static const uint SLICE { pwm_gpio_to_slice_num( Pin::PWM_A ) };

    static inline void init( void );

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
    gpio_set_function( Pin::PWM_B, GPIO_FUNC_PWM );

    // std::printf(
    //     "%f %d\n", CLKDIV, WRAP
    // );

    pwm_config cfg { pwm_get_default_config() };
    pwm_config_set_clkdiv( &cfg, CLKDIV );
    pwm_config_set_wrap( &cfg, WRAP );


    pwm_init( SLICE, &cfg, true );
    pwm_set_enabled( SLICE, true );
}

#endif
