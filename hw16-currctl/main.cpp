
#include "pin.h"
#include "i2c_util.h"
#include "pwm_util.h"
#include "INA219.h"

#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

#include <cstdio>
#include <cstdint>

namespace
{
    static inline volatile std::int8_t STATE { 0 };
    static constexpr float current_ref_f { 1.65F };
}

namespace PI
{
    static constexpr float
        Kp { 0.5 },
        Ki { 0.1 };

    static inline float error { 0.0F };
    static inline float S_error { 0.0F }; // error integral

    static inline float ctlval { 0.0F };

    static inline float update( const float err )
    {
        error = err;
        S_error += err;
        ctlval = Kp * err + Ki * S_error;
        return ctlval;
    }
}

// struct TimerCallback
// {
//     static inline std::int32_t counter { 0 };
//     static inline float current_f { 0.0F };

//     static inline bool
//     operator()( __unused struct repeating_timer *t )
//     {
//         if ( ::STATE )
//         {
//             const std::int16_t current { INA219::read() };
//             current_f = INA219::tomA( current );
//         }
//         const float err { ::current_ref_f - current_f };
//         const float ctlval = PI::update( err );

//         const std::uint16_t pwm_duty { static_cast< std::uint16_t >( ctlval ) };
//         pwm_set_chan_level( PWMUtil::SLICE, PWM_CHAN_A, pwm_duty );
//         pwm_set_chan_level( PWMUtil::SLICE, PWM_CHAN_B, PWMUtil::WRAP );


//     }
// };

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init( Pin::ADC );
    adc_select_input( 0 );

    I2CUtil::init();

    PWMUtil::init();

    while ( true )
    {
        const std::uint16_t adc_val { adc_read() };
        const std::int16_t curr { INA219::read_current_raw() };

        std::printf( "ADC: %u, curr: %d", adc_val, curr );

        sleep_ms( 200 );
    }
}
