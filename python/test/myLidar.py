from mrplidar import RPLidar
from serialport import serial_ports
from threading import Thread
from time import sleep, time
from tool import only

import threading

PATH = '/'.join(__file__.split('/')[:-1])

#TODO: 急速模式
@only
class MyLidar(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.lidar = RPLidar()
        self.data = [0] * 360
        self.qualit = [0] * 360
        self.scan = True
        self.threadLock = threading.Lock()
        self.daemon = 1
        self.realData = []
        self.tmp = []
        self.stop = 0
        self.ready = 0

    def connect(self, force=0):
        if force:
            self.ready = 0
        ports = serial_ports()
        while not self.ready:
            for port in ports:
                if self.lidar.connect(port=port):
                    self.ready = 1
                    break

    def disconnect(self):
        self.lidar.disconnect()
        self.ready = 0

    def pwm(self, pwm=511):
        self.lidar.set_pwm(pwm)
        
    def run(self):
        while not self.stop:
            if self.scan:
                self.recive()
            else:
                sleep(0.2)
        self.lidar.stop_motor()

    def recive(self):
        for measurment in self.lidar.iter_measurments(360):
            if not self.scan or self.stop:
                break
            new_scan, qualit, angle, distance = measurment
            self.tmp.append(measurment)
            self.threadLock.acquire()
            self.data[int(angle)] = distance
            self.qualit[int(angle)] = qualit
            self.threadLock.release()
            if new_scan:
                self.realData = self.tmp
                self.tmp = []
                print('new data')

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
    lidar.start()
    while 1:
        if input('>>>'):
            lidar.save_date()
        else:
            break
    lidar.exit()