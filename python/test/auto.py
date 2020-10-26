from myLidar import MyLidar
from micon import Micon
from time import sleep


print('conn lidar')
lidar = MyLidar()
lidar.connect()
lidar.start()
print('conn micon')
micon = Micon()
micon.connect(force=1)
micon.start()
while 1:
    sleep(0.1)
    print(lidar.data[270], lidar.data[269], lidar.data[271])
    if lidar.data[270]<20:
        print('stop')
        micon.dealt(('V', 0,0))
        continue
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
