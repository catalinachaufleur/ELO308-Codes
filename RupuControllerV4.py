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

#---------------------------------------------------------
file_name = "monitoreo.csv"  # archivo csv
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



class App(customtkinter.CTk):
    def __init__(self):
        super().__init__()
        self.protocol("WM_DELETE_WINDOW", self.on_closing)
        
        
        self.title("RÜPÜ Controller")
        self.geometry("700x800+0+0") 
        
        self.letras_sugeridas =["L","S","T","O"]
        
        #Ingresar IP Monitor          
        self.monitor_ip_label = customtkinter.CTkLabel(self, text= "IP Monitor:", fg_color="transparent")
        self.monitor_ip_label.grid(row=0, column=0, padx=5, pady=5)
        
        self.monitor_ip_entry = customtkinter.CTkEntry(self)
        self.monitor_ip_entry.grid(row=0, column=1, padx=5, pady=5)
        self.monitor_ip_entry.insert(customtkinter.END,IPAddr)
 
        #Ingresar Número de Robots
        self.num_label = customtkinter.CTkLabel(self, text="Número de robots")
        self.num_label.grid(row=1, column=0, padx=5, pady=5)
        
        self.entry_num = customtkinter.CTkComboBox(self, values=["1", "2","3","4","5","6"])
        self.entry_num.grid(row=1, column=1, padx=5, pady=5)
        self.entry_num.set("1") 
        
        #Botón Ok (Ingresa IP y número de robots)
        self.submit_button = customtkinter.CTkButton(self, text="Ok", command=self.create_ip_entries)
        self.submit_button.grid(row=1, column=2, padx=5, pady=5)
        
        #Lista que guarda IP y Label ingresada
        self.ip_entry_widgets = []
                
        #Botones Calibrar y Controlar
        self.calibrar_button = customtkinter.CTkButton(self, width = 80,text="Calibrar", command=self.clickCalibrarButton)
        self.controlar_button = customtkinter.CTkButton(self, width = 80,text="Controlar", command=self.controllView)

        #crear lista de letras desplegable
        self.letras_lista = []

        # Agregar una lista desplegable en la ventana de monitoreo
        self.selected_letter = customtkinter.StringVar()
        self.letter_combobox = customtkinter.CTkComboBox(self, values=self.letras_lista,variable=self.selected_letter )

        self.labelRobot = customtkinter.CTkLabel(self, text="Etiqueta robot:")
        self.letter_combobox.set("L")

        #switch pestaña de control
        self.switch_var = customtkinter.StringVar(value="off")
        
        
    #Función Cierre App
    def on_closing(self):
        if len(self.ip_entry_widgets)>0:
            self.clickStopButton()
        self.destroy()
    
    #Crea y guarda las entradas de IP y Label
    def create_ip_entries(self):
        #espacio vacío
        #label_vacia = customtkinter.CTkLabel(app, text="", fg_color="transparent")
        #label_vacia.grid(row=2, column=1, padx=5, pady=5)
        
        num_entries = int(self.entry_num.get())          
        sugerido=len(self.letras_sugeridas)
        for i in range(num_entries-sugerido):
            self.letras_sugeridas.append("")

        for i in range(num_entries):
            ip_label = customtkinter.CTkLabel(self, text=f'IP {i + 1}:')
            ip_label.grid(row=i + 3, column=0, padx=5, pady=5)

            ip_entry =  customtkinter.CTkEntry(self)
            ip_entry.grid(row=i + 3, column=1, padx=5, pady=5)
            ip_entry.insert(tk.END,'192.168.1.10')

            #letter_label = customtkinter.CTkLabel(self, text=f'Etiqueta {i + 1}:')
            #letter_label.grid(row=i + 3, column=2, padx=5, pady=5)
            
            letter_entry = customtkinter.CTkEntry(self)
            letter_entry.grid(row=i + 3, column=2, padx=5, pady=5)
            letter_entry.insert(tk.END,self.letras_sugeridas[i])

            label_vacia = customtkinter.CTkLabel(app, text="", fg_color="transparent")
            label_vacia.grid(row=len(self.ip_entry_widgets) + 4, column=1, padx=5, pady=5)
      
    
            self.ip_entry_widgets.append((ip_entry, letter_entry))# [(ip1,letra1),(ip2,letra2),...]
            
            self.create_widgets()
            
            
    #Posiciona botones Calibrar y Controlar
    def create_widgets(self):
          
        self.calibrar_button.grid(row=len(self.ip_entry_widgets) + 5, column=1, padx=5, pady=5)
        self.controlar_button.grid(row=len(self.ip_entry_widgets) + 5, column=2, padx=5, pady=5)    

    #Función botón Calibrar
    def clickCalibrarButton(self):
         for i in reversed(range(len(self.ip_entry_widgets))):
            UDP_IP_TX = self.ip_entry_widgets[i][0].get()
            UDP_PORT_TX = 1111
            MESSAGE = "E/calibrar/1"
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            #print("message:", MESSAGE, "IP:", UDP_IP_TX)
            tm.sleep(0.2)
            
    #Función Iniciar  
    def clickIniciarButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print("message:", MESSAGE, "IP", self.ip_entry_widgets[0][0].get())
        tm.sleep(0.5)
        #MESSAGE = "E/cv_ref/" + str(min_v)
        #sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

        for i in range(len(self.ip_entry_widgets)-1):
            UDP_IP_TX = self.ip_entry_widgets[i+1][0].get()
            dist =10
            MESSAGE = "E/cd_ref/" + str(dist)
            print(MESSAGE, UDP_IP_TX)
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

    # Función botón detener
    def clickStopButton(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111

        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print("message:", MESSAGE, "IP:",UDP_IP_TX)


    def updateValueV(self, value,IP):
        label = customtkinter.CTkLabel(app, text="    ", fg_color="transparent")
        label = customtkinter.CTkLabel(app, text=str(round(value)), fg_color="transparent")
        label.grid(row=len(self.ip_entry_widgets) + 9, column=3, columnspan=1, padx=10, pady=10)
        
        if IP != "ALL":
            UDP_IP_TX = IP
            UDP_PORT_TX = 1111
        
            MESSAGE = "E/cv_ref/" + str(round(value))
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:",IP)
        else:
            for i in range(len(self.ip_entry_widgets)):
                UDP_IP_TX = self.ip_entry_widgets[i][0].get()
                UDP_PORT_TX = 1111
        
                MESSAGE = "E/cv_ref/" + str(round(value))
                print("message:", MESSAGE, "IP:",UDP_IP_TX)

                #sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
               
        
    
    def updateValueD(self, value, IP):
        labelV_nu = customtkinter.CTkLabel(app, text="    ", fg_color="transparent")
        labelV_num = customtkinter.CTkLabel(app, text=str(round(value)), fg_color="transparent")
        labelV_num.grid(row=len(self.ip_entry_widgets) + 11, column=3, columnspan=1, padx=10, pady=10)
        
        if IP != "ALL":
            UDP_IP_TX = IP
            UDP_PORT_TX = 1111

            MESSAGE = "E/cd_ref/" + str(round(value))
            sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
            print("message:", MESSAGE, "IP:",IP)

        else:
            for i in range(len(self.ip_entry_widgets)):
                UDP_IP_TX = self.ip_entry_widgets[i][0].get()
                UDP_PORT_TX = 1111
            

                MESSAGE = "E/cd_ref/" + str(round(value))
                print("message:", MESSAGE, "IP:",UDP_IP_TX)
                #sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
                
                
    
    def getIP(self,label):
        targetIP=""
        if label != "ALL":
            for entry in self.ip_entry_widgets:
                if entry[1].get() == label:
                    targetIP = entry[0].get()
            return targetIP
        else:
            return "ALL"
    
    def GetData(self, out_data,dato,figure):

        while True:
            data, addr = sock_RX.recvfrom(4096)
            testo = str(data.decode('utf-8'))
            lista = testo.split(",")
            texto = open(file_name, "a")
            texto.write(testo+'\n')
            texto.close()
            print(self.letter_combobox.get())
            #figure.suptitle("Señal robot " + self.letter_combobox.get(), fontsize=16)
            
            if lista[0] == self.letter_combobox.get():
                out_data[1].append(float(lista[dato]))
                if len(out_data[1]) > 100:
                    out_data[1].pop(0)


    def animate_vel(self):
        
        fig, axes = plt.subplots(3, 1, figsize=(8, 12))
        #fig.suptitle('Señales de monitoreo robot '+ self.letter_combobox.get(), fontsize=14) 
        plt.subplots_adjust(top=0.9, hspace=0.5)  

        lines = []

        for ax in axes:
            line, = ax.plot([], [])
            lines.append(line)

        axes[0].set_title('Velocidad', fontsize=12)
        axes[0].set_xlim(0, 100)
        axes[0].set_ylim(0, 30)

        axes[1].set_title('Distancia Predecesor', fontsize=12)
        axes[1].set_xlim(0, 100)
        axes[1].set_ylim(0, 20)

        axes[2].set_title('Ángulo de inclinación', fontsize=12)
        axes[2].set_xlim(0, 100)
        axes[2].set_ylim(-1, 1)
        
        """
        for ax in axes:
            ax.set_ylim(0, 30)
            ax.set_xlim(0, 200)
        """
        

        def update_line(num, lines, data):
            for line, d in zip(lines, data):
                line.set_data(range(len(d[1])), d[1])
            return lines
       
        line_ani = animation.FuncAnimation(fig, update_line, fargs=(lines, [gData1, gData2, gData3]),interval=50, blit=True, cache_frame_data=False)

        dataCollector1 = threading.Thread(target=self.GetData, args=(gData1, 5,fig))
        dataCollector2 = threading.Thread(target=self.GetData, args=(gData2, 2,fig))
        dataCollector3 = threading.Thread(target=self.GetData, args=(gData3, 6,fig))

        dataCollector1.start()
        dataCollector2.start()
        dataCollector3.start()


        fig.canvas.manager.window.wm_geometry("+1000+0")
        plt.show()
        
        
        def on_close(event):
            '''dataCollector1.join()  # Esperar a que el hilo termine
            dataCollector2.join()
            dataCollector3.join()
            sys.exit(0)  # Salir del programa'''

        # Configurar el evento de cierre de la ventana
        fig.canvas.mpl_connect('close_event', on_close)
        
        
            

    def start_monitoring_vel(self):
        self.animate_vel()


    
    def vuelta1(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111
        
        MESSAGE = "E/cv_ref/" + str(round(10))
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        #print(UDP_IP_TX)


    def vuelta2(self):
        UDP_IP_TX =  self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111
        MESSAGE = "E/cv_ref/" + str(round(10))
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

        UDP_IP_TX = self.ip_entry_widgets[1][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(15))

        UDP_IP_TX = self.ip_entry_widgets[2][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(15))

        UDP_IP_TX = self.ip_entry_widgets[3][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(15))


    def vuelta3(self):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111
        MESSAGE = "E/cv_ref/" + str(round(20))
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))

        UDP_IP_TX  = self.ip_entry_widgets[1][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(10))

        UDP_IP_TX  = self.ip_entry_widgets[2][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(10)) 

        UDP_IP_TX = self.ip_entry_widgets[3][0].get()
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        MESSAGE = "E/cd_ref/" + str(round(10))

    

    def switch_event(self):
        print("switch toggled, current value:", self.switch_var.get())
        if self.switch_var.get() =="on":
            self.clickIniciarButton()
        else:
            self.clickStopButton()

    def testdemo(self):

        self.toplevel_window = ToplevelWindow(self)  # create window if its None or destroyed
        self.toplevel_window.focus()  # if window exists focus it
        

    def controllView(self):
        


        title = customtkinter.CTkLabel(app, text="Panel de Control", font =("Serif",18), fg_color="transparent",compound="center")
        
        self.letras_lista = [entry[1].get() for entry in self.ip_entry_widgets]
        if len(self.letras_lista)>1:
            self.letras_lista.append("ALL")
        self.letter_combobox = customtkinter.CTkComboBox(self, values=self.letras_lista,variable=self.selected_letter)

        switch = customtkinter.CTkSwitch(app, text="Estado Robot", command=self.switch_event,
                                 variable=self.switch_var, onvalue="on", offvalue="off")
       
        # Posicionar los sliders y labels
        labelV = customtkinter.CTkLabel(self, text="Velocidad")
        sliderV = customtkinter.CTkSlider(self, from_=min_v, to=max_v, orientation="horizontal",state="normal")
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get(),self.getIP(self.letter_combobox.get())))

        labelD = customtkinter.CTkLabel(self, text="Distancia")
        sliderD = customtkinter.CTkSlider(self, from_=min_d, to=max_d, orientation="horizontal")
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get(),self.getIP(self.letter_combobox.get())))

        
        iniciar_button_monitor = customtkinter.CTkButton(self, width = 60,text="Iniciar", command=self.clickIniciarButton) #cambiar por sliderV.get()
        stop_button_monitor = customtkinter.CTkButton(self, width = 60, text="Detener", command=self.clickStopButton)

        # Agregar un botón para iniciar el monitoreo y la animación
        monitor_button = customtkinter.CTkButton(self, text="Monitorear Señales", command=self.start_monitoring_vel)

        validar_button = customtkinter.CTkButton(self, text="Validar", command=self.testdemo)

        #Configuración del controlador PID
       
       
        """ Test videos"""
        v1_button = customtkinter.CTkButton(self, text="Vuelta 1", command=self.vuelta1)
        v2_button = customtkinter.CTkButton(self, text="Vuelta 2", command=self.vuelta2)
        v3_button = customtkinter.CTkButton(self, text="Vuelta 3", command=self.vuelta3)
        v1_button.grid(row=len(self.ip_entry_widgets) + 13, column=1,columnspan=1, padx=10, pady=10)
        v2_button.grid(row=len(self.ip_entry_widgets) + 13, column=2,columnspan=1, padx=10, pady=10)
        v3_button.grid(row=len(self.ip_entry_widgets) + 13, column=3,columnspan=1, padx=10, pady=10)
        
        
        
        
        
        '''----------------Posicionar elementos en la App--------------''' 
             
        empty_label = customtkinter.CTkLabel(self, text="")
        empty_label.grid(row=len(self.ip_entry_widgets) + 6, columnspan=1)  # Ajusta la columna según tus necesidades

        title.grid(row=len(self.ip_entry_widgets) + 7, column=1,columnspan=3, padx=10, pady=10)

        switch.grid(row=len(self.ip_entry_widgets) + 8, column=1, padx=10, pady=10)
        validar_button.grid(row=len(self.ip_entry_widgets) +8, column=2, padx=10, pady=10)
        
        
        #Sliders
        labelV.grid(row=len(self.ip_entry_widgets) + 10, column=1, columnspan=1, padx=10, pady=10)
        sliderV.grid(row=len(self.ip_entry_widgets) + 10, column=2, padx=10, pady=10)

        labelD.grid(row=len(self.ip_entry_widgets) + 11, column=1, columnspan=1, padx=10, pady=10)
        sliderD.grid(row=len(self.ip_entry_widgets) + 11, column=2, padx=10, pady=10)

    
        # Posicionar los botones 
        #iniciar_button_monitor.grid(row=len(self.ip_entry_widgets) + 8, column=0, padx=10, pady=10)
        #stop_button_monitor.grid(row=len(self.ip_entry_widgets) + 9, column=0, padx=10, pady=10)
        monitor_button.grid(row=len(self.ip_entry_widgets) + 12, column=1,columnspan=2, padx=10, pady=10)

class ToplevelWindow(customtkinter.CTkToplevel):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.geometry("400x300+1200+300")

        self.label = customtkinter.CTkLabel(self, text="Validación de Funcionamiento")
        self.label.pack(padx=20, pady=20)

        
       
       
customtkinter.set_default_color_theme("dark-blue")
customtkinter.set_appearance_mode("light")
app = App()
app.mainloop()