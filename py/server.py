from socket import socket

def start_server():
    server_sock = socket()
    server_sock.bind(("localhost", 80))

    return server_sock

def accept_client(sock: socket):
    sock.listen(0)
    return sock.accept()

def get_file_content(file: str):
    try:
        fd = open(file)
    except OSError:
        return "File not found"

    buffer = fd.read()
    fd.close()

    return buffer

def recv_data(sock: socket, bufsize: int):
    return sock.recv(bufsize).decode()

def send_data(sock: socket, data: bytes):
    sock.send(data)

def main():
    print("Start server... ", end='')

    server_sock = start_server()

    print("OK\nWaiting client... ", end='')

    client_sock, address = accept_client(server_sock)

    print("Connected")

    while True:
        print("Waiting client request...")

        filename = recv_data(client_sock, 1024)

        print(f"Recv: {filename}\n")

        if filename == "exit":
            print("Disconnect...")
            break

        send_buffer = get_file_content(filename)

        send_data(
            client_sock,
            f"File: {filename}\n"
            f"Length: { len(send_buffer) }\n"
            f"Content:\n{ send_buffer }"
                .encode()
        )

    server_sock.close()
    client_sock.close()

    print("Exit...\n")

    return 0

if __name__ == "__main__":
    main()
