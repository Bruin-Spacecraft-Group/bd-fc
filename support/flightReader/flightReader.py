from binascii import unhexlify
import struct

def parse(data):
    hData = data.hex()
    currentString = ''
    string = ''
    for i in range(0,len(hData),2):
        currentString = hData[i:i+2] + ' '
        string += currentString
    r = {}
    r['FLAGS'] = string[0:2]
    r['SD ADDRESS'] = string[3:14]
    r['SD ADDRESS'] = int(littleEndian(r['SD ADDRESS']), 16)
    r['CURRENT TIME'] = string[15:26]
    r['CURRENT TIME'] = int(littleEndian(r['CURRENT TIME']), 16)
    r['TRIGGER TIME'] = string[27:38]
    r['TRIGGER TIME'] = int(littleEndian(r['TRIGGER TIME']), 16)
    r['NFF DATA'] = string[39:638]
    r['NFF DATA'] = ''.join([chr(int(x, 16)) for x in (r['NFF DATA'].split())])
    r['SENSE DATA'] = string[639:662]
    r['SENSE DATA'] = littleEndianArr(r['SENSE DATA']);
    t = string[663:1526]
    t = littleEndianArr(t);
    r['ACCEL DATA'] = []
    r['GYRO DATA'] = []
    r['MAG DATA'] = []
    for i in range(0, 16):
        r['ACCEL DATA'].append([t[  i*9  ], t[i*9 + 1], t[i*9 + 2]])
        r['GYRO DATA'].append([t[i*9 + 3], t[i*9 + 4], t[i*9 + 5]])
        r['MAG DATA'].append([t[i*9 + 6], t[i*9 + 7], t[i*9 + 8]])

    r['FLOW DATA'] = string[1527:1532]
    r['FLOW DATA'] = int(littleEndian(r['FLOW DATA']), 16)
    r['NULL BYTE'] = string[1533:1535]

    return r

'''
Function littleEndianArr
Will take in a string of byte data that is meant to represent an array,
and output values within
NOTE: It does not breakout out to expected array index.
Rather it will parse the data type and output a list of the data in the given order
@param hexString: str, string of hex values
@param dataTypeSize: int, size of data type given
@return: list, list of strings that each are in little endian
'''

def littleEndianArr(hexString):
    # list of all the bytes in consecutive order
    byteList = hexString.split()
    l = []
    for i in range(0, len(byteList), 2):
    # i represents the # of elements that should be reversed
        l.append(struct.unpack('<h', bytes.fromhex(''.join(byteList[i:i+2])))[0])
    return(l)

'''
Function littleEndian
Will take in a string of hex values and output a string of hex values in little endian
@param hexString: str, a string of hex values
@return: str, hex values in little endian
'''
def littleEndian(hexString):
    leng = len(hexString)
    if(leng == 2):
        return hexString

    byteList = hexString.split()[::-1]
    allBytes = ''
    for i in byteList:
        allBytes += i
    return allBytes


'''
Function translateByte
Will translate a string of hex values to readable int values
@param hexString: str,list; a string of hex values
@param size: int; size of list, if str then set to size 1
@return: none
'''
def translateByteToInt(hexString,size=1):
    if(size == 1):
        print(int(hexString,16))
    elif(size > 1):
        for i in range(0,size):
            print(int(hexString[i],16))


'''
Function translateByte
Will translate a string of hex values to readable char values
@param hexString: str; a string of hex values
@return: none
'''
def translateByteToChar(hexString):
    print(unhexlify(hexString))
