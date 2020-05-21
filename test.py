import serial

import tkinter
from tkinter import *
from tkinter import filedialog

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
import matplotlib.animation as animation
from matplotlib import style

from time import time, ctime, gmtime, strftime

# ser = serial.Serial('COM9', baudrate = 9600, timeout = 1)

# def getValues():
    
#     ser.write(b'g')
#     arduinoData = ser.readline().decode('ascii')
#     return arduinoData


# while(1):

#     userInput = input('Get data point?')

#     if userInput == 'y':
#         print(getValues())


# import csv


ser = serial.Serial('COM9', baudrate = 9600, timeout = 1)

running = False

data = []

style.use("dark_background")

f = Figure(figsize=(8,6), dpi =100)
a = f.add_subplot(111)

def animate(i):
    arr = []
    for x in data:
        arr.append(x[1])

    a.clear()
    a.set_ylim(0, 50)
    a.plot(arr)


def scanning():
    if running:  # Only do this if the Stop button has not been clicked
        
        ser.write(b'g')
        arduinoData = ser.readline().decode('ascii')
        print(arduinoData)

        x = 0
        t = time()
        if arduinoData is not None:
            x =  float(arduinoData)
            t = time()
        
        data.append([strftime("%d-%b-%Y-%H-%M-%S", gmtime()),(((x/1023)*5) -.5)*100])
        print((((x/1023)*5) -.5)*100)

    # After 1 second, call scanning again (create a recursive loop)
    window.after(1000, scanning)

def start():
    """Enable scanning by setting the global flag to True."""
    global running
    running = True

def stop():
    """Stop scanning by setting the global flag to False."""
    global running
    running = False

def save():
    destination =  filedialog.askdirectory()
    t = time()
    with open(destination + '/'+ strftime("%d-%b-%Y-%H-%M-%S", gmtime()) + '-tempdata.csv', 'w',  encoding='utf-8-sig', newline = '') as csvfile:
        writer = csv.writer(csvfile)
        for point in data:
            writer.writerow(point)
    

window = Tk()
window.title("PlantSense")
window.geometry('800x800')
lbl = Label(window, text="PlantSense")
lbl.grid()

start = Button(window, text="Start Scan", command=start)
stop = Button(window, text="Stop", command=stop)
save = Button(window, text="Save", command=save)

start.grid()
stop.grid()
save.grid()


canvas = FigureCanvasTkAgg(f, master = window)
canvas.draw()
canvas.get_tk_widget().grid()

window.after(1000, scanning)  # After 1 second, call scanning
ani = animation.FuncAnimation(f, animate, interval = 1000)
window.mainloop()
