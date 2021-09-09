from socket import *
server_name = '81.70.181.13'
server_port = 12000
client_socket = socket(AF_INET, SOCK_DGRAM)
message = input("Please Input lowercase sentence:")
client_socket.sendto(message.encode(), (server_name, server_port))
modified_message, server_address = client_socket.recvfrom(2048)
print(modified_message.decode())
print(server_address)
client_socket.close()