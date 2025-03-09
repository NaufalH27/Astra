import socket
import threading
import time

host = '127.0.0.1'
port = 8080

message = "Hey server!"

def send_tcp_request(thread_id):
    """Send a single TCP request."""
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((host, port))
        print(f"Thread {thread_id}: Connection established!")
        client_socket.sendall(message.encode())
        data = client_socket.recv(1024)
        print(f"Thread {thread_id}: Received: {data.decode()}")
    except socket.timeout:
        print(f"Thread {thread_id}: Connection timed out.")
    except socket.error as e:
        print(f"Thread {thread_id}: Connection failed: {e}")
    finally:
        client_socket.close()

num_threads = 100000

threads = []
for i in range(num_threads):
    thread = threading.Thread(target=send_tcp_request, args=(i,))
    threads.append(thread)
    thread.start()

start = time.time()
for thread in threads:
    thread.join()

end = time.time()
length = end - start
print("All threads have completed in " + str(length))
