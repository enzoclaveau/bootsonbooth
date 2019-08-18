import serial
from pygame import mixer

ser = serial.Serial('/dev/ttyACM0', 9600)

mixer.init(channels=2)
mixer.music.load('../../music/Ardor.mp3')
mixer.music.play()


while 1:
    if ser.in_waiting > 0:
        line = ser.readline()
        line = line.strip()
        print(line)
        if line == '11':
            mixer.music.unpause()
        elif line == '10':
            mixer.music.pause()
