import socket
import time

host = '127.0.0.1'
port = 8080

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.settimeout(5)

try:
    client_socket.connect((host, port))
    print("Connection established!")
    
    message = "Hey Server!"
    client_socket.sendall(message.encode())
    print("Data sent!")
    data = client_socket.recv(1024)
    print(data)
except socket.timeout:
    print("Connection timed out.")
except socket.error as e:
    print(f"Connection failed: {e}")
finally:
    client_socket.close()
