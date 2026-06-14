
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
#include <algorithm>

namespace PI
{
    static volatile bool state { 0 };

    static constexpr float
        Kp { 0.5F },
        Ki { 0.1F };

    static inline float S_error { 0.0F }; // error integral

    static inline float ctlval { 0.0F };

    static inline float update( const float err )
    {
        S_error += err;
        ctlval = Kp * err + Ki * S_error;
        return ctlval;
    }

    static inline void start( void )
    {
        state = 1;
        S_error = 0.0F;
    }

    static inline void stop( void )
    {
        state = 0;
        S_error = 0.0F;
    }
}

static inline std::uint16_t get_adc_avg( const std::uint16_t adc_val )
{
    static constexpr std::size_t SAMPLE_CNT { 4 };
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
    static inline std::int16_t current { 0 };
    static inline volatile std::size_t counter { 0 };

    static inline bool
    operator()( __unused struct repeating_timer *t )
    {

        static constexpr std::int16_t ABS_REF_CURRENT { 100 };
        static std::int16_t ref_current { -ABS_REF_CURRENT };
        static std::int16_t dir { -1 }; // 1 or -1

        static constexpr float REF_DELTA_DUTY_F { 1200.0F };

        static constexpr std::uint16_t SAFE_POT_MIN { 3800 }, SAFE_POT_MAX { 4000 };

        counter += 1;

        const std::uint16_t pot { adc_read() };
        const std::uint16_t pot_avg { get_adc_avg( pot ) };

        if ( !PI::state )
        {
            return true;
        }

        if ( pot_avg < SAFE_POT_MIN || pot_avg > SAFE_POT_MAX )
        {
            PWMUtil::halt();
            return true;
        }

        if ( counter >= 400 )
        {
            // Shutdown Sequence
            PWMUtil::halt();
            counter = 0;
            PI::stop();

            dir = 1;
            ref_current = ABS_REF_CURRENT;

            return true;
        }

        if ( counter % 100 == 0 )
        {
            dir = -dir;
            ref_current = -ref_current;
        }

        current = INA219::read_current_raw();
        const std::int16_t err { ref_current - current };
        float ctlval = PI::update( static_cast< float >( err ) );

        std::uint16_t duty_a { PWMUtil::WRAP }, duty_b { PWMUtil::WRAP };

        if ( dir > 0 )
        {
            ctlval = std::clamp( ctlval, 0.0F, 1.0F );
            const std::uint16_t delta_duty { static_cast< std::uint16_t >( ctlval * REF_DELTA_DUTY_F ) };
            duty_b = PWMUtil::WRAP - delta_duty;
        }
        else
        {
            ctlval = std::clamp( ctlval, -1.0F, 0.0F );
            ctlval = -ctlval;
            const std::uint16_t delta_duty { static_cast< std::uint16_t >( ctlval * REF_DELTA_DUTY_F ) };
            duty_a = PWMUtil::WRAP - delta_duty;
        }
        PWMUtil::set_chan_ab( duty_a, duty_b );

        std::printf( "counter: %lu, pot_avg: %u, curr: %d, dutys: %u %u\n", counter, pot_avg, current, duty_a, duty_b );

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

    sleep_ms( 8000 );
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

    // callbacks begin 1ms apart - 1kHZ
    static constexpr std::int32_t TIMER_INTR_HZ { 100 };
    static constexpr std::int32_t MS_PER_TIMER_INTR { 1000 / TIMER_INTR_HZ };

    struct repeating_timer timer {};
    add_repeating_timer_ms(
        -MS_PER_TIMER_INTR, 
        &TimerCallback::operator(),
        NULL, &timer
    );

    PWMUtil::halt();

    while ( true )
    {
        
        
        std::printf( "main loop curr: %d\n", INA219::read_current_raw() );
        sleep_ms( 500 );

        // PWMUtil::set_chan_ab( PWMUtil::WRAP, 2000 );
        // sleep_ms( 500 );
        // PWMUtil::set_chan_ab( 2000, PWMUtil::WRAP);
        
        // if ( !PI::state )
        // {
        //     PI::start();
        //     std::printf( "main loop curr: %d\n", INA219::read_current_raw() );
        // }
    }
}
