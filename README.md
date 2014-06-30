wmr
===

Output data from Oregon Scientific WMR9xx (WMR918, WMR928, WMR968 etc.) weather stations serial port to XML on a Raspberry Pi.

I made this stuff along time ago for an old XP box and now I got a Raspberry Pi I decided to port it.

It is a rather rudimentary implementation with a small footprint.

It will read the serial port provided and output the detected sensor data as XML. There are two outputs, on stdout you will get the XML element currently detected and in a file you will get the entire XML document.
You can use the XML document to play with XSLT on your web server, read it into a JavaScript, or load it from your mobile app to present the weather station data however you like.
The included Document Type Definition wmr.dtd contains the description of the potential output.

It supports the following sensors:
- Rain sensor
- Wind sensor
- Pressure/Temperature/Humidity (BaroThermoHygro or BTH)
- Temp/Humidity (TH)
- Temp (T)

as well as the station data itself. The WMR918 is capable of outputting:
- Clock (System time is used instead until it is detected)
- Battery status


This has been tested to run on Raspbian but compiling it on any Linux flavor should be fairly straightforward.

Build:
  make -k Makefile all
  
Deploy (Copies binary to your users bin directory):
  make -k Makefile deploy
  
Run:
  ./wx <serial port> <output file>
  
Example:
    ./wx /dev/ttyUSB0 /var/www/mywmr.xml

As an example of output you can see my weather station running on my Raspberry Pi here: http://raspberry.troest.com/current.xml
  
