import socket

host = '127.0.0.1'
port = 8080

message = "Hey server!"
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    client_socket.connect((host, port))
    while (True) :
        client_socket.sendall(message.encode())
        print("massage sent")
        data = client_socket.recv(1024)
        print(data)


except socket.error as e:
    print(f"Connection failed: {e}")
finally:
    client_socket.close()


