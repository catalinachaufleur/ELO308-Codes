import tkinter as tk
from tkinter import ttk

import socket
'''
##-------------------------

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import threading
#Librerias
import sys
import paho.mqtt.client as mqtt
import pymysql
import csv
'''
####---------------------

UDP_IP_TX =""
UDP_PORT_TX = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP


"""
##---------------------------------------------------------
UDP_IP = "192.168.1.101" # ip del computador que recibe datos (mismo que el que corre este script)
UDP_PORT = 1234

#UDP
sock_RX = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_RX.bind((UDP_IP, UDP_PORT))
#---------------------------------------------------------
file_name = "C:\\Users\\56975\\Documents\\Catalina\\ELO308-Codes\\ELO308-Codes\\monitoreo.csv"  # archivo csv
texto = open(file_name,'w')
#estado = "T,"+String(Input_d)+","+String(d_ref)+","+String(vel_ref)+","+String(Input_vel)+","+String(Input_theta)+","+String(Output_d)+","+String(Output_vel)+","+String(Output_theta);
 
texto.write('Robot,Delta_muestra,Input_d,d_ref,vel_ref,Input_vel,Input_theta,Output_d,Output_vel,Output_theta'+'\n')

texto.close()
"""

#------------------------------------

class Window(tk.Tk):  # Heredar de tk.Tk para crear la ventana principal
    def __init__(self):
        super().__init__()

        self.title("RÜPÜ Controller")
        self.geometry("800x600")  # Tamaño de la ventana principal

        self.num_label = tk.Label(self, text="Ingresa el número de Robots")
        self.num_label.grid(row=0, column=0, padx=5, pady=5)

        self.entry_num = ttk.Combobox(self, values=list(range(1, 11)))
        self.entry_num.grid(row=0, column=1, padx=5, pady=5)
        self.entry_num.set("1") 

        self.submit_button = tk.Button(self, text="Ok", command=self.create_ip_entries)
        self.submit_button.grid(row=0, column=2, padx=5, pady=5)

        self.calibrar_button = tk.Button(self, text="Calibrar", command=self.clickCalibrarButton)
        self.monitor_button = tk.Button(self, text="Monitorear", command=self.open_monitor_window)

        self.ip_entry_widgets = []  # Lista para mantener las casillas de texto de IPs y letras

    def create_widgets(self):
        self.calibrar_button.grid(row=len(self.ip_entry_widgets) + 2, column=1, padx=5, pady=5)
        self.monitor_button.grid(row=len(self.ip_entry_widgets) + 2, column=2, padx=5, pady=5)

    def create_ip_entries(self):
        num_entries = int(self.entry_num.get())  
        self.create_ip_entry_widgets(num_entries)
        self.create_widgets()# Mostrar los botones despues de ok

    def create_ip_entry_widgets(self, n):
        for i in range(n):
            ip_label = tk.Label(self, text=f'IP {i + 1}:')
            ip_label.grid(row=i + 2, column=0, padx=5, pady=5)

            ip_entry = tk.Entry(self)
            ip_entry.grid(row=i + 2, column=1, padx=5, pady=5)

            letter_label = tk.Label(self, text=f'Letra {i + 1}:')
            letter_label.grid(row=i + 2, column=2, padx=5, pady=5)

            letter_entry = tk.Entry(self)
            letter_entry.grid(row=i + 2, column=3, padx=5, pady=5)

            self.ip_entry_widgets.append((ip_entry, letter_entry))# [(ip1,letra1),(ip2,letra2),...]

    def clickCalibrarButton(self):
        for i in range(len(self.ip_entry_widgets)):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            UDP_PORT_TX = 1111
            MESSAGE = "E/calibrar/1"
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:", UDP_IP_TX)

            
    def clickIniciarButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP", self.ip_entry_widgets[0][0].get())


    def clickStopButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE, "IP:",IP)


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
            data, addr = sock.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            texto = open(file_name, "a")
            texto.write(testo+'\n')
            texto.close()
            
            if lista[0] == 'L':
                out_data[1].append(float(lista[5]))
                if len(out_data[1]) > 200:
                    out_data[1].pop(0)

