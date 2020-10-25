# -*- coding: utf-8 -*-
from threading import Thread
from time import time, sleep
from micon import Micon
import selectors
import socket
import types

status = {}
last = time()
timeout = 0.5
speed = 800
rotationRatio = 0.2

class SOC(Thread):
    def __init__(self, output, host='192.168.0.116', port=12301):
        Thread.__init__(self)
        lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        lsock.bind((host, port))
        lsock.listen()
        lsock.setblocking(False)
        self.sel = selectors.DefaultSelector()
        self.sel.register(lsock, selectors.EVENT_READ, data=None)
        self.out = output
        print(F'on{host}:{port}')
        self.daemon = 1

    def run(self):
        while True:
            events = self.sel.select(timeout=None)

            for key, mask in events:
                if key.data is None:
                    self.accept_wrapper(key.fileobj)
                else:
                    self.service_connection(key, mask)

    def accept_wrapper(self, sock):
        conn, addr = sock.accept()# Should be ready to read
        # print('accepted connection from', addr)
        conn.setblocking(False)
        data = types.SimpleNamespace(addr=addr, inb=b'')
        # events = selectors.EVENT_READ | selectors.EVENT_WRITE
        events = selectors.EVENT_READ
        self.sel.register(conn, events, data=data)

    def service_connection(self, key, mask):
        sock = key.fileobj
        data = key.data
        if mask & selectors.EVENT_READ:
            recv_data = sock.recv(1024)# Should be ready to read
            # for recv_a in recv_data:
            if recv_data:
                data.inb = recv_data
                self.out(data.inb.decode())
        else:
            # print('closing connection to', data.addr)
            self.sel.unregister(sock)
            sock.close()

def dealt(cmd):
    if cmd == 'OK':
        global last
        last = time()
        return
    global status
    status[cmd[2:]] = 1 if cmd.startswith('D_') else 0
    # print(cmd)
    # print(status)
    move = 1 if status.get('w') else (-1 if status.get('s') else 0)
    dire = 1 if status.get('d') else (-1 if status.get('a') else 0)
    
    speedL = 0
    speedR = 0
    if move:
        speedL = speed*move
        speedR = speed*move
        if dire:
            speedL += rotationRatio*speed*dire*move
            speedR -= rotationRatio*speed*dire*move
    else:
        if dire:
            speedL = -speed*dire
            speedR = speed*dire
    print(F'{speedL:5}{speedR:5}')
    return 'v', speedL, speedR

if __name__ == "__main__":
    micon = Micon()
    # micon.connect(force=1)
    micon.start()
    s = SOC(lambda cmd: micon.dealt(dealt(cmd)))
    s.start()
    while 1:
        if time()-last > timeout:
            last = time()
        else:
            sleep(0.1)
        pass
    pass