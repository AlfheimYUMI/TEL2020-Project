import matplotlib.pyplot as plt
from math import degrees, sin, cos, radians

PATH = '/'.join(__file__.split('/')[:-1])+'/'
tmp = []
with open(PATH+'data/'+'1600065119.dat', 'r') as f:
    for line in f:
        tmp.append(line.split())
print(tmp)
_, qualit, angle, distance = zip(*tmp)
angle = list(map(float, angle))
distance = list(map(float, distance))
lance = len(tmp)

def isLine(d1, d2, d3, deg=None):
    if not d2:
        d2 = 0.00001
    if not deg:
        if abs(d1+d3-d2-d2)/d2<0.005:
            return 1
        else:
            return 0

def r2xy(drs):
    tmp = []
    for data in drs:
        d, r = data
        tmp.append([cos(radians(d))*r, sin(radians(d))*r])
    return zip(*tmp)

tmp = [[], []]
x, y = r2xy(zip(angle, distance))
plt.subplot(121)
plt.plot(x, y)
plt.subplot(122)
for i in range(lance):
    j = i-1
    plt.plot()
    if isLine(distance[j-1], distance[j], distance[j+1]):
        tmp[0].append(x[j])
        tmp[1].append(y[j])
    else:
        print(tmp)
        if tmp[0]:
            plt.plot(tmp[0], tmp[1], 'r-', lw=1)
        tmp = [[], []]
plt.show()