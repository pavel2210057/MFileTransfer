from socket import socket

def connect_local():
    client_sock = socket()

    client_sock.connect(("localhost", 80))

    return client_sock

def recv_data(sock: socket, bufsize: int):
    return sock.recv(bufsize).decode()

def send_data(sock: socket, data: bytes):
    sock.send(data)

def main():
    print("Connect to host... ", end='')

    client_socket = connect_local()

    print("OK")

    while True:
        resource = str(input("Input resource name: "))

        send_data(client_socket, resource.encode())

        if resource == "exit":
            print("Disconnect...")
            break

        print(f"\n\nRecv:\n{recv_data(client_socket, 1024)}\n")

    client_socket.close()

    print("Exit...\n")

    return 0

if __name__ == "__main__":
    main()