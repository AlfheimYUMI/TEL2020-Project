from serialport import serial_ports
from time import sleep
from threading import Thread
from tool import only
import threading
import serial

class Micon(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.ser1 = None
        self.ser2 = None
        self.ready = 0
        self.stop = 0
        self.daemon = 1

    def __exit__(self, type, value, traceback):
        if self.ser1:
            self.ser1.close()
        if self.ser2:
            self.ser2.close()
    
    def connect(self, name='USB', force = 0):
        try:
            self.ser1 = serial.Serial('/dev/ttyUSB1', 115200)
            print('connect USB1')
        except:
            self.ser1 = None
        try:
            self.ser2 = serial.Serial('/dev/ttyUSB2', 115200)
            print('connect USB2')
        except:
            self.ser2 = None
        if self.ser1 or self.ser2:
            self.ready = 1
        # if force:
        #     self.ready = 0
        # while not self.ready:
        #     for port in serial_ports(name):
        #         try:
        #             self.ser = serial.Serial(port, 115200)
        #             if force:
        #                 self.ready = 1
        #                 break
        #             self.ser.flushInput()
        #             self.ser.write(b'[Z]')
        #             ret = self.ser.readline()
        #             if 'OK' in ret:
        #                 self.ready = 1
        #                 break
        #         except:
        #             pass
    
    def run(self):
        while self.ready:
            if self.ser1:
                read = self.ser1.read_all()
                if read:
                    print(read)
            if self.ser2:
                read = self.ser2.read_all()
                if read:
                    print(read)
            sleep(0.2)
                # self.dealt((' '))

    def dealt(self, itera):
        if itera:
            cmd = str(itera[0])
            vals = list(map(str, itera[1:]))
            self.sand(cmd, vals)

    def sand(self, cmd='z', vals=[], start='[', split=',', end=']'):
        cmds = [cmd] + vals
        text = start+split.join(cmds)+end
        print("text = ", text)
        if self.ser:
            self.ser.write(bytes(text, 'ascii'))

    def write(self, cmd):
        print(cmd)
        if self.ser1:
            self.ser1.write(bytes(cmd, 'ascii'))
        if self.ser2:
            self.ser2.write(bytes(cmd, 'ascii'))
    def write1(self, cmd):
        print(cmd)
        if self.ser1:
            self.ser1.write(bytes(cmd, 'ascii'))
    def write2(self, cmd):
        print(cmd)
        if self.ser2:
            self.ser2.write(bytes(cmd, 'ascii'))

if __name__ == "__main__":
    micon = Micon()
    micon.connect(force=1)
    micon.start()
    val = 0
    for i in range(5):
        micon.write(F'[V,{1000-i*100},{1000-i*100}]')
        sleep(2)

    micon.write(F'[V,0,0]')