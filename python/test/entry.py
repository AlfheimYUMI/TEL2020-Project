from tkinter import *

root = Tk()

li = [1, 2, 3]
ls = Listbox(root)
for term in li:
    ls.insert(0, term)

Label(root, text='test').grid(row=1, column=1, rowspan=5, columnspan=1)
ls.grid(row=6, column=1, rowspan=1, columnspan=1, )
 
root.mainloop()