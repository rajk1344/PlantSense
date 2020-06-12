import tkinter as tk
import tkinter.ttk as ttk
import serial.tools.list_ports

# --- functions ---


def serial_ports():
    arr = serial.tools.list_ports.comports()
    ports = []

    for item in arr:
        ports.append(item.device)

    return ports


def on_select(event=None):

    # get selection from event
    print("event.widget:", event.widget.get())

    # or get selection directly from combobox
    print("comboboxes: ", cb.get())
    ser = cb.get()
    print(ser)

# --- main ---


root = tk.Tk()

cb = ttk.Combobox(root, values=serial_ports())
cb.pack()

# assign function to combobox
cb.bind('<<ComboboxSelected>>', on_select)

root.mainloop()

# arr = serial.tools.list_ports.comports()

# for item in arr:
#     print(item.device)
