from mrplidar import RPLidar
from serialport import serial_ports
from threading import Thread
from time import sleep

import threading


class myLidar(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.lidar = RPLidar()
        self.data = [0] * 360
        self.qualit = [0] * 360
        self.scan = True
        self.threadLock = threading.Lock()
        self.daemon = 1
        self.connect()
    
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
        while 1:
            if self.scan:
                self.update()
            else:
                sleep(0.2)

    def update(self):
        for measurment in self.lidar.iter_measurments(360):
            if not self.scan:
                break
            new_scan, qualit, angle, distance = measurment
            self.threadLock.acquire()
            self.data[int(angle)] = distance
            self.qualit[int(angle)] = qualit
            self.threadLock.release()
            if new_scan:
                # print(self.data)
                pass

    def getData(self, deg=0):
        self.threadLock.acquire()
        ret = self.data[deg], self.qualit[deg]
        self.threadLock.release()
        return ret

if __name__ == "__main__":
    lidar = myLidar()
    lidar.start()
    while 1:
        sleep(1)
        print(lidar.getData())
        lidar.scan = False