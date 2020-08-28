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

# init
port = ''
running = False
data = []


def animate(i):
    arr = []

    for x in data:
        arr.append([x[1], x[2], x[3], x[4]])

    a.clear()
    a.set_ylim(0, 150)
    a.plot(arr)


def scanning():
    if running:  # Only do this if the Stop button has not been clicked

        ser.write(b'g')
        arduinoData = ser.readline().decode('ascii')
        print(arduinoData)

        reader = csv.reader(arduinoData.split('\n'), delimiter=',')

        x1 = 0
        x2 = 0
        x3 = 0
        x4 = 0

        for row in reader:
            print(row)

            try:
                if row[0] != 'ovf':
                    x1 = float(row[0])
            except:
                pass

            try:
                if row[1] != 'ovf':
                    x2 = float(row[1])
            except:
                pass

            try:
                if row[2] != 'ovf':
                    x3 = float(row[2])
            except:
                pass

            try:
                if row[3] != 'ovf':
                    x4 = float(row[3])
            except:
                pass

        data.append([strftime("%d-%b-%Y-%H-%M-%S", gmtime()), x1, x2, x3, x4])

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

    with open(destination + '/' + strftime("%Y-%M-%d-%H-%M-%S", gmtime()) + '-plantSense_data.csv', 'w',  encoding='utf-8-sig', newline='') as csvfile:
        writer = csv.writer(csvfile)

        for point in data:
            writer.writerow(point)


def connect():
    global ser
    print(port)
    ser = serial.Serial(port, baudrate=9600, timeout=1)


def reset():
    global data
    data.clear()

style.use("dark_background")

f = Figure(figsize=(4, 4), dpi=100)
a = f.add_subplot(111)

window = Tk()
window.title("PlantSense")
window.geometry('800x500')

# labels
title_lbl = Label(window, text="PlantSense")
data_lbl = Label(window, text="Data Collection:")
connect_lbl = Label(window, text="Arduino COM Port:")
temp_lbl = Label(window, text="Temprature Readings:")

# buttons
start = Button(window, text="Start Scan", command=start)
stop = Button(window, text="Pause", command=stop)
save = Button(window, text="Save", command=save)
connect = Button(window, text="Connect", command=connect)
reset = Button(window, text="Reset", command=reset)

# combobox
cb = ttk.Combobox(window, values=serial_ports())

# graph
canvas = FigureCanvasTkAgg(f, master=window)
canvas.draw()

# row 1
title_lbl.grid(row=0, column=0, columnspan=5)
canvas.get_tk_widget().grid(row=1, column=0, rowspan=9)

# row 2
data_lbl.grid(row=2, column=1)
start.grid(row=3, column=1)
stop.grid(row=4, column=1)
save.grid(row=5, column=1)
reset.grid(row=6, column=1)

# row 3
connect_lbl.grid(row=7, column=1)
connect.grid(row=8, column=2)
cb.grid(row=7, column=2)

# row 4
temp_lbl.grid(row=9, column=1)

# assign function to combobox
cb.bind('<<ComboboxSelected>>', port_select)

window.after(1000, scanning)  # After 1 second, call scanning
ani = animation.FuncAnimation(f, animate, interval=1000)
window.mainloop() 