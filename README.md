Fuel Pump Automation using Rf-ID

The repository contains the code for fuel dispensing against MRN raised in ERP.

First the python script gets the api of ERP and obtains the JSON then parses the it and gets the vehicle number and the quantity sanctioned in the ERP,
it then sends the data serially to the MCU (in this particular case is Arduino MEGA 2560).

The MCU stores the data and whenever the vehicle is made available to the dispenser the Rf-ID reader scans the vehicle Rf-ID tag fitted on the fuel tank lid
and authenticates.

The amount of fuel sanctioned is shown on the display module and the fuel pump operator can then see the maximum quantity to be dispensed.
