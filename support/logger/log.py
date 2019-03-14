import serial
import time
import serial.tools.list_ports

all_ports = [p.device for p in serial.tools.list_ports.comports()]

for i, e in enumerate(all_ports):
    print("{}: {}".format(i, e))

port = input('Enter Arduino Port: ')

if not port.isnumeric() or int(port) > len(all_ports):
    exit(1)

port = serial.Serial(all_ports[int(port)], 115200, dsrdtr=True, rtscts=True)

with open(('log_'+time.ctime()).translate(str.maketrans(' ', '_')), 'w', encoding='utf-8') as f:
    while(True):
        string = port.readline().decode('utf-8')
        f.write(string)
        print(string, end='')
