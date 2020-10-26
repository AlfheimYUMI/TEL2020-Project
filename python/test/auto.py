from myLidar import MyLidar
from micon import Micon
from time import sleep
lidar = MyLidar()
lidar.connect()
lidar.start()
micon = Micon()
micon.connect(force=1)
micon.start()
while 1:
    sleep(0.1)
    if lidar.data[270]<200:
        print('stop')
        break
    m = lidar.get_angle(270)
    if abs(m)<0.006:
        print('str')
        micon.dealt(('V', 800,800))
    elif m>0:
        print('right')
        micon.dealt(('V', 500,-500))
    else:
        print('left')
        micon.dealt(('V', 500,-500))
