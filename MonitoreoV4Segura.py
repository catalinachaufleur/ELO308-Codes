import tkinter as tk
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

####---------------------

UDP_IP_TX =""
UDP_PORT_TX = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

##---------------------------------------------------------
hostname=socket.gethostname()
IPAddr=socket.gethostbyname(hostname)
#print("Your Computer Name is:"+hostname)
#print("Your Computer IP Address is:"+IPAddr)

UDP_IP_RX = IPAddr # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT_RX = 1234

#UDP
sock_RX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_RX.bind((UDP_IP_RX, UDP_PORT_RX))
#---------------------------------------------------------
file_name = "monitoreo.csv"  # archivo csv
texto = open(file_name,'w')
#estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
 
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')

texto.close()

gData = []
gData.append([0])
gData.append([0])
j=0
#Configuramos la gr�fica
fig = plt.figure()
ax = fig.add_subplot(111)
hl, = plt.plot(gData[0], gData[1])
plt.ylim(0, 30)
plt.xlim(0,200)

#------------------------------------

class Window(tk.Tk):  # Heredar de tk.Tk para crear la ventana principal
    def __init__(self):
        super().__init__()

        self.title("RÜPÜ Controller")
        self.geometry("1200x1000")  # Tamaño de la ventana principal
        #self.attributes('-fullscreen', True)


        self.letras_sugeridas =["L","S","T"]  

        #IP Monitor          
        self.monitor_ip_label = tk.Label(self, text= "IP de este computador:")
        self.monitor_ip_label.grid(row=0, column=0, padx=5, pady=5)
        
        self.monitor_ip_entry = tk.Entry(self)
        self.monitor_ip_entry.grid(row=0, column=1, padx=5, pady=5)
        self.monitor_ip_entry.insert(tk.END,IPAddr)

        #Número de Robots        
        self.num_label = tk.Label(self, text="Ingresa el número de Robots")
        self.num_label.grid(row=1, column=0, padx=5, pady=5)

        self.entry_num = ttk.Combobox(self, values=list(range(1, 11)))
        self.entry_num.grid(row=1, column=1, padx=5, pady=5)
        self.entry_num.set("1") 

        self.submit_button = tk.Button(self, text="Ok", command=self.create_ip_entries)
        self.submit_button.grid(row=1, column=2, padx=5, pady=5)

        self.calibrar_button = tk.Button(self, text="Calibrar", command=self.clickCalibrarButton)
        self.monitor_button = tk.Button(self, text="Controlar", command=self.open_monitor_window)

        self.ip_entry_widgets = []  # Lista para mantener las casillas de texto de IPs y letras

        #crear lista de letras desplegable
        self.letras = []

        # Agregar una lista desplegable en la ventana de monitoreo
        self.selected_letter = tk.StringVar()
        self.letter_combobox = ttk.Combobox(self, textvariable=self.selected_letter, values=self.letras)

        self.labelRobot = tk.Label(self, text="Robot:")
        #setar en el Lider
        self.letter_combobox.set("L")

