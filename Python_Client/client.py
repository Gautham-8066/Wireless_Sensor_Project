import socket

# CONFIGURATION
ESP_IP = "-------"  # <--- CHANGE THIS to your ESP32's IP
PORT = 80                # Must match the port in Arduino code

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print(f"Connecting to ESP32 at {ESP_IP}...")

try:
    sock.connect((ESP_IP, PORT))
    print("Connected! Receiving Data:\n")
    
    while True:
        # Receive data (buffer size 1024 bytes)
        data = sock.recv(1024)
        if not data:
            break
        print(data.decode('utf-8').strip())

except ConnectionRefusedError:
    print("Could not connect. Check ESP32 IP address and ensure it is powered on.")
except KeyboardInterrupt:
    print("\nDisconnected.")
finally:
    sock.close()
