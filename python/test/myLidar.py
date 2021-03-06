# -*- coding: utf-8 -*-
from mrplidar import RPLidar
from serialport import serial_ports
from threading import Thread
from time import sleep, time
from tool import *
from micon import Micon
import threading
try:
    import pigpio
    debug = 0
except ImportError:
    print('Warning: pigio is NOT imported')
    debug = 1
    import mpigpio as pigpio

PATH = '/'.join(__file__.split('/')[:-1])
ki = 1
kp = 0

FRONT = 90
BEHIND = 270
SIDE = 0
midRange = [-2, -1, 0, 1, 2]


#TODO: 急速模式
@only
class MyLidar(Thread):

    def __init__(self, print=print):
        self.print = print
        Thread.__init__(self)
        self.lidar = RPLidar()
        self.data = [0] * 362
        self.qualit = [0] * 362
        self.scan = True
        self.threadLock = threading.Lock()
        self.daemon = 1  
        self.realData = []
        self.tmp = []
        self.stop = 0
        self.ready = 0
        self._pi = get_only(pigpio.pi)
        self.speed = 360
        self.iterm = 821200
        self.output = 821200

    def pwm(self, target=360):
        error = self.speed-target
        self.iterm += error * ki
        self.output = error * kp + self.iterm
        self.output = max(min(1000000, self.output), 0)
        ret = self._pi.hardware_PWM(12, 1000, self.output)

    def front(self):
        pass

    def behind(self):
        pass

    def side(self):
        pass

    def update(self):
        pass

    def detect(self):
        # find edge
        base = self.data[FRONT]
        gap = 0.1*base
    
    def get_angle(self, center: int = 270, theta: int = 3):
        # print(self.data)
        d1 = sum(self.data[center + theta - 1:center + theta + 2]) / 3
        if center == 0:
            center = 360
        d2 = sum(self.data[center - theta - 1:center - theta + 2]) / 3

        angle = (d1 - d2) / max(d1, d2, 0.0001)

        return angle  # (distance, sin(angle))

    def connect(self, force=0):
        try:
            if self.lidar.connect(port='/dev/ttyUSB0'):
                self.ready = 1
        except:
            pass
        # ret = self._pi.hardware_PWM(12, 1000, self.output)
        # sleep(1)
        # if force:
        #     self.ready = 0
        # while not self.ready:
        #     ports = serial_ports()
        #     print(ports)
        #     for port in ports:
        #         print(port)
        #         try:
        #             if self.lidar.connect(port=port):
        #                 self.ready = 1
        #                 break
        #         except:
        #             pass

    def disconnect(self):
        self.lidar.disconnect()
        self.ready = 0
        
    def run(self):
        print('run', 'lidar')
        while not self.stop:
            if self.scan:
                try:
                    self.recive()
                except:
                    pass
            else:
                sleep(0.2)
        print(F'dead', 'lidar')
        self.lidar.stop_motor()

    def recive(self):
        for measurment in self.lidar.iter_measurments(360):
            if not self.scan or self.stop:
                break
            new_scan, qualit, angle, distance = measurment
            self.tmp.append(measurment)
            self.threadLock.acquire()
            try:
                self.data[int(angle)] = distance
                self.qualit[int(angle)] = qualit
            except:
                print(angle)
            self.threadLock.release()
            if new_scan:
                self.realData = self.tmp
                self.speed = len(self.realData)
                self.tmp = []
                self.pwm()
                print('new')

    def getData(self, deg=0):
        self.threadLock.acquire()
        ret = self.data[deg], self.qualit[deg]
        self.threadLock.release()
        return ret

    def save_date(self, path=PATH):
        with open(PATH + '/' + str(int(time())) + '.dat', 'w+') as f:
            for data in self.realData:
                a, b, c, d = data
                f.write(f'{a} {b} {c} {d}\n')

    def exit(self):
        self.stop = 1

if __name__ == "__main__":
    lidar = MyLidar()
    lidar.connect()
    # lidar.run()
    lidar.start()

    # print('conn micon')
    # micon = Micon()
    # micon.connect(force=1)
    # micon.start()
    # while 1:
    #     sleep(0.1)
    #     if lidar.data[270]<100:
    #         print('stop')
    #         micon.dealt(('V', 0,0))
    #         continue
    #     m = lidar.get_angle(270)
    #     if abs(m)<0.01:
    #         print('str')
    #         micon.dealt(('V', 200,200))
    #     elif m>0:
    #         print('right')
    #         micon.dealt(('V', 100,-100))
    #     else:
    #         print('left')
    #         micon.dealt(('V', -100,100))
    for i in range(100):
        print(lidar.data[270], lidar.data[269], lidar.data[271])
        print('F',lidar.get_angle(270))
        print('S',lidar.get_angle(0))
        print('B',lidar.get_angle(90))
        sleep(0.5)
    lidar.exit()