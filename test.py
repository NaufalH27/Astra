import socket
import time

# Define the listener's host and port
host = '127.0.0.1'
port = 8080

# Create a TCP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Set a timeout to avoid blocking indefinitely
client_socket.settimeout(5)

# Try connecting to the listener
try:
    client_socket.connect((host, port))
    print("Connection established!")
    
    # Send some data if connected
    message = "asdasda!"
    client_socket.sendall(message.encode())
    print("Data sent!")
    data = client_socket.recv(1024)
except socket.timeout:
    print("Connection timed out.")
except socket.error as e:
    print(f"Connection failed: {e}")
finally:
    client_socket.close()
