"""
    simple script that opens a listnens on a network port for udp messages
    use this to digonise messages being recieved
"""




import socket

HOST = "0.0.0.0"
PORT = 39394

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((HOST, PORT))

print(f"LISTENING ON {HOST}:{PORT}...")

try:
    while True:
        data, addr = sock.recvfrom(65535)
        print(f"\nRECEIVED {len(data)} BYTES FROM {addr}")
        print(data)
except KeyboardInterrupt:
    print("\nLISTENER STOPPED.")
finally:
    sock.close()