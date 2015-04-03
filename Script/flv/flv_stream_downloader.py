import socket
from struct import *
from sys import exit
import sys
from os.path import basename
import os
import time
import datetime
from decimal import *

request_string = '''GET /v1/o?a=98765&b=15247d69 HTTP/1.1\r\nContent-Length: 0\r\nHost: ip******:443\r\n\r\n'''
host = '******' #ip
port = 443

root_path = 'e:/tmp/flv_tag_info/'
def showFileHeaderInfo(buf):
    flvHeader = buf
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
  
    firstTagSize = unpack('>I', flvHeader[9:13])
    print "\tfirstTagSize:",firstTagSize[0]
    


def showTagsInfo(buf, start=0):
    cur_pos = start
    consumed_len = 0
    nTag = 0
    while True:
        TagHeader = buf[cur_pos:cur_pos+11]

        if not TagHeader or cur_pos+11 > len(buf):
            print ">>>>>>>>>>showTagsInfo-End<<<<<<<<<<"
            break
        cur_pos = cur_pos+11
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
        metaTagData = buf[cur_pos:cur_pos+dataSize[0]]
        if not metaTagData or cur_pos+dataSize[0] > len(buf):
            print ">>>>>>>>>>showTagsInfo-End<<<<<<<<<<"
            break
        cur_pos = cur_pos+dataSize[0]
        print "\t\t Tag Date len: ", len(metaTagData)
        preTagSize = buf[cur_pos:cur_pos+4]
        if not preTagSize or cur_pos+4 > len(buf):
            print ">>>>>>>>>>showTagsInfo-End<<<<<<<<<<"
            break
        cur_pos = cur_pos + 4

        nPreTagSize = unpack('>I', preTagSize)
        print "\t\t nPreTagSize :", nPreTagSize[0]
        print "\t\t time: ", time.time()
        consumed_len = cur_pos
        file_path = root_path + basename('info')
        if not os.path.exists(file_path):
            os.makedirs(file_path)
        fout=open(file_path +'/' + basename('info') + '.meta', 'a')
        if tagType[0] == 9 and nPreTagSize[0] >= 100:
            #fout.write("" + str(dataSize[0]) + "\t\tTime: " + str(datetime.datetime.now().microsecond) + "\n")
            fout.write("TagPayloadSize: %8d   Time: %s\n" %(dataSize[0], str(time.time())))
        fout.close()
    return consumed_len

# Connect to the server
s = socket.socket()
s.connect((host, port))
s.send(request_string)

print s.recv(1024)
start = 13
is_header_handled = False
consumed = 0
buf = ""
while True:
    recv_buf = s.recv(1024)
    buf = buf[consumed:len(buf)] + recv_buf
    print len(buf)
    print buf
    if not len(buf):
        break
    if not is_header_handled:
        showFileHeaderInfo(buf)
        is_header_handled = True
        consumed = showTagsInfo(buf, start)
    else:
        consumed = showTagsInfo(buf, 0)
    
