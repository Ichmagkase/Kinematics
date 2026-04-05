import socket

HOST = '127.0.0.1'
PORT = 4242

print(f"Host: {HOST}, Port: {PORT}")

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
    while True:
        playerId = input("Enter a player ID: ")
        action = input("Enter an action: ")

        message = (playerId + "|" + action + "\n").encode("utf-8")

        s.sendto(message, (HOST, PORT))

        print(f"Sent message: {message}")
