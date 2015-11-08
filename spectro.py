# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

# -*- coding: utf-8 -*-
"""
Created on Wed Nov 04 22:15:47 2015

@author: Beat
"""

import serial
import matplotlib
ser = serial.Serial(
    port=raw_input('Please enter serial port: '),
    baudrate=115200,
    parity=serial.PARITY_ODD,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

#ser.open()
ser.isOpen()

print 'Opened serial port'

#get rid of incomplete dataset
while ser.read(1) != '\n' :
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
    
matplotlib.pyplot.plot(range(myValuesLength), myValuesInt)

ser.close();