# ... (continúa con el resto de tu código)

    def start_monitoring(self):
        # Crear una nueva ventana para la animación
        animation_window = tk.Toplevel(self)
        animation_window.title("Monitoreo de Velocidad")
        animation_window.geometry("800x600")  # Ajusta el tamaño según tus necesidades

        # Agregar una etiqueta para la animación (opcional)
        animation_label = tk.Label(animation_window, text="Velocidad:")
        animation_label.pack(padx=10, pady=10)

        # Configurar la gráfica para la animación
        fig = plt.figure()
        ax = fig.add_subplot(111)
        hl, = plt.plot([], [])
        plt.ylim(0, 30)
        plt.xlim(0, 200)

        def update_line(num, hl, data):
            hl.set_data(range(len(data[1])), data[1])
            return hl,

        line_ani = animation.FuncAnimation(fig, update_line, fargs=(hl, gData), interval=50, blit=False)

        plt.show()

        # Lanzar el hilo para la obtención de datos y la animación
        data_collector = threading.Thread(target=self.GetData, args=(gData,))
        data_collector.start()
    #-----------------------
        
    def open_monitor_window(self):
        monitor_window = tk.Toplevel(self)
        monitor_window.title("Monitor")
        monitor_window.geometry("800x600")  # Tamaño de la ventana del monitor


        # Crear una lista de letras para las opciones de la lista desplegable
        letras = [entry[1].get() for entry in self.ip_entry_widgets]

        # Agregar una lista desplegable en la ventana de monitoreo
        selected_letter = tk.StringVar()
        letter_combobox = ttk.Combobox(monitor_window, textvariable=selected_letter, values=letras)

        labelRobot = tk.Label(monitor_window, text="Escoja el robot(?)")

        #setar en el Lider
        letter_combobox.set(self.ip_entry_widgets[0][1].get())
        

        #botones de monitoreo
        #calibrar_button_monitor = tk.Button(monitor_window, text="Calibrar", command=self.clickCalibrarButton)
        iniciar_button_monitor = tk.Button(monitor_window, text="Iniciar", command=self.clickIniciarButton)
        stop_button_monitor = tk.Button(monitor_window, text="Detener", command=self.clickStopButton)

        # Posicionar los sliders y labels
        labelV = tk.Label(monitor_window, text="Velocidad")
        sliderV = tk.Scale(monitor_window, from_=10, to=30, orient="horizontal")
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get(),self.getIP(letter_combobox.get())))

        labelD = tk.Label(monitor_window, text="Distancia")
        sliderD = tk.Scale(monitor_window, from_=10, to=25, orient="horizontal")
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get(),self.getIP(letter_combobox.get())))

 
        # Agregar un botón para iniciar el monitoreo y la animación
        start_monitor_button = tk.Button(monitor_window, text="Monitorear Señal", command=self.start_monitoring)
        start_monitor_button.grid(row=4, column=1, padx=10, pady=10)

        # ... (otros elementos de la GUI, como botones, sliders, etc.)



       
        '''----------------Posicionar elementos en la App--------------''' 
        # Posicionar los botones 
        #calibrar_button_monitor.grid(row=0, column=0, padx=10, pady=10)
        iniciar_button_monitor.grid(row=0, column=0, padx=10, pady=10)
        stop_button_monitor.grid(row=0, column=1, padx=10, pady=10)
        
        labelRobot.grid(row=1, column=0, padx=10, pady=10) 
        letter_combobox.grid(row=1, column=1, padx=10, pady=10)

        #Sliders
        labelV.grid(row=2, column=0, columnspan=3, padx=10, pady=10)
        sliderV.grid(row=2, column=2, padx=10, pady=10)

        labelD.grid(row=3, column=0, columnspan=3, padx=10, pady=10)
        sliderD.grid(row=3, column=2, padx=10, pady=10)

def main():
    app = Window()
    app.mainloop()

if __name__ == "__main__":
    main()
