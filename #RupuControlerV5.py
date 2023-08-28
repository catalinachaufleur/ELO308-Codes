#RupuControlerV5

import tkinter as tk
import customtkinter as ctk

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

#---------------------------------------------------------
file_name = "monitoreo5.csv"  # archivo csv
texto = open(file_name,'w')
#estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
 
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')
texto.close()

min_v = 0
max_v = 30

min_d = 5
max_d = 25

gData1 = [[0], [0]]
gData2 = [[0], [0]]
gData3 = [[0], [0]]

