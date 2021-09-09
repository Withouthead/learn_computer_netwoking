from socket import *
server_name = '81.70.181.13'
server_port = 12000
client_socket = socket(AF_INET, SOCK_STREAM)
client_socket.connect((server_name, server_port))
sentence = input("Please Input your lowercase sentence")
client_socket.send(sentence.encode())
modified_sentence = client_socket.recv(1024)
print("From Server ", modified_sentence)
