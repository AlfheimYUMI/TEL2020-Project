from myLidar import MyLidar
from micon import Micon
from time import sleep
import math

front = 270
back  = 90
left  = 0
right = 180
print('conn lidar')
lidar = MyLidar()
lidar.connect()
lidar.start()
print('conn micon')
micon = Micon()
micon.connect(force=1)
micon.start()

def go_back(target,velocity,sight) :
    while 1 :
        if lidar.data[sight] * abs(math.cos(sight - 270)) >= target :
            micon.write(F'[V,{-velocity},{-velocity}]')
        else:
            break
    micon.write(F'[V,0,0]')

if __name__ == "__main__":
    sight    = front - 20  #偏右
    target   = 300
    velocity = 500
    while 1 :
        if lidar.data[sight] * abs(math.cos(math.radians(sight - 270))) >= target or lidar.data[sight] == 0:
            print(lidar.data[sight])
            micon.write(F'[V,{velocity},{velocity}]')
        else:
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)
    sleep(0.1)

    sight    = 360 - 10
    target   = 0.006
    velocity = 500
    delay    = 3
    micon.write(F'[V,{velocity},{-velocity}]')
    sleep(delay)
    while 1 :
        m = lidar.get_angle(sight)
        if m > target :
            print('right')
            micon.write(F'[V,{velocity},{-velocity}]')
        elif m < -target :
            print('left')
            micon.write(F'[V,{-velocity},{velocity}]')
        else :
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)
    sleep(0.1)

    sight    = 35  #偏右
    target   = 400
    velocity = 500
    while 1 :
        if lidar.data[sight] * abs(math.cos(math.radians(sight - 270))) >= target or lidar.data[sight] == 0:
            print(lidar.data[sight])
            micon.write(F'[V,{velocity},{velocity}]')
        else:
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)
    sleep(0.1)

    sight    = front
    target   = 0.006
    velocity = 500
    delay    = 3
    micon.write(F'[V,{-velocity},{velocity}]')
    sleep(delay)
    while 1 :
        m = lidar.get_angle(sight)
        if m > target :
            print('right')
            micon.write(F'[V,{velocity},{-velocity}]')
        elif m < -target :
            print('left')
            micon.write(F'[V,{-velocity},{velocity}]')
        else :
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)
    sleep(0.1)

    sight    = front
    target   = 200
    velocity = 500
    while 1 :
        if lidar.data[sight] * abs(math.cos(math.radians(sight - 270))) >= target or lidar.data[sight] == 0:
            print(lidar.data[sight])
            micon.write(F'[V,{velocity},{velocity}]')
        else:
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)

    sleep(0.1)
    sight    = front
    target   = 300
    velocity = 500
    while 1 :
        if lidar.data[sight] * abs(math.cos(math.radians(sight - 270))) <= target or lidar.data[sight] == 0:
            print(lidar.data[sight])
            micon.write(F'[V,{-velocity},{-velocity}]')
        else:
            micon.write(F'[V,0,0]')
            break
        sleep(0.1)
    sleep(0.1)













print(F'done')