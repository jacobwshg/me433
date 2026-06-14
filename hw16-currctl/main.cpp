
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
#include <array>

namespace
{

    static constexpr std::uint16_t
        POT_HI { 4000 }, 
        POT_LO { 3800 };

    static inline volatile std::int8_t STATE { 0 };
    static inline float current_ref_f { 1.65F };

}

namespace PI
{
    static constexpr float
        Kp { 0.5 },
        Ki { 0.1 };

    static inline float S_error { 0.0F }; // error integral

    static inline float ctlval { 0.0F };

    static inline float update( const float err )
    {
        S_error += err;
        ctlval = Kp * err + Ki * S_error;
        return ctlval;
    }

    static inline void reset( void )
    {
        S_error = 0.0F;
    }
}

static inline std::uint16_t get_adc_avg( const std::uint16_t adc_val )
{
    static constexpr std::size_t SAMPLE_CNT { 8 };
    static std::array< std::uint16_t, SAMPLE_CNT > adc_ringbuf {};
    static std::uint16_t adc_sum { 0 };
    static std::size_t idx { 0 };

    adc_sum -= adc_ringbuf[ idx ];
    adc_ringbuf[ idx ] = adc_val;
    adc_sum += adc_val;
    if ( ++idx == SAMPLE_CNT ) { idx = 0; }
    const std::uint16_t adc_avg { adc_sum / SAMPLE_CNT };
    return adc_avg;
}

struct TimerCallback
{
    static inline std::int32_t counter { 0 };
    static inline float current_f { 0.0F };

    static inline bool
    operator()( __unused struct repeating_timer *t )
    {

        const std::uint16_t adc_val { adc_read() };
        const std::uint16_t adc_avg { get_adc_avg( adc_val ) };
        const std::int16_t curr { INA219::read_current_raw() };

        if ( adc_avg > ::POT_HI )
        {
            PWMUtil::set_chan_ab( PWMUtil::WRAP, PWMUtil::DUTY_LOW );
        }
        if ( adc_avg < ::POT_LO )
        {
            PWMUtil::set_chan_ab( PWMUtil::DUTY_LOW, PWMUtil::WRAP );
        }
        return true;
    }
};


int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init( Pin::ADC );
    adc_select_input( 0 );

    I2CUtil::init();

    PWMUtil::init();

    sleep_ms( 6000 );
    std::printf( "INA219 no init config: 0x%X, power: %u, calib: %u\n",
        INA219::read( INA219::Reg::CONFIG ),
        INA219::read( INA219::Reg::POWER ),
        INA219::read( INA219::Reg::CALIB )
    );

    INA219::init();
    sleep_ms( 2000 );

    std::printf( "INA219 after init config: 0x%X, power: %u, calib: %u\n",
        INA219::read( INA219::Reg::CONFIG ),
        INA219::read( INA219::Reg::POWER ),
        INA219::read( INA219::Reg::CALIB )
    );


    struct repeating_timer timer {};
    add_repeating_timer_ms(
        -10, // callbacks begin 1ms apart - 1kHZ
        &TimerCallback::operator(),
        NULL, &timer
    );

    ::current_ref_f = 1.5F;
    ::STATE = 1;

    PWMUtil::halt();

    while ( true )
    {
    //     sleep_ms( 100 );
    //     std::printf( "pot: %u, curr: %d\n", adc_read(), INA219::read_current_raw() );

        sleep_ms( 5 );
        const uint16_t pot { adc_read() };
        std::printf( "pot: %u\n", pot );


    }
}
