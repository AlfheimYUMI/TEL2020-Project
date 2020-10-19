from threading import Thread
from time import sleep
from tool import *
from tkinter import *
import tkinter.font as TkFont
import subprocess
import selectors
import socket
import types
import json
import os
try:
    import pigpio
    debug = 0
except ImportError:
    print('Warning: pigio is NOT imported')
    debug = 1
    import mpigpio as pigpio

PATH = '/'.join(__file__.split('/')[:-1]) + '/'
pycmd = 'python' if debug else 'python3'

class SOC(Thread):
    def __init__(self, output, host='127.0.0.1', port=12300):
        Thread.__init__(self)
        lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        lsock.bind((host, port))
        lsock.listen()
        lsock.setblocking(False)
        self.sel = selectors.DefaultSelector()
        self.sel.register(lsock, selectors.EVENT_READ, data=None)
        self.out = output
        self.out(F'on{host}:{port}', 'mainSOC')
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
                self.out(*data.inb.decode().split(','))
        else:
            # print('closing connection to', data.addr)
            self.sel.unregister(sock)
            sock.close()


@only
class Entry(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.showList = [1, 2, 3]
        self.daemon = 1
        self.menu = {}
        self.path = []
        self.tmplist = []
        self.point = 0
        self.cursor = 0
        self.length = 0
        self.log = []
        self.tick = {
            'home': 0,
            'up': 0,
            'down': 0,
            'check': 0,
        }
        self.subpid = []
        self.pinInit()

    def dealt(self, cmd, key):
        if isinstance(cmd, str):
            if cmd.startswith('fun_'):
                self.print(F'fun {cmd[4:]}', 'fun')
            elif cmd.startswith('run_'):
                self.print(F'run {cmd[4:]}', 'run')
            elif cmd.startswith('python_'):
                self._python(cmd[7:])
            elif cmd.startswith('kill_'):
                self._kill(cmd[5:])
            else:
                self.print(subprocess.check_output(cmd, timeout=100, shell=True), 'system')
            self.path.pop()
            self.reflash_list()
        elif key == '返回':
            self.path.pop()
            self.reflash_list()
        elif key == '結束':
            self.root.destroy()

    def _python(self, arg):
        self.print(F'run {arg}', 'menu')
        process = subprocess.Popen([F'{pycmd}', F'{PATH+arg}'], shell=False)
        self.subpid.append(types.SimpleNamespace(name=arg, process=process))
        self.print(self.subpid[-1].process.poll(), 'python')
        print(PATH + arg)
        
    def _kill(self, name):
        for index, pid in enumerate(self.subpid):
            if pid.name == name:
                pid.process.kill()
                self.print(self.subpid.pop(index).name, 'kill')
                break

    def pinInit(self, pin_home=26, pin_up=19, pin_down=13, pin_check=6):
        self._pi = pigpio.pi()
        self._pi.set_mode(pin_home, pigpio.INPUT)
        self._pi.set_mode(pin_up, pigpio.INPUT)
        self._pi.set_mode(pin_down, pigpio.INPUT)
        self._pi.set_mode(pin_check, pigpio.INPUT)
        self._pi.set_pull_up_down(pin_home, pigpio.PUD_DOWN)
        self._pi.set_pull_up_down(pin_up, pigpio.PUD_DOWN)
        self._pi.set_pull_up_down(pin_down, pigpio.PUD_DOWN)
        self._pi.set_pull_up_down(pin_check, pigpio.PUD_DOWN)
        self._pi.set_noise_filter(pin_home, 10000, 1)
        self._pi.set_noise_filter(pin_up, 10000, 1)
        self._pi.set_noise_filter(pin_down, 10000, 1)
        self._pi.set_noise_filter(pin_check, 10000, 1)
        self._pi.callback(pin_home, pigpio.EITHER_EDGE, self._home)
        self._pi.callback(pin_up, pigpio.RISING_EDGE , self.up)
        self._pi.callback(pin_down, pigpio.RISING_EDGE , self.down)
        self._pi.callback(pin_check, pigpio.RISING_EDGE , self.check)

    def createWindows(self):
        self.root = Tk()
        self.root.title("樹莓派執行介面")
        if debug:
            self.root.geometry('480x320')
        else:
            self.root.attributes("-fullscreen", True)
        self.root.config(bg='#000000')
        self.status_label = Label(self.root, text='init')
        self.status_label.place(relheight=0.1, relwidth=1, relx=0, rely=0)
        self.status_label.config(bd=0, bg='#1C2312', fg='#A9B4C2', font=TkFont.Font(family="Helvetica", size=30, weight="bold"))
        self.display_list = Listbox(self.root)
        self.display_list.config(bd=0, bg='#1C2312', fg='#A9B4C2', selectbackground='#7D98A1', selectforeground='#393E46', font=TkFont.Font(family="Helvetica", size=24))
        self.display_list.place(relheight=0.58, relwidth=1, relx=0, rely=0.1)
        self.display_log = Listbox(self.root)
        self.display_log.config(bd=0, bg='#1C2312', fg='#A9B4C2', selectbackground='#7D98A1', selectforeground='#393E46', font=TkFont.Font(family="Helvetica", size=20))
        self.display_log.place(relheight=0.32, relwidth=1, relx=0, rely=0.68)
        self.load_menu()

    def load_menu(self):
        with open(PATH+'menu.json', encoding='utf-8') as f:
            self.menu = json.load(f)
        self.change_menu()

    def load_python(self):
        self.menu["程式相關"]["執行其他程式"] = {"返回": None}
        os.chdir(PATH)
        for fname in os.listdir():
            if '.py' in fname:
                self.menu["程式相關"]["執行其他程式"][fname] = F"python_{fname}"

    def load_process(self):
        self.menu["程式相關"]["執行中程式"] = {"返回": None}
        for pid in self.subpid:
            self.menu["程式相關"]["執行中程式"][pid.name] = F"kill_{pid.name}"

    def change_menu(self):
        self.path = []
        self.tmplist = []
        self.point = 0
        self.reflash_list()

    def reflash_list(self):
        self.tmplist = self.menu
        for path in self.path:
            self.tmplist = self.tmplist[path]
        self.point = 0
        self.cursor = 0
        self.length = self.tmplist.__len__()
        self.status_label.config(text=self.path[-1] if self.path else 'MENU')
        self.update()
    
    def update(self):
        self.display_list.delete(0, END)
        for i in range(5):
            if i + self.point > self.length-1:
                break
            self.display_list.insert(i, list(self.tmplist.keys())[self.point+i])
        self.display_list.selection_set(self.cursor)

    def run(self):
        self.createWindows()
        self.soc = SOC(self.print)
        self.soc.start()
        self.root.mainloop()


    def down(self, *arg):
        if self.cursor < 4:
            if self.cursor<self.length-1:
                self.cursor += 1
        elif self.point+4<self.length-1:
            self.point += 1
        self.update()

    def up(self, *arg):
        if self.cursor > 0:
            self.cursor -= 1
        elif self.point > 0:
            self.point -= 1
        self.update()

    def check(self, *arg):
        target = list(self.tmplist.keys())[self.point + self.cursor]
        if isinstance(self.tmplist[target], dict):
            self.path.append(target)
            if target == '執行中程式':
                self.load_process()
            if target == '執行其他程式':
                self.load_python()
            self.reflash_list()
        else:
            self.dealt(self.tmplist[target], target)

    def _home(self, gpio, level, tick):
        if level == 1: # rising
            self.tick['home'] = tick
            # self.num1+=1
        elif level == 0: # falling
        # if self.num
            passTime = tick-self.tick['home']
            if passTime > 500000:
                pass  # long
            else:
                self.home()

    def home(self):
        self.path = []
        self.reflash_list()

    def print(self, text='', name='unknow'):
        msg = F'[{F"{name:7}"[:7]}]' + text.__str__()
        self.display_log.insert(END, msg)
        if self.display_log.size() > 3:
            self.display_log.delete(0)

app = Entry()
if not debug:
    app.run()
else:
    app.start()
    while 1:
        inn = input('>>>')
        if not inn:
            break
        if inn == 'w':
            app.up()
        elif inn == 's':
            app.down()
        elif inn == 'd':
            app.check()
        elif inn == 'a':
            app.home()