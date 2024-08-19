# Sofar_Modbus_Wifi
![Image](Sofar-HYD6000-ES.png)
![Image](panneau-photovoltaique.png)

Windows app to read Sofar Inverter HYD3000/6000-ES information  <br>
This sotfware has been develloped under C++ Builder 6 ( Borland ) <br><br>



# screenshot
![Image](ClientSofar2-a.png) <br> <br>
![Image](ClientSofar2-b.png) <br> <br>
![Image](ClientSofar2-c.png) <br> <br>

# Operation
This software ( ClientSofar2 ) requests information from the Sofar hybrid inverter via Wifi. <br>
The request consists of a Modbus frame, included in a special Sofar TCP frame. <br>
It returns various information as product power, load power, errors, etc ... <br>

# Using ClientSofar2
Specify your IP Inverter and clic "Connect" <br><br>
Clic on "Ask for Serial number Wifi Interface" <br><br>
Clic on "Read Word XXX ->"

# Other Sofar Inverters
This software has been tested on HYD6000-ES.<br><br>
It probably can function on all HYDXXX-ES and ME3000SP series.<br><br>
But it can't function on HYDXXX-EP series, because the addresses words are not the same.<br><br>
