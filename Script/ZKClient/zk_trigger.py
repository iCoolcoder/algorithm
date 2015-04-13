#!/usr/bin/python
import socket  
import sys

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
sock.connect(('127.0.0.1', 8877))
str = sys.argv[1] + '|' + sys.argv[2]
sock.send(str)  
sock.close()