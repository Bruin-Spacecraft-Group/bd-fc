from binascii import unhexify

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
def littleEndianArr(hexString,dataTypeSize):
    byteList = hexString.split()
    dataBreakoutList = []
    leng = len(byteList)

    for i in range(0,int(leng/dataTypeSize)):
        currentData = []
        for j in range(0,dataTypeSize):
            currentData.append(byteList[i])
        currentData = currentData[::-1]
        dataBreakoutList.append(currentData)

    dataBreakoutString = []
    for i in dataBreakoutList:
        allBytes = ''.join(i)
        dataBreakoutString.append(allBytes)

    return dataBreakoutString

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
    print(unhexify(hexString))
