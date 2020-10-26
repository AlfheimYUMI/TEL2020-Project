from myLidar import MyLidar
from micon import Micon
lidar = MyLidar()
lidar.connect()
lidar.start()
micon = Micon()
micon.connect(force=1)
micon.start()
while 1:
    m = lidar.get_angle(270)
    if abs(m)<0.006:
        micon.dealt(('V', 800,800))
    elif m>0:
        micon.dealt(('V', 500,-500))
    else:
        micon.dealt(('V', 500,-500))
