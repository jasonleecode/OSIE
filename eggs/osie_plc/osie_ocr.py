"""
Implement a basic OCR app that can find countour sin image and
based on number of countours control a modbus server remotely.
"""

import cv2
import numpy as np
import pymodbus
from pymodbus.client.sync import ModbusTcpClient as ModbusClient
from time import sleep
from datetime import datetime
import os
import time

UNIT = 0x1
OSIE_PLC_ADDRESS = "localhost" # "192.168.0.48" for real machine
OSIE_PLC_PORT = 502
ROOT_FOLDER_PATH = "/mnt/flash"

def nothing(x):
    # any operation
    pass

def setConveyorState(state):
    """
    Send to modbus server command to start (state 1)
    or stop (state 0) conveyor.
    """
    client = ModbusClient(OSIE_PLC_ADDRESS, port=OSIE_PLC_PORT)
    client.connect()
    client.write_coils(0, [state], unit=UNIT)
    client.close()


def openAndCloseAirValve(seconds=0.05):
    """
    Send to modbus server command to open and close the air valve for
    a specified amount of time in seconds.
    """
    client = ModbusClient(OSIE_PLC_ADDRESS, port=OSIE_PLC_PORT)
    client.connect()
    #rr = client.read_coils(1, 1, unit=UNIT)
    client.write_coils(1, [True], unit=UNIT)
    sleep(seconds)
    client.write_coils(1, [False], unit=UNIT)
    client.close()

def storeFrame(frame):
    """
    Store a video frame in locally.
    """
    folder_path = "%s/%s" %(ROOT_FOLDER_PATH, datetime.today().strftime('%Y-%m-%d'))
    if not os.path.isdir(folder_path):
        os.mkdir(folder_path)
    millis = int(round(time.time() * 1000))
    file_path = "%s/%s.jpg" %(folder_path, millis)
    cv2.imwrite(file_path, frame)
    #f = open(file_path, "w")
    #f.write(file_content)
    #f.close()



# start conveyor
setConveyorState(1)

cap = cv2.VideoCapture(1)
cv2.namedWindow("Trackbars")
cv2.createTrackbar("L-H", "Trackbars", 0, 180, nothing)
cv2.createTrackbar("L-S", "Trackbars", 66, 255, nothing)
cv2.createTrackbar("L-V", "Trackbars", 134, 255, nothing)
cv2.createTrackbar("U-H", "Trackbars", 180, 180, nothing)
cv2.createTrackbar("U-S", "Trackbars", 255, 255, nothing)
cv2.createTrackbar("U-V", "Trackbars", 243, 255, nothing)

font = cv2.FONT_HERSHEY_COMPLEX

while True:
    _, frame = cap.read()
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    l_h = cv2.getTrackbarPos("L-H", "Trackbars")
    l_s = cv2.getTrackbarPos("L-S", "Trackbars")
    l_v = cv2.getTrackbarPos("L-V", "Trackbars")
    u_h = cv2.getTrackbarPos("U-H", "Trackbars")
    u_s = cv2.getTrackbarPos("U-S", "Trackbars")
    u_v = cv2.getTrackbarPos("U-V", "Trackbars")

    lower_red = np.array([l_h, l_s, l_v])
    upper_red = np.array([u_h, u_s, u_v])

    mask = cv2.inRange(hsv, lower_red, upper_red)
    kernel = np.ones((5, 5), np.uint8)
    mask = cv2.erode(mask, kernel)

    # Contours detection
    if int(cv2.__version__[0]) > 3:
        # Opencv 4.x.x
        contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    else:
        # Opencv 3.x.x
        _, contours, _ = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    for cnt in contours:
        area = cv2.contourArea(cnt)
        approx = cv2.approxPolyDP(cnt, 0.02*cv2.arcLength(cnt, True), True)
        x = approx.ravel()[0]
        y = approx.ravel()[1]

        if area > 400:
            cv2.drawContours(frame, [approx], 0, (0, 0, 0), 5)

            if len(approx) == 3:
                cv2.putText(frame, "Triangle", (x, y), font, 1, (0, 0, 0))
                storeFrame(frame)
                openAndCloseAirValve()

            elif len(approx) == 4:
                cv2.putText(frame, "Rectangle", (x, y), font, 1, (0, 0, 0))
                storeFrame(frame)
                openAndCloseAirValve()

            elif 10 < len(approx) < 20:
                cv2.putText(frame, "Circle-fire!", (x, y), font, 1, (0, 0, 0))
                storeFrame(frame)
                openAndCloseAirValve()

    cv2.imshow("Frame", frame)
    cv2.imshow("Mask", mask)

    key = cv2.waitKey(1)
    if key == 27:
        break

cap.release()
cv2.destroyAllWindows()

# stop conveyor
setConveyorState(0)


