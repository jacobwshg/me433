
#include "pin.h"
#include "util.h"
#include "pwm_util.h"
#include "i2c_util.h"
#include "HX711.h"
#include "INA219.h"
#include "iir.h"

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

#include <cstdio>
#include <limits>
#include <algorithm>

namespace
{

    IIR iir_F { 0.95F };
    //
    // HX711 readings are typically positive numbers
    // on the order of 1e5-1e6,
    //
    // which decreases with force
    //
    static std::int32_t
        Fmin { INT32_MAX },
        Fmax { INT32_MIN },
        // Fmax - Fmin
        Frange { 0 };

    //
    // Fthresh =  Fmin + ( Frange * FTHRESH_SCALE )
    // force <= Fthresh considered active
    //
    static constexpr float FTHRESH_SCALE { 0.4 };
    static std::int32_t Fthresh { INT32_MAX };

    //
    // INA219 readings can be positive or negative
    // depending on current direction
    //
    static std::int32_t Imax_abs { 100 };
    static float Imax_abs_inv_f { 0.0F };

    //
    // Ithresh_abs = Imax_abs * ITHRESH_SCALE
    // |current| >= Ithresh_abs considered active
    //
    static constexpr float ITHRESH_SCALE { 0.1 };
    static std::int32_t Ithresh_abs { INT32_MAX };
}

struct TimerCallback
{
    static inline volatile bool expired { false };

    static inline bool
    operator()( __unused repeating_timer_t *t )
    {
        expired = true;
        return true;
    }
};

static inline void
button_init( void )
{
    gpio_init( Pin::BTN );
    gpio_set_dir( Pin::BTN, GPIO_IN );
    gpio_pull_down( Pin::BTN );
}

/*
 * use a couple of seconds to sample the extreme values
 * of user force and current input
 */
static inline void
get_FI_lims( void )
{
    static constexpr absolute_time_t NSECS { 20 };
    absolute_time_t starttime { get_absolute_time() };
    const absolute_time_t endtime { starttime + NSECS * 1'000'000 };

    std::printf( "begin warmup sampling\n" );

    //while ( get_absolute_time() < endtime )
    while ( PICO_ERROR_TIMEOUT == getchar_timeout_us( 1 ) )
    {
        sleep_ms( 50 );

        static std::uint32_t pot {};
        pot = adc_read();

        static std::int32_t F {};
        F = HX711::read_sample();
        F = ::iir_F.add_sample< std::int32_t >( F );
        ::Fmin = std::min( F, ::Fmin );
        ::Fmax = std::max( F, ::Fmax );

        static std::int32_t I {};
        I = INA219::read_current_raw();
        ::Imax_abs = std::max( std::abs( I ), Imax_abs );

        if ( TimerCallback::expired )
        {
            TimerCallback::expired = false;
            std::printf( "sampled force %d, curr %d, pot %u\n", F, I, pot );
        }

    }

    std::printf( "warmup sampling done\n" );

    ::Frange = ::Fmax - ::Fmin;
    ::Fthresh = static_cast< std::int32_t >(
        static_cast< float >( ::Fmax ) -
        ::FTHRESH_SCALE * ::Frange
    );

    //
    // cache inverse once to avoid repeated division 
    // when scaling down future current readings
    //
    ::Imax_abs_inv_f = 1.0F / ::Imax_abs;
    ::Ithresh_abs = static_cast< std::int32_t >(
        ::ITHRESH_SCALE * ::Imax_abs
    );

    std::printf( "Fmin %d, Fmax %d, |Imax| %d", ::Fmin, ::Fmax, ::Imax_abs );

}

int main()
{
    stdio_init_all();

    I2CUtil::init();
    PWMUtil::init();

    HX711::init();

    adc_init();
    adc_gpio_init( Pin::ADC0 );
    adc_select_input( 0 );

    sleep_ms( 5000 );

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


    repeating_timer_t sample_disp_timer {};
    add_repeating_timer_ms( -100, &TimerCallback::operator(), NULL, &sample_disp_timer );

    get_FI_lims();

    // remove
    //while ( true ) {}

    while ( true )
    {
        sleep_ms( 20 );

        if ( gpio_get( Pin::BTN ) )
        {
            std::printf( "button pressed\n" );
        }

        static bool load_cell_pressed { false };
        static std::int32_t F {};
        F = HX711::read_sample();
        load_cell_pressed = { F <= ::Fthresh };

        if ( load_cell_pressed )
        {
            // load cell pressed - unlock motor
            PWMUtil::coast();
            std::printf( "load cell pressed, force: %d\n", F );
        }
        else
        {
            static bool motor_pushed { false };
            static std::int32_t I {}, I_abs {};
            I = INA219::read_current_raw();
            I_abs = std::abs( I );
            motor_pushed = { I_abs >= ::Ithresh_abs };
            if ( !motor_pushed )
            {
                // motor not pushed - hold neutral
                PWMUtil::brake();
            }
            else
            {
                std::printf( "motor pushed, curr: %d\n", I );
                // motor pushed - push back agains user
                static std::int8_t dir {};
                dir = I > 0 ? 1 : -1;

                static float curr_scale {};
                curr_scale = ::Imax_abs_inv_f * I_abs;
                curr_scale = std::clamp( curr_scale, 0.0F, 1.0F );

                static PWMUtil::duty_t delta_duty {};
                delta_duty = static_cast< PWMUtil::duty_t >(
                    curr_scale * PWMUtil::WRAP
                );

                switch ( dir )
                {
                case -1:
                    {
                        PWMUtil::set_chan_ab( PWMUtil::WRAP, PWMUtil::WRAP - delta_duty );
                    }
                    break;
                case 1:
                    {
                        PWMUtil::set_chan_ab( PWMUtil::WRAP - delta_duty, PWMUtil::WRAP );
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}