# Funciones ------------------------------------------

    def create_widgets(self):
        self.calibrar_button.grid(row=len(self.ip_entry_widgets) + 4, column=1, padx=5, pady=5)
        self.monitor_button.grid(row=len(self.ip_entry_widgets) + 4, column=2, padx=5, pady=5)

    def create_ip_entries(self):
        num_entries = int(self.entry_num.get())  
        self.create_ip_entry_widgets(num_entries)
        self.create_widgets()# Mostrar los botones despues de ok

    def create_ip_entry_widgets(self, n):
        #Se incluyen las letras sugeridas en las casillas
        sugerido=len(self.letras_sugeridas)
        for i in range(n-sugerido):
            self.letras_sugeridas.append("")

        for i in range(n):
            ip_label = tk.Label(self, text=f'IP {i + 1}:')
            ip_label.grid(row=i + 3, column=0, padx=5, pady=5)

            ip_entry = tk.Entry(self)
            ip_entry.grid(row=i + 3, column=1, padx=5, pady=5)
            ip_entry.insert(tk.END,'192.168.1.1')

            letter_label = tk.Label(self, text=f'Letra {i + 1}:')
            letter_label.grid(row=i + 3, column=2, padx=5, pady=5)
            
            letter_entry = tk.Entry(self)
            letter_entry.grid(row=i + 3, column=3, padx=5, pady=5)
            letter_entry.insert(tk.END,self.letras_sugeridas[i])


            self.ip_entry_widgets.append((ip_entry, letter_entry))# [(ip1,letra1),(ip2,letra2),...]

    def clickCalibrarButton(self):
         for i in reversed(range(len(self.ip_entry_widgets))):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            UDP_PORT_TX = 1111
            MESSAGE = "E/calibrar/1"
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:", UDP_IP_TX)
            tm.sleep(0.2)

            
    def clickIniciarButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print("message:", MESSAGE, "IP", self.ip_entry_widgets[0][0].get())
        tm.sleep(0.5)
        MESSAGE = "E/cv_ref/" + str(7)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))


    def clickStopButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:",UDP_IP_TX)


    def updateValueV(self, value,IP):
        UDP_IP_TX = IP
        UDP_PORT_TX = 1111
        
        MESSAGE = "E/cv_ref/" + str(value)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:",IP)
    
    def updateValueD(self, value, IP):
        UDP_IP_TX = IP
        UDP_PORT_TX = 1111
        
        MESSAGE = "E/cd_ref/" + str(value)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:",IP)

    
    def getIP(self,label):
        targetIP=""
        for entry in self.ip_entry_widgets:
            #print(entry[1].get(), "==", label)
            if entry[1].get() == label:
                targetIP = entry[0].get()
        #print(targetIP)
        return targetIP
    
    def GetData(self, out_data):
        while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            print(lista)
            texto = open(file_name, "a")
            texto.write(testo+'\n')
            texto.close()
            
            if lista[0] == 'L':
                out_data[1].append(float(lista[5]))
                if len(out_data[1]) > 200:
                    out_data[1].pop(0)


    def animate(self):
        def update_line(num, hl, data):
            hl.set_data(range(len(data[1])), data[1])
            return hl,
        
        fig = plt.figure()
        ax = fig.add_subplot(111)
        hl, = plt.plot([], [])
        plt.ylim(0, 30)
        plt.xlim(0, 200)
        


        line_ani = animation.FuncAnimation(fig, update_line, fargs=(hl, gData), interval=50, blit=False, cache_frame_data=False)
        plt.show()

        # Iniciar la obtención de datos y la animación en un hilo
        data_collector = threading.Thread(target=self.GetData, args=(gData,))
        data_collector.start()

    def start_monitoring(self):
        self.animate()

    #-----------------------
        
    def open_monitor_window(self):
        #if hasattr(self, 'monitor_window') and self.monitor_window.winfo_exists():
        #    return

        #monitor_window = tk.Toplevel(self)
        #monitor_window.title("Monitor")
        #monitor_window.geometry("800x600")  # Tamaño de la ventana del monitor


        # Crear una lista de letras para las opciones de la lista desplegable
        self.letras = [entry[1].get() for entry in self.ip_entry_widgets]
        self.letter_combobox = ttk.Combobox(self, textvariable=self.selected_letter, values=self.letras)
                
        # Posicionar los sliders y labels
        labelV = tk.Label(self, text="Velocidad")
        sliderV = tk.Scale(self, from_=10, to=30, orient="horizontal")
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get(),self.getIP(self.letter_combobox.get())))

        labelD = tk.Label(self, text="Distancia")
        sliderD = tk.Scale(self, from_=10, to=25, orient="horizontal")
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get(),self.getIP(self.letter_combobox.get())))

        iniciar_button_monitor = tk.Button(self, text="Iniciar", command=self.clickIniciarButton) #cambiar por sliderV.get()
        stop_button_monitor = tk.Button(self, text="Detener", command=self.clickStopButton)

        # Agregar un botón para iniciar el monitoreo y la animación
        start_monitor_button = tk.Button(self, text="Monitorear Señal", command=self.start_monitoring)
        start_monitor_button.grid(row=len(self.ip_entry_widgets) + 8, column=2, padx=10, pady=10)



       
        '''----------------Posicionar elementos en la App--------------''' 
        # Posicionar los botones 
        #calibrar_button_monitor.grid(row=4, column=0, padx=10, pady=10)
        iniciar_button_monitor.grid(row=len(self.ip_entry_widgets) + 8, column=1, padx=10, pady=10)
        stop_button_monitor.grid(row=len(self.ip_entry_widgets) + 8, column=0, padx=10, pady=10)
        
        
        empty_label = tk.Label(self, text="")
        empty_label.grid(row=len(self.ip_entry_widgets) + 3, columnspan=3)  # Ajusta la columna según tus necesidades

        
        self.labelRobot.grid(row=len(self.ip_entry_widgets) + 5, column=0, padx=10, pady=10) 
        self.letter_combobox.grid(row=len(self.ip_entry_widgets) + 5, column=1, padx=10, pady=10)

        #Sliders
        labelV.grid(row=len(self.ip_entry_widgets) + 6, column=0, columnspan=3, padx=10, pady=10)
        sliderV.grid(row=len(self.ip_entry_widgets) + 6, column=2, padx=10, pady=10)

        labelD.grid(row=len(self.ip_entry_widgets) + 7, column=0, columnspan=3, padx=10, pady=10)
        sliderD.grid(row=len(self.ip_entry_widgets) + 7, column=2, padx=10, pady=10)

app = Window()
app.mainloop()

