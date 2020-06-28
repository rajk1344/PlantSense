from time import time, ctime, gmtime, strftime
import time

import matplotlib
from matplotlib import style
import matplotlib.animation as animation
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (
    FigureCanvasTkAgg, NavigationToolbar2Tk)

import serial
import serial.tools.list_ports

import tkinter
from tkinter import *
from tkinter import filedialog
import tkinter.ttk as ttk

import csv

matplotlib.use("TkAgg")

port = ''

running = False
test = False

data = []


def animate(i):
    arr = []

    for x in data:
        arr.append([x[1], x[2]])

    a.clear()
    a.set_ylim(0, 50)
    a.plot(arr)

def scanning():
    if running:  # Only do this if the Stop button has not been clicked

        ser.write(b'g')
        arduinoData = ser.readline().decode('ascii')
        print(arduinoData)

        reader = csv.reader(arduinoData.split('\n'), delimiter=',')

        x1 = 0
        x2 = 0
        for row in reader:
            x1 = float(row[0])
            x2 = float(row[1])

        data.append([strftime("%d-%b-%Y-%H-%M-%S", gmtime()), x1, x2])

    window.after(1000, scanning)

def serial_ports():
    arr = serial.tools.list_ports.comports()
    ports = []

    for item in arr:
        ports.append(item.device)

    return ports

def port_select(event=None):
    global port
    port = cb.get()

def start():
    """Enable scanning by setting the global flag to True."""
    global running
    running = True

def stop():
    """Stop scanning by setting the global flag to False."""
    global running
    running = False

def save():
    destination = filedialog.askdirectory()
    t = time()
    with open(destination + '/' + strftime("%d-%b-%Y-%H-%M-%S", gmtime()) + '-tempdata.csv', 'w',  encoding='utf-8-sig', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for point in data:
            writer.writerow(point)

def connect():
    global ser
    print(port)
    ser = serial.Serial(port, baudrate=9600, timeout=1)

style.use("dark_background")

f = Figure(figsize=(4, 4), dpi=100)
a = f.add_subplot(111)

window = Tk()
window.title("PlantSense")
window.geometry('800x500') 

#labels
title_lbl = Label(window, text="PlantSense")
data_lbl = Label(window, text="Data Collection:")
connect_lbl = Label(window, text="Arduino COM Port:")

#buttons
start = Button(window, text="Start Scan", command=start)
stop = Button(window, text="Stop", command=stop)
save = Button(window, text="Save", command=save)
connect = Button(window, text="Connect", command=connect)

#combobox
cb = ttk.Combobox(window, values=serial_ports())

#snapping to grid
title_lbl.grid(row=0, column=0, columnspan = 4)
data_lbl.grid(row=1, column=1)
connect_lbl.grid(row=2, column=1)

start.grid(row=1, column=2)
stop.grid(row=1, column=3)
save.grid(row=1, column=4)

connect.grid(row=2, column=3)
cb.grid(row=2, column=2)

# assign function to combobox
cb.bind('<<ComboboxSelected>>', port_select)

canvas = FigureCanvasTkAgg(f, master=window)
canvas.draw()
canvas.get_tk_widget().grid(row=1, column=0, rowspan=2)

window.after(1000, scanning)  # After 1 second, call scanning
ani = animation.FuncAnimation(f, animate, interval=1000)
window.mainloop()
