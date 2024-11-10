import requests
import socket
import atexit
import time

# Set the URL for the WordPress endpoint
WORDPRESS_API_URL = "https://webtesting.sk/wp-json/server-status/v1/status"

# Function to get the local IP address
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(0)
    try:
        # Connect to an external host to find the local IP (no data sent)
        s.connect(('10.254.254.254', 1))
        ip_address = s.getsockname()[0]
    except Exception:
        ip_address = '127.0.0.1'  # fallback to localhost
    finally:
        s.close()
    return ip_address

# Function to register server with WordPress
def register_server():
    ip_address = get_local_ip()
    data = {"ip": ip_address, "action": "start"}
    try:
        response = requests.post(WORDPRESS_API_URL, json=data)
        response.raise_for_status()
        print(f"Server registered with IP {ip_address}.")
    except requests.RequestException as e:
        print(f"Failed to register server: {e}")

# Function to unregister server when stopping
def unregister_server():
    data = {"action": "stop"}
    try:
        response = requests.post(WORDPRESS_API_URL, json=data)
        response.raise_for_status()
        print("Server unregistered.")
    except requests.RequestException as e:
        print(f"Failed to unregister server: {e}")

# Register the server when the script starts
register_server()

# Ensure the server unregisters on exit
atexit.register(unregister_server)

# Simulate the server running indefinitely
try:
    print("Server is running. Press Ctrl+C to stop.")
    while True:
        time.sleep(10)  # Server is idle
except KeyboardInterrupt:
    print("Stopping the server...")

