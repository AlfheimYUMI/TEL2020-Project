# -*- coding: utf-8 -*-
import socket
import time
try:
    import pigpio
    debug = 0
except ImportError:
    # print('Warning: pigio is NOT imported')
    debug = 1
    import mpigpio as pigpio
from time import sleep

HOST ='127.0.0.1'# 伺服器的主機名或者 IP 地址

PORT = 12300  # 伺服器使用的埠

def send(text, name='unknow'):
    print(F'{text},{name}')
    if not debug:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(bytes(F'{text},{name}', 'utf-8'))


if __name__ == "__main__":
    start = time.time()
    try:
        for i in range(100):
            time.sleep(1)
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                print(F'send{i}')
                s.connect((HOST, PORT))
                s.sendall(bytes(F'test[{i}],client', 'utf-8'))
                sleep(1000)
                s.sendall(bytes(F'test[{i}],client', 'utf-8'))
    except:
        pass
    end = time.time()
    print(F'{start}->{end}:  {end-start}')