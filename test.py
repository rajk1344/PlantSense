from pyfirmata import Arduino, util
from pyfirmata.util import Iterator

import time

board = Arduino('COM9') #Select the correct port
it = util.Iterator(board)
it.start()

while True:
    val = board.analog[0].read()
    board.analog[0].enable_reporting()
    
    x = 0
    
    if val is not None:
        x =  float(val)
    
    print(((x*5) -.5)*100)
    time.sleep(0.5)