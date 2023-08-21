import tkinter as tk
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import time as tm

import socket
import queue
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
j=0

min_v = 8
max_v = 30

min_d = 10
max_d = 20

data_queue1 = queue.Queue(maxsize=200)  # Puedes ajustar el tamaño máximo
data_queue2 = queue.Queue(maxsize=200)
data_queue3 = queue.Queue(maxsize=200)
#------------------------------------

class Window(tk.Tk):  # Heredar de tk.Tk para crear la ventana principal
    def __init__(self):
        super().__init__()
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        self.title("RÜPÜ Controller")
        self.geometry("800x800+0+0")  # Tamaño de la ventana principal
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
            #print("message:", MESSAGE, "IP:", UDP_IP_TX)
            tm.sleep(0.2)

            
    def clickIniciarButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print("message:", MESSAGE, "IP", self.ip_entry_widgets[0][0].get())
        tm.sleep(0.5)
        MESSAGE = "E/cv_ref/" + str(min_v)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

        for i in range(len(self.ip_entry_widgets)-1):
            UDP_IP_TX = self.ip_entry_widgets[i-1][0].get()
            MESSAGE = "E/cd_ref/" + str(min_d)
            print(MESSAGE, UDP_IP_TX)
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))



    def clickStopButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print("message:", MESSAGE, "IP:",UDP_IP_TX)


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
    
    def GetData(self, out_data,dato,figure):

        while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            texto = open(file_name, "a")
            texto.write(testo+'\n')
            texto.close()
            #print(self.letter_combobox.get())
            figure.suptitle("Señal robot " + self.letter_combobox.get(), fontsize=16)
            
            if lista[0] == self.letter_combobox.get():
                out_data.put(float(lista[dato])) #data_queue1.put(float(lista[5]))
                if out_data.qsize() > 200:
                    out_data.get()


    def animate_vel(self):
        
        fig, axes = plt.subplots(3, 1, figsize=(8, 12))
        #fig.suptitle('Señales de monitoreo robot '+ self.letter_combobox.get(), fontsize=14) 
        plt.subplots_adjust(top=0.9, hspace=0.5)  

        lines = []

        for ax in axes:
            line, = ax.plot([], [])
            lines.append(line)

        axes[0].set_title('Velocidad', fontsize=12)
        axes[0].set_xlim(0, 200)
        axes[0].set_ylim(0, 30)

        axes[1].set_title('Distancia Predecesor', fontsize=12)
        axes[1].set_xlim(0, 200)
        axes[1].set_ylim(0, 20)

        axes[2].set_title('Ángulo de inclinación', fontsize=12)
        axes[2].set_xlim(0, 200)
        axes[2].set_ylim(-1, 1)
        
        """
        for ax in axes:
            ax.set_ylim(0, 30)
            ax.set_xlim(0, 200)
        """
        

        def update_line(num, lines, data_queues):
            #Obtener los datos más recientes de las colas
            data1 = list(data_queues[0].queue)
            data2 = list(data_queues[1].queue)
            data3 = list(data_queues[2].queue)

            for line, d in zip(lines, [data1, data2, data3]):
                line.set_data(range(len(d)), d)
            return lines
       
        line_ani = animation.FuncAnimation(fig, update_line, fargs=(lines, [data_queue1, data_queue2, data_queue3]),interval=50, blit=True, cache_frame_data=False)

        dataCollector1 = threading.Thread(target=self.GetData, args=(data_queue1, 5,fig))
        dataCollector2 = threading.Thread(target=self.GetData, args=(data_queue2, 2,fig))
        dataCollector3 = threading.Thread(target=self.GetData, args=(data_queue3, 6,fig))

        dataCollector1.start()
        dataCollector2.start()
        dataCollector3.start()

        '''
        def on_close(event):
            dataCollector1.join()  # Esperar a que el hilo termine
            dataCollector2.join()
            dataCollector3.join()
            sys.exit(0)  # Salir del programa

        # Configurar el evento de cierre de la ventana
        fig.canvas.mpl_connect('close_event', on_close)
        '''
        fig.canvas.manager.window.wm_geometry("+800+0")
        plt.show()
        
        
            

    def start_monitoring_vel(self):
        self.animate_vel()

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
        sliderV = tk.Scale(self, from_=min_v, to=max_v, orient="horizontal")
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get(),self.getIP(self.letter_combobox.get())))

        labelD = tk.Label(self, text="Distancia")
        sliderD = tk.Scale(self, from_=min_d, to=max_d, orient="horizontal")
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get(),self.getIP(self.letter_combobox.get())))

        iniciar_button_monitor = tk.Button(self, text="Iniciar", command=self.clickIniciarButton) #cambiar por sliderV.get()
        stop_button_monitor = tk.Button(self, text="Detener", command=self.clickStopButton)

        # Agregar un botón para iniciar el monitoreo y la animación
        start_monitor_button = tk.Button(self, text="Monitorear Velocidad", command=self.start_monitoring_vel)
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

    def on_closing(self):
        if len(self.ip_entry_widgets)>0:
            self.clickStopButton()
        self.destroy()

app = Window()
app.mainloop()

