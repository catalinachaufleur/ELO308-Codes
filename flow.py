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

file_name = "monitoreo.csv"  # archivo csv
texto = open(file_name,'w')
#estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
 
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')
texto.close()


def GetData():

        while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            texto = open(file_name, "a")
            texto.write(testo+"u"+'\n')
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

#calibrar
for ip in reversed(robots):
            UDP_IP_TX = ip
            UDP_PORT_TX = 1111
            MESSAGE = "E/calibrar/1"
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:", UDP_IP_TX)
            tm.sleep(0.2)
            
#iniciar
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111
MESSAGE = "E/parar/no"
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        
for i in range(len(robots)-1):
    UDP_IP_TX = robots[i+1]
    dist =10
    MESSAGE = "E/cd_ref/" + str(dist)
    print(MESSAGE, UDP_IP_TX)
    sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

tm.sleep(0.5)
#etapa  1
UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(10))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


dataCollector1 = threading.Thread(target=GetData)
dataCollector1.start()
tm.sleep(5)


UDP_IP_TX = robots[0]
UDP_PORT_TX = 1111        
MESSAGE = "E/cv_ref/" + str(round(20))
print(MESSAGE)
sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

