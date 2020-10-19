from time import sleep, time
try:
    import pigpio
    debug = 0
except ImportError:
    print('Warning: pigio is NOT imported')
    debug = 1
    import mpigpio as pigpio
pwmpin = 12
pi = pigpio.pi()
pi.set_PWM_range(pwmpin, 9999)
pi.set_PWM_dutycycle(pwmpin, 50)