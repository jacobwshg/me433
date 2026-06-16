
import board
import pwmio
import time

PWM_PIN = board.GP26

PWM_CLK_PERIOD = 20 #ms

# conservative
#MIN_DUTY_FRAC = 0.05
#MAX_DUTY_FRAC = 0.10

MIN_DUTY_FRAC = 0.025
MAX_DUTY_FRAC = 0.125

WRAP = 65535

MIN_DUTY = MIN_DUTY_FRAC * WRAP
MAX_DUTY = MAX_DUTY_FRAC * WRAP

INV_180 = 1.0 / 180.0

ANGLE_DUTY_TBL = [
    ( int ) (
        MIN_DUTY + ( MAX_DUTY-MIN_DUTY ) * ( angle*INV_180 )
    )
    for angle in range( 181 )
]

def main():

    servo = pwmio.PWMOut( PWM_PIN, variable_frequency=True )
    servo.frequency = 50

    servo.duty_cycle = ANGLE_DUTY_TBL[ 0 ]

    while True:
        for angle in range( 180 ):
            servo.duty_cycle = ANGLE_DUTY_TBL[ angle ]
            time.sleep( 0.01 )
        for angle in range( 180, 0, -1 ):
            servo.duty_cycle = ANGLE_DUTY_TBL[ angle ]
            time.sleep( 0.01 )

if __name__ == "__main__":
    main()
