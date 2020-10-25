from serialport import serial_ports
from time import sleep
from threading import Thread
from tool import only
import threading
import serial

class Micon(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.ser = None
        self.ready = 0
        self.stop = 0
        self.daemon = 1

    def __exit__(self, type, value, traceback):
        if self.ready:
            self.ser.close()
    
    def connect(self, name='', force = 0):
        if force:
            self.ready = 0
        while not self.ready:
            for port in serial_ports(name):
                try:
                    self.ser = serial.Serial(port, 9600, timeout=100)
                    if force:
                        self.ready = 1
                        break
                    self.ser.flushInput()
                    self.ser.write(b'[Z]')
                    ret = self.ser.readline()
                    if 'OK' in ret:
                        self.ready = 1
                        break
                except:
                    pass
    
    def run(self):
        while self.ready:
            if self.ser:
                read = self.ser.read_all()
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

if __name__ == "__main__":
    micon = Micon()
    micon.connect(force=1)
    micon.start()
    val = 0
    for i in range(5):
        micon.dealt(('V', 1000-i*100, 1000-i*100))
        sleep(2)

    micon.dealt(('V', 0, 0))