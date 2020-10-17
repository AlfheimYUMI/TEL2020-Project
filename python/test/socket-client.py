import socket
import time

HOST ='127.0.0.1'# 伺服器的主機名或者 IP 地址

PORT =12300# 伺服器使用的埠
start = time.time()
for i in range(100):
    time.sleep(0.1)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(bytes(F'test[{i}],client1234', 'utf-8'))
end = time.time()
print(F'{start}->{end}:  {end-start}')
