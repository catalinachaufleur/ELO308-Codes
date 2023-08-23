import tkinter as tk
from tkinter import ttk

import socket

UDP_IP_TX =""
UDP_PORT_TX = 0

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP

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
            print("message:", MESSAGE)

            
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
        print("message:", MESSAGE, "IP", self.ip_entry_widgets[0][0].get())


    def updateValueV(self, value):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111
        
        MESSAGE = "E/cv_ref/" + str(value)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)
    
    def updateValueD(self, value):
        UDP_IP_TX = self.ip_entry_widgets[0][0].get()
        UDP_PORT_TX = 1111
        
        MESSAGE = "E/cd_ref/" + str(value)
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)
    
        
    def open_monitor_window(self):
        monitor_window = tk.Toplevel(self)
        monitor_window.title("Monitor")
        monitor_window.geometry("400x300")  # Tamaño de la ventana del monitor

        #botones de monitoreo
        calibrar_button_monitor = tk.Button(monitor_window, text="Calibrar", command=self.clickCalibrarButton)
        iniciar_button_monitor = tk.Button(monitor_window, text="Iniciar", command=self.clickIniciarButton)
        stop_button_monitor = tk.Button(monitor_window, text="Detener", command=self.clickStopButton)


        # Posicionar los sliders
        sliderV = tk.Scale(monitor_window, from_=10, to=30, orient="horizontal")
        sliderV.pack(padx=10, pady=10)
        sliderV.bind("<ButtonRelease-1>", lambda event: self.updateValueV(sliderV.get()))


        sliderD = tk.Scale(monitor_window, from_=10, to=30, orient="horizontal")
        sliderD.pack(padx=10, pady=10)
        sliderD.bind("<ButtonRelease-1>", lambda event: self.updateValueD(sliderD.get()))

        
        # Posicionar los botones 
        calibrar_button_monitor.pack(padx=10, pady=10)
        iniciar_button_monitor.pack(padx=10, pady=10)
        stop_button_monitor.pack(padx=10, pady=10)



    



def main():
    app = Window()
    app.mainloop()

if __name__ == "__main__":
    main()


