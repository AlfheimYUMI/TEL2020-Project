import socket
import time
try:
    import pigpio
    debug = 0
except ImportError:
    # print('Warning: pigio is NOT imported')
    debug = 0
    import mpigpio as pigpio

HOST ='127.0.0.1'# 伺服器的主機名或者 IP 地址

PORT = 12300  # 伺服器使用的埠

def send(text, name='unknow'):
    if debug:
        print(F'{text},{name}')
    else:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(bytes(F'{text},{name}', 'utf-8'))


if __name__ == "__main__":
    start = time.time()
    for i in range(100):
        time.sleep(1)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(bytes(F'test[{i}],client', 'utf-8'))
    end = time.time()
    print(F'{start}->{end}:  {end-start}')