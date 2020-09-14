from mrplidar import RPLidar
from serialport import serial_ports
from threading import Thread
from time import sleep, time

import threading

PATH = '/'.join(__file__.split('/')[:-1])


class myLidar(Thread):

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

    def connect(self):
        ports = serial_ports()
        for port in ports:
            if self.lidar.connect(port=port):
                break

    def disconnect(self):
        self.lidar.disconnect()

    def pwm(self, pwm=511):
        self.lidar.set_pwm(pwm)
        
    def run(self):
        while not self.stop:
            if self.scan:
                self.update()
            else:
                sleep(0.2)

    def update(self):
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
                pass

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
    lidar = myLidar()
    lidar.connect()
    lidar.start()
    while 1:
        if input('>>>'):
            lidar.save_date()
        else:
            break
    lidar.exit()