# -*- coding: utf-8 -*-
from threading import Thread
from time import time, sleep
import selectors
import socket
import types

status = {}
last = time()
timeout = 0.5

class SOC(Thread):
    def __init__(self, output, host='127.0.0.1', port=12301):
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
    print(cmd)
    print(status)
    # if status[]

if __name__ == "__main__":
    s = SOC(dealt)
    s.start()
    while 1:
        if time()-last > timeout:
            last = time()
            # print('soc timeout')
            # stop
        else:
            
            sleep(0.1)
        pass
    pass