# -*- coding: utf-8 -*-
# import socket
# import time

# HOST ='127.0.0.1'# 伺服器的主機名或者 IP 地址
# PORT = 12300  # 伺服器使用的埠


# def send(text, name='unknow'):
#     print(F'{text},{name}')
#     if not debug:
#         with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#             s.connect((HOST, PORT))
#             s.sendall(bytes(F'{text},{name}', 'utf-8'))


# if __name__ == "__main__":
#     start = time.time()
#     for i in range(100):
#         time.sleep(1)
#         with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
#             print(F'send{i}')
#             s.connect((HOST, PORT))
#             s.sendall(bytes(F'test[{i}],client', 'utf-8'))
#             sleep(1)
#             s.sendall(bytes(F'test[{i}],client', 'utf-8'))
#     end = time.time()
#     print(F'{start}->{end}:  {end-start}')
# -*- coding: utf-8 -*-
from pynput import keyboard
from time import time, sleep
import socket

HOST ='127.0.0.1'# 伺服器的主機名或者 IP 地址
PORT = 12301  # 伺服器使用的埠
last = time()
Heartbeat = 0.3  # sec
stop = 0


def press(key):
    try:
        return bytes(F'D_{key.char}', 'utf-8')
    except AttributeError:
        return bytes(F'D_{key}', 'utf-8')

def release(key):
    if key == keyboard.Key.esc:
        global stop
        stop = 1
    try:
        return bytes(F'U_{key.char}', 'utf-8')
    except AttributeError:
        return bytes(F'U_{key}', 'utf-8')

if __name__ == "__main__":
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        with keyboard.Listener(
            on_press = lambda key:s.sendall(press(key)),
            on_release = lambda key:s.sendall(release(key))) as listener:
            while not stop:
                sleep(0.05)
                if time()-last > Heartbeat:
                    last = time()
                    s.sendall(bytes('OK', 'utf-8'))
