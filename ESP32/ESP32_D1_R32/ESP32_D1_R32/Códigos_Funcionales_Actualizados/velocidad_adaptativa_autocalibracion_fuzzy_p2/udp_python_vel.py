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
file_name = "C:\\Users\\Carlos\\Documents\\UTFSM\\autitos\\software\\plataforma_movil\\codigos_actualizados\\robot_zero\\velocidad_adaptativa_autocalibracion_fuzzy_p2\\monitoreo.csv"  # archivo csv

UDP_IP = "192.168.18.7" # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT = 1234
#UDP
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
#creacion de archivo CSV
texto = open(file_name,'w')
texto.write('S,Input_d,d_ref,vel_ref,Input_vel,vel_md,vel_mi,curva,fuzzyOut,VELpwm_out,Input_theta,direccion,thethaPWM_out,E'+'\n')

texto.close()

while True:
    data, addr = sock.recvfrom(4096) # buffer size is 1024 byte
    testo = str(data.decode('utf-8'))
    lista = testo.split(",")
    if lista[0]=="S":
        data, addr = sock.recvfrom(4096) # buffer size is 1024 byte
        testo1 = str(data.decode('utf-8'))
        lista = testo1.split(",")
        if lista[len(lista)-1]=="E":
            texto = open(file_name,"a")
            texto.write(testo+testo1+'\n')
            texto.close()
            print(testo+testo1)
    
