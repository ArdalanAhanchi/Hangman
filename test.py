import socket

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("127.0.0.1", 21754))
#data = 1
data = 1
print(data)
s.sendall(bytes(data))
#print(1)
#data = s.recv(1)
print(data)
s.close()
