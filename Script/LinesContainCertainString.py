#!/usr/bin/python

def getWordCount(filePath, outputFilePath, word):
    file = open(filePath, 'r')
    outputfile = open(outputFilePath, 'w')
    while 1:
        line = file.readline()
        if not line:
            break
        if word in line:
            outputfile.write(line)
    return

if __name__ == '__main__':
    print getWordCount('824.log', '824lu', '10.105.20.72')
