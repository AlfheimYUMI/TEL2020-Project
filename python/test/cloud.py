# -*- coding: utf-8 -*-
from pynput import keyboard
from time import time, sleep
from micon import Micon
import socket

HOST ='192.168.0.2'# 伺服器的主機名或者 IP 地址
PORT = 12301  # 伺服器使用的埠
last = time()
Heartbeat = 0.3  # sec
stop = 0
keys = {}
speed = 800
rotationRatio = 0.2


class Car:
    def __init__(self, host=HOST, port = PORT, debug=0):
        self.debug = debug
        if not debug:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((host, port))
        else:
            self.s = 0
        self.V = (0,0)
        self.a = (9,0,0,0,0,0)
        self.keys = {}
        self.crab = 0

    def keyDown(self, key):
        try:
            self.keys[key.char] = 1
        except:
            self.keys[key.__str__()] = 1
        self.update()

    def keyUp(self, key):
        try:
            self.keys[key.char] = 0
        except:
            self.keys[key.__str__()] = 0
        self.update()

    def update(self):
        print(self.keys)
        if self.keys.get('p'):
            self.V = (0, 0)
            self.a = (9,)
            return

        move = 1 if self.keys.get('w') else (-1 if self.keys.get('s') else 0)
        dire = 1 if self.keys.get('d') else (-1 if self.keys.get('a') else 0)
        
        speedL = 0
        speedR = 0
        if move:
            speedL = speed*move
            speedR = speed*move
            if dire:
                speedL -= rotationRatio*speed*dire*move
                speedR += rotationRatio*speed*dire*move
        else:
            if dire:
                speedL = -speed*dire
                speedR = speed*dire
        self.V = (speedR, speedL)
        for i in "123456789":
            if self.keys.get(i):
                self.a = (i,)
                return
        XX = 1 if self.keys.get('i') else (-1 if self.keys.get('k') else 0)
        YY = 1 if self.keys.get('j') else (-1 if self.keys.get('l') else 0)
        ZZ = 1 if self.keys.get('u') else (-1 if self.keys.get('o') else 0)
        AA = 1 if self.keys.get('m') else (-1 if self.keys.get(',') else 0)
        self.a = (0, XX, YY, ZZ, AA, 0)
    def send(self):
        if self.s:
            self.s.sendall(bytes(F"[V,{','.join(map(str,self.V))}]", 'utf-8'))
            self.s.sendall(bytes(F"[a,{','.join(map(str,self.a))}]", 'utf-8'))
        else:
            return F"[V,{','.join(map(str,self.V))}]"+F"[a,{','.join(map(str,self.a))}]"

if __name__ == "__main__":
    debug = 0
    if debug:
        micon = Micon()
        micon.connect(force=1)
        micon.start()
    car = Car(debug=debug)
    with keyboard.Listener(
        on_press = car.keyDown,
        on_release = car.keyUp) as listener:
        while not stop:
            sleep(0.1)
            if debug:
                micon.write(car.send())
            else:
                car.send()
