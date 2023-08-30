import tkinter as tk
import customtkinter

from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time as tm

import socket

##-------------------------
import matplotlib.pyplot as plt #descargar
import matplotlib.animation as animation
import threading 
#Librerias
import sys
import paho.mqtt.client as mqtt #descargar
import pymysql #descargar
import csv

UDP_IP_TX =""
UDP_PORT_TX = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

##---------------------------------------------------------
hostname=socket.gethostname()
IPAddr=socket.gethostbyname(hostname)

UDP_IP_RX = IPAddr # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT_RX = 1234

#UDP
sock_RX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_RX.bind((UDP_IP_RX, UDP_PORT_RX))

intento=5

file_name = "intento"+str(intento)+".csv"  # archivo csv
texto = open(file_name,'w')
    #estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
    
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')
texto.close()
                
    


def GetData(file_name,):

    while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            texto = open(file_name, "a")
            data, addr = sock_RX.recvfrom(4096)
            texto.write(testo+'\n')
            texto.close()

            

               
            #figure.suptitle("Señal robot " + self.letter_combobox.get(), fontsize=16)
 

"""
robots=[]
n_robots=int(input("Número de robots: "))

for i in range(n_robots):
    ip=input("Ingrese IP")
    robots.append(ip)
"""
robots=["192.168.1.105","192.168.1.104","192.168.1.106"]
calibrar=input("¿Calibrar? (S/N)")

if (calibrar == "S"):
    #calibrar
    for ip in reversed(robots):
        UDP_IP_TX = ip
        UDP_PORT_TX = 1111
        MESSAGE = "E/calibrar/1"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:", UDP_IP_TX)
        tm.sleep(0.2)

    input("Calibración Completa")
     
######INICIAR
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111
MESSAGE = "E/parar/no"
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


######ALINEAR            
for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =10
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


tm.sleep(5)

#iniciar monitoreo
dataCollector1 = threading.Thread(target=GetData, args=(file_name,))
dataCollector1.start()

#####################################ETAPA 1####################################################
#velocidad 10, distancia 10
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(10))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

  
tm.sleep(10) 

#####################################ETAPA 2####################################################
#velocidad 20, distancia 10
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(20))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =10
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

#DURACIÓN ETAPA 2
tm.sleep(10) 


#####################################ETAPA 3####################################################
#velocidad 10, distancia 20
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(20))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =15
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


#DURACIÓN ETAPA 3
tm.sleep(10) 

#####################################ETAPA 4####################################################
#velocidad 10, distancia 5
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(10))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =5
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


#DURACIÓN ETAPA 4
tm.sleep(10) 
#####################################ETAPA 4####################################################
#velocidad 20, distancia 5
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(20))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =5
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


#DURACIÓN ETAPA 4
tm.sleep(10) 


#####################################ETAPA 4####################################################
#velocidad 10, distancia 5
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(20))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =10
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


#DURACIÓN ETAPA 4
tm.sleep(10) 


######################DETENER
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111
MESSAGE = "E/parar/si"
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
