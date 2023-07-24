"""
_____________________________________________________
|   -Conexion de UDP con la base de datos en MySQL  |
|    -Crear archivo CSV                             |
|---------------------------------------------------|

"""
#Librerias
import sys
import paho.mqtt.client as mqtt
import pymysql
import csv
import socket
#Ajustables
file_name = "C:\\Users\\Carlos\\Documents\\UTFSM\\autitos\\software\\plataforma_movil\\codigos_actualizados\\velocidad_adaptativa_robot2_ds\\prueba_datos.csv"  # archivo csv

UDP_IP = "192.168.18.7" # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT = 5555
#UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
#creacion de archivo CSV
texto = open(file_name,'w')
texto.write('time_event,theta_ref,Input_theta,vel_ref,Input_vel,d_ref,Input_d'+'\n')

texto.close()

while True:
    data, addr = sock.recvfrom(4096) # buffer size is 1024 byte
    testo = str(data.decode('utf-8'))
    lista = testo.split(",")
    texto = open(file_name,"a")
    texto.write(testo+'\n')
    texto.close()
    print(testo)
    
