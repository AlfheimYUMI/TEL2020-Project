from tool import only
from myLidar import MyLidar
@only
class Planner:

    def __init__(self):
        self.lidar = MyLidar()
        self.lidar.connect()