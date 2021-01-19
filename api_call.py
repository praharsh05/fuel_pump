import urllib.request, urllib.parse, urllib.error
import json
import ssl
serviceurl = 'http://192.168.36.32:8000/mrn/?format=json'
print('Retrieving', serviceurl)
html = urllib.request.urlopen(serviceurl).read().decode()
print('Retrieved', len(html), 'characters')
js = json.loads(html)
print(json.dumps(js, indent=4))
for item in js:
    print('mrn no: ', item)
    mrn_no= item
    mrn_no.rstrip("\"")
    mrn_no= mrn_no[3]
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
