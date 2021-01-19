import urllib.request, urllib.parse, urllib.error
import json
from config import *
import serial
import time


serviceurl = 'http://192.168.36.32:8000/mrn/?format=json'
print('Retrieving', serviceurl)
html = urllib.request.urlopen(serviceurl).read().decode()
print('Retrieved', len(html), 'characters')
js = json.loads(html)
print(json.dumps(js, indent=4))
for item in js:
    print('mrn no: ', item)
    mrn_no= item
    print(mrn_no)
    print('mrn date: ',js[item]["mrm_date"])
    mrn_date= js[item]["mrm_date"]
    print(mrn_date)
    print('qty: ',js[item]["quantity"])
    qty=js[item]["quantity"]
    print(qty)
    print('vehicle: ',js[item]["vehicle"])
    vehicle= js[item]["vehicle"]
    print(vehicle)
ser = serial.Serial(com_port, baud_rate)
ser.write(vehicle)
time.sleep(50)
ser.write(qty)
time.sleep(50)
ser.write(mrn_no)
