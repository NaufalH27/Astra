import socket
import time

host = '127.0.0.1'
port = 8080

message = "Hey server!"
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client_socket.connect((host, port))
    client_socket.sendall(message.encode())
    print("massage sent")
    while (True) :
        time.sleep(1)

except socket.error as e:
    print(f"Connection failed: {e}")
finally:
    client_socket.close()


