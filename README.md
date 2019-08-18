Commando om windows map te laden in Linux (zorg dat je een map boven Windows-Share zit:
sudo mount.cifs //192.168.178.10/bootsonbooth Windows-Share

Vind uit hoe de aangesloten Arduino heet met volgend commando:
ls /dev/tty*

De naam van Arduino op de pi: /dev/ttyACM0 (het lijkt niet uit te maken welke usb port er gebruikt wordt).

Voor de native serial port op de Arduino het volgende object gebruiken: 
SerialUsB

Voor de programming serial port op de Arduino het volgende object gebruiken:
Serial

"C:\InstalledSoftware\Arduino\hardware\tools\avr\avr\include\avr\io.h"