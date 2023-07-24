import socket
 
UDP_IP = "192.168.100.12"
UDP_PORT = 5555

sock = socket.socket(socket.AF_INET, # Internet
                    socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    Datos, addr = sock.recvfrom(4096) # buffer size is 1024 bytes
    
    #print("received message: %s" % data)
    #print((str(data)))
    Separacion=Datos.split()
    print(Separacion)   