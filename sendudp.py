from tkinter import *
import socket

UDP_IP_TX = "192.168.1.111"
UDP_PORT_TX = 1111

print("UDP target IP:", UDP_IP_TX)
print("UDP target port:", UDP_PORT_TX)


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP





class Window(Frame):

    def __init__(self, master=None):
        Frame.__init__(self, master)        
        self.master = master

        # widget can take all window
        self.pack(fill=BOTH, expand=1)

        # create buttons
        calibrarButton = Button(self, text="Calibrar", command=self.clickCalibrarButton)
        iniciarBtn = Button(self, text = "Iniciar", command=self.clickIniciarBtn)
        stopButton = Button(self, text = "Detener", command=self.clickStopButton)


        #Slider V
        labelVel=Label(self, text="Velocidad")
        self.sliderV = Scale(self, from_=10, to=25, orient="horizontal")
        self.sliderV.bind("<ButtonRelease-1>", self.updateValueV)
        self.sliderV.pack()

        #Slider D
        labelDist=Label(self, text="Distancia")
        self.sliderD = Scale(self, from_=10, to=25, orient="horizontal")
        self.sliderD.bind("<ButtonRelease-1>", self.updateValueD)
        self.sliderD.pack()
    
    

        #place buttons
        calibrarButton.place(x=0, y=0)
        iniciarBtn.place( x=0, y= 40)
        stopButton.place(x=0, y=80)
        labelVel.place(x=80, y=10)
        self.sliderV.place (x=150, y =10)
        labelDist.place(x=80, y=80)
        self.sliderD.place (x=150, y =80)
        

    def clickCalibrarButton(self):
        UDP_IP_TX = "192.168.1.108"
        UDP_PORT_TX = 1111

        MESSAGE = "E/calibrar/1"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)

        
        UDP_IP_TX = "192.168.1.111"
        UDP_PORT_TX = 1111

        MESSAGE = "E/calibrar/1"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)
  
        

    def clickIniciarBtn(self):
        MESSAGE = "E/parar/no"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)

    def clickStopButton(self):
        MESSAGE = "E/parar/si"
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)

    def updateValueV(self, event):
        MESSAGE = "E/cv_ref/" + str(self.sliderV.get())
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)
    
    def updateValueD(self, event):
        MESSAGE = "E/cd_ref/" + str(self.sliderD.get())
        sock.sendto(bytes(MESSAGE, "utf-8"), (UDP_IP_TX, UDP_PORT_TX))
        print("message:", MESSAGE)

                
        
root = Tk()
app = Window(root)
root.wm_title("RÜPÜ Controller")
root.geometry("320x200")
root.mainloop()
