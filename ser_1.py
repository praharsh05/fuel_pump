import urllib.request, urllib.parse, urllib.error
import json
from config import *
import serial
import time


serviceurl = 'http://192.168.36.32:8001/mrn/?format=json'
print('Retrieving', serviceurl)
html = urllib.request.urlopen(serviceurl).read().decode()
js = json.loads(html)
print(json.dumps(js, indent=4))
ser = serial.Serial(com_port, baud_rate)
while(1):
    for item in js:
        mrn_no= item
        mrn_date= js[item]["mrm_date"]
        qty=js[item]["quantity"]
        vehicle= js[item]["vehicle"]
        send= vehicle + ' ' + qty
        send= send.encode()
        ser.write(send)
        time.sleep(3)
        print(send)
        # ser.close()
