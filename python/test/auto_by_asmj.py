from myLidar import MyLidar
from micon import Micon
from time import sleep
import math

front = 270
back  = 90
left  = 0
right = 180
running = True

print('conn lidar')
lidar = MyLidar()
lidar.connect()
lidar.start()
print('conn micon')
micon = Micon()
micon.connect(force=1)
micon.start()

while running == True:
    go_front(30,500,front - 10)
    running = False

def go_front(target,velocity,sight) :
    done = True
    while done == False :
        if lidar.data[sight] * abs(math.cos(sight - 270)) >= target :
            micon.write(F'[V,%d,%d]' % (velocity,velocity))
        else:
            done = True
        sleep(0.1)
    micon.write(F'[V,0,0]')

def go_back(target,velocity,sight) :
    done = True
    while done == False :
        if lidar.data[sight] * abs(math.cos(sight - 270)) >= target :
            micon.write(F'[V,%d,%d]' % ( -1*velocity, -1*velocity))
        else:
            done = True
    micon.write(F'[V,0,0]')