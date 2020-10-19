from time import sleep, time
from client import send
try:
    import pigpio
    debug = 0
except ImportError:
    print('Warning: pigio is NOT imported')
    debug = 1
    import mpigpio as pigpio
pwmpin = 12
pi = pigpio.pi()
for i in range(10):
    ret = pi.hardware_PWM(12, 1000, 100000 * (i + 1))
    send(ret)
    send(100000 * (i + 1), 'pwm')
    sleep(2)
# pi.set_PWM_range(pwmpin, 9999)
# pi.set_PWM_dutycycle(pwmpin, 50)