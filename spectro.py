# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

# -*- coding: utf-8 -*-
"""
Created on Wed Nov 04 22:15:47 2015

@author: Beat

/dev/cu.usbmodem14123 for red launchpad on mac
"""

import serial
import matplotlib.pyplot as plt
import time
from IPython import display
import pylab as pl
import numpy as np


ser = serial.Serial(
<<<<<<< HEAD
    port="/dev/cu.usbmodem14123",
=======
    port=raw_input('Please enter serial port: '),
    #port='COM11',
>>>>>>> 74eed0e3d2ea8526ab55a71ce02d6b4730ffe6bf
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

#ser.open()
ser.isOpen()


while 1:

<<<<<<< HEAD
    
    print 'Opened serial port' 
    
    #get rid of incomplete dataset
    while ser.read(1) != '\n' :
=======
#while 1 :
charIn = ser.read(1)
dataBuffer = charIn
while charIn != '\n' :
    charIn = ser.read(1)
    dataBuffer += charIn

#uncomment to save to file
#f = open('tempresults.csv', 'w')
#f.write(dataBuffer)
#f.close()

myValuesString = dataBuffer.split(',')
myValuesInt = []
myValuesLength = 0
for singleValue in myValuesString:
    try:
        myValuesInt.append(int(singleValue))
        myValuesLength+=1
    except:
>>>>>>> 74eed0e3d2ea8526ab55a71ce02d6b4730ffe6bf
        pass
    #while 1 :
    charIn = ser.read(1)
    dataBuffer = charIn
    while charIn != '\n' :
        charIn = ser.read(1)
        dataBuffer += charIn
    
    myValuesString = dataBuffer.split(',')
    myValuesInt = []
    myValuesLength = 0
  
    for singleValue in myValuesString:
        try:
            myValuesInt.append(int(singleValue))
           
            myValuesLength+=1
           
            
        except:
            pass
    
    
    print 'plotting'
    display.clear_output(wait=True)
    display.display(pl.gcf())
    #myValuesInt.reverse()
    
    plt.minorticks_on()
    plt.grid(color='r', linestyle='-', linewidth=1, which='both')
    plt.plot(map(lambda x: x/float(2),range(myValuesLength)), myValuesInt)
    
    #plt.xticks(np.arange(0, 750, 0.5))    
    
    plt.axis([150,1200,60,200])
    plt.show()
    
    time.sleep(1)
    plt.close()
    
    

ser.close()
    