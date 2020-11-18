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

def go_front(target,velocity,sight) :
    while 1 :
        if lidar.data[sight] * abs(math.cos(sight - 270)) >= target :
            micon.write(F'[V,{velocity},{velocity}]')
        else:
            break
        sleep(0.1)
    micon.write(F'[V,0,0]')

def go_back(target,velocity,sight) :
    while 1 :
        if lidar.data[sight] * abs(math.cos(sight - 270)) >= target :
            micon.write(F'[V,{-velocity},{-velocity}]')
        else:
            break
    micon.write(F'[V,0,0]')

if __name__ == "__main__":

    running = True
    while running == True:
        go_front(300,500,front-10)
        running = False