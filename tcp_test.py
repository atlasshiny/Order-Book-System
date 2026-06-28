import socket

# Connect to your running C++ Asio Server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(("127.0.0.1", 8080))

# A mock Market Buy string using actual SOH '\x01' bytes
fix_message = "8=FIX.4.2\x019=33\x0135=D\x0111=123\x0154=1\x0140=1\x0144=20\x0138=3\x0110=171\x01"

s.sendall(fix_message.encode('utf-8'))
s.close()
print("FIX message frame blasted successfully.")