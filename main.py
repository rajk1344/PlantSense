import csv

from pyfirmata import Arduino, util
from pyfirmata.util import Iterator

import tkinter
from tkinter import *

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from matplotlib.figure import Figure
import matplotlib.animation as animation
from matplotlib import style

import time

destination = 'test.csv'

board = Arduino('COM9') #Select the correct port
it = util.Iterator(board)
it.start()

running = False

data = []

style.use("ggplot")

f = Figure(figsize=(5,5), dpi =100)
a = f.add_subplot(111)

def animate(i):
    a.clear()
    a.plot(data)


def scanning():
    if running:  # Only do this if the Stop button has not been clicked
        
        board.analog[0].enable_reporting()
        val = board.analog[0].read()

        x = 0
        
        if val is not None:
            x =  float(val)
        
        data.append(((x*5) -.5)*100)
        print(((x*5) -.5)*100)

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


window = Tk()
window.title("PlantSense")
window.geometry('500x500')
lbl = Label(window, text="PlantSense")
lbl.grid(column=0, row=0)

start = Button(window, text="Start Scan", command=start)
stop = Button(window, text="Stop", command=stop)

start.grid()
stop.grid()

canvas = FigureCanvasTkAgg(f, master = window)
canvas.draw()
canvas.get_tk_widget().grid()

window.after(1000, scanning)  # After 1 second, call scanning
ani = animation.FuncAnimation(f, animate, interval = 1000)
window.mainloop()
