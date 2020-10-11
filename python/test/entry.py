from tkinter import *
import tkinter.font as TkFont
from threading import Thread
import menu

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

    def createWindows(self):
        self.root = Tk()
        self.root.title("樹莓派執行介面")
        self.root.geometry('480x320')
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
        self.change_menu(menu.menu)

    def change_menu(self, dist):
        self.menu = dist
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
        self.update()
    
    def update(self):
        self.display_list.delete(0, END)
        for i in range(5):
            if i + self.point > self.length-1:
                break
            self.display_list.insert(i, list(self.tmplist.keys())[self.point+i])
        self.display_list.selection_set(self.cursor)
        self.print(f'{self.length}, {self.point}, {self.cursor}')

    def run(self):
        self.createWindows()
        self.root.mainloop()

    def down(self):
        if self.cursor < 4:
            if self.cursor<self.length-1:
                self.cursor += 1
        elif self.point+4<self.length-1:
            self.point += 1
        self.update()

    def up(self):
        if self.cursor > 0:
            self.cursor -= 1
        elif self.point > 0:
            self.point -= 1
        self.update()

    def check(self):
        target = list(self.tmplist.keys())[self.point + self.cursor]
        if isinstance(self.tmplist[target], dict):
            self.path.append(target)
            self.reflash_list()
        else:
            if target == '返回':
                self.path.pop()
                self.reflash_list()
                return
            self.print(F'select: {target}, cmd: {self.tmplist[target]}')

    def home(self):
        self.path = []
        self.reflash_list()

    def print(self, text='', name='unknow'):
        msg = F'[{name:7}]' + text
        self.display_log.insert(END, msg)
        if self.display_log.size() > 3:
            self.display_log.delete(0)

if __name__ == "__main__":
    app = Entry()
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