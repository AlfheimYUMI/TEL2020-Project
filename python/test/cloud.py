# -*- coding: utf-8 -*-
from pynput import keyboard
from time import time, sleep
import socket

HOST ='192.168.43.118'# 伺服器的主機名或者 IP 地址
PORT = 12301  # 伺服器使用的埠
last = time()
Heartbeat = 0.3  # sec
stop = 0
keys = {}
speed = 800
rotationRatio = 0.2


class Car:
    def __init__(self, host=HOST, port = PORT, debug=0):
        if not debug:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((host, port))
        self.V = (0,0)
        self.a = (100,0,100,0,1)
        self.keys = {}

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
        self.V = (speedL, speedR)
    
    def send(self):
        self.s.sendall(bytes(F"[V,{','.join(map(str,self.V))}]", 'utf-8'))
        self.s.sendall(bytes(F"[a,{','.join(map(str,self.a))}]", 'utf-8'))

if __name__ == "__main__":
    car = Car(debug=0)
    with keyboard.Listener(
        on_press = car.keyDown,
        on_release = car.keyUp) as listener:
        while not stop:
            sleep(0.5)
            car.send()
