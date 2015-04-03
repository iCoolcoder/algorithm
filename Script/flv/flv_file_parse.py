#!/usr/bin/python
from struct import *
from sys import exit
import sys
from os.path import basename
import os
import time
root_path = '/tmp/flv_tag_info/'
def showTagsInfo():
    nTag = 0
    while True:
        TagHeader = f.read(11)
  
        if not TagHeader:
            print ">>>>>>>>>>showTagsInfo-End<<<<<<<<<<"
            break
  
        print "\t<---Data Tag ",nTag," --->"
        nTag += 1
        print "\t\t <---Tag Header--->"
  
        tagType = unpack('B',TagHeader[0])
        if tagType[0] == 18:
            print "\t\t type:     ",tagType[0],"(script data)"
        elif tagType[0] == 9:
            print "\t\t type:     ",tagType[0],"(video tag)"
        elif tagType[0] == 8:
            print "\t\t type:     ",tagType[0],"(autio tag)"
        else:
            print ">>>>>>>>>>>>>showTagsInfo() tagType Error<<<<<<<<<<<<<<<<"
            exit() 
  
        dataSize = unpack('>I','\x00' + TagHeader[1:4])
        print "\t\t dataSize: ",dataSize[0]
  
        timeStamp = unpack('>I',TagHeader[4:8])
        print "\t\t timeStamp:",timeStamp[0]
  
        streamId = unpack('>I','\x00' + TagHeader[8:11])
        print "\t\t streamId :",streamId[0]
  
        print "\t\t <---meta Tag Data--->"
        metaTagData = f.read(dataSize[0])
        print "\t\t Tag Date len: ",len(metaTagData)
        preTagSize = f.read(4)
        nPreTagSize = unpack('>I',preTagSize)
        print "\t\t nPreTagSize :",nPreTagSize[0]," (",f.tell(),")"
        print "\t\t time: ", time.time()
        file_path = root_path + basename(f.name)
        fout=open(file_path + '/'+ str(int(time.time())) + '.' + str(nTag) + '.tag', 'w')
        fout.write(TagHeader + metaTagData + preTagSize)
        fout.close()
  
  
def showFileHeaderInfo():
    flvHeader = f.read(9)
    signature = unpack('ccc',flvHeader[0:3])
    version = unpack('b',flvHeader[3])
  
    strSignature = ''.join(signature)
    if strSignature == 'FLV':
        print "Flv structure"
        print "\t<---File Header--->"
        print "\tsignature:",strSignature
        print "\tversion:  ",version[0]
    else:
        print ">>>>>>>>>>>>>Not an FlV file<<<<<<<<<<<"
        exit()
     
   
    audioMask = 0x04
    videoMask = 0x01
 
    hasAudio = unpack('B',flvHeader[4])[0] & audioMask
    hasVideo = unpack('B',flvHeader[4])[0] & videoMask
  
    if hasAudio == audioMask:
        print "\thasAudio: ",hasAudio>>2
    if hasVideo == videoMask:
        print "\thasVideo: ",hasVideo
 
    headerSize = unpack('>I',flvHeader[5:9]) ##big-endian(network)
  
    print "\theaderSize:",headerSize[0]
  
    firstTagSize = f.read(4)
 
    file_path = root_path + basename(f.name)
    if not os.path.exists(file_path):
        os.makedirs(file_path)
    fout=open(file_path +'/' + basename(f.name) + '.meta', 'w')
    fout.write(flvHeader + firstTagSize)
    fout.close()
 
    nFirstTagSize = unpack('>I',firstTagSize)
    print "\tfirstTagSize:",nFirstTagSize[0]," (",f.tell(),")"
  
#main start
if len(sys.argv) < 2:
    print "Usage: slice.py [test.flv]"
    exit()
f = open(sys.argv[1],'r')
print "Start slice.py Open :",f.name
showFileHeaderInfo()
showTagsInfo()
f.close()