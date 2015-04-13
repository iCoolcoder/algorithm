#!/usr/bin/python
# -*- coding: UTF-8 -*-

import logging
from os.path import basename, join

from zkclient import ZKClient, zookeeper, watchmethod

import os
import sys
import threading
import signal
import time

logging.basicConfig(
  level = logging.DEBUG,
  format = "[%(asctime)s] %(levelname)-8s %(message)s"
)

log = logging

class GJZookeeper(object):

  ZK_HOST = "localhost:2181"
  ROOT = "/Roles"
  WORKERS_PATH = join(ROOT, "agents")
  MASTERS_NUM = 1
  TIMEOUT = 10000

  def __init__(self, verbose = True):
    self.VERBOSE = verbose
    self.masters = []
    self.is_master = False
    self.path = None

    self.APP_ROOT = "/Applications"
    self.APP_CONF = join(self.APP_ROOT,"NginxConf")

    self.zk = ZKClient(self.ZK_HOST, timeout = self.TIMEOUT)
    self.say("login zookeeper successfully!")
    # init
    self.create_roles_znode()
    # register
    self.register()

  def create_roles_znode(self):
    """
    create the zookeeper node if not exist
    |-Roles
       |-workers
    """
    nodes = (self.ROOT, self.WORKERS_PATH)
    for node in nodes: 
      if not self.zk.exists(node):
        try:
          self.zk.create(node, "")
        except:
          pass

  @property
  def is_slave(self):
    return not self.is_master

  def register(self):
    """
    register a node for this worker,znode type : EPHEMERAL | SEQUENCE
    |-Roles
       |-workers
           |-worker000000000x      ==>>master
           |-worker000000000x+1    ==>>worker
           ....
    """
    self.path = self.zk.create(self.WORKERS_PATH + "/worker", "1", flags=zookeeper.EPHEMERAL | zookeeper.SEQUENCE)
    self.path = basename(self.path)
    self.say("I'm %s" % self.path)
    # check who is the master
    self.get_master()

  def get_master(self):
    """
    get children, and check who is the smallest child
    """
    @watchmethod
    def watcher(event):
      self.say("child changed, try to get master again.")
      self.get_master()

    children = self.zk.get_children(self.WORKERS_PATH, watcher)
    children.sort()
    self.say("%s's children: %s" % (self.WORKERS_PATH, children)) 

    # check if I'm master
    self.masters = children[:self.MASTERS_NUM]
    if self.path in self.masters:
      self.is_master = True
      self.say("I've become master!")
      self.create_app_znode()
    else:
      self.say("%s is masters, I'm slave" % self.masters)

  def create_app_znode(self):
    """
    create the zookeeper node if not exist
    |-Applications
          |-NginxConf
    """
    nodes = (self.APP_ROOT, self.APP_CONF)
    for node in nodes: 
      if not self.zk.exists(node):
        try:
          self.say("Create znode [%s] ..."%(node))
          self.zk.create(node, "")
        except:
          pass

  def create_conf_znode(self, data):
    """
    create the zookeeper node's children if not exist,contents is conf data
    |-Applications
          |-NginxConf
              |-item-000000000x => data
    """
    self.child_node = join(self.APP_CONF, "conf-")
    path = self.zk.create(self.child_node, data, flags=zookeeper.SEQUENCE)
    self.say("create znode %s"%path)

  def say(self, msg):
    """
    print messages to screen
    """
    if self.VERBOSE:
      if self.path:
        log.info("[ %s(%s) ] %s" % (self.path, "master" if self.is_master else "slave", msg))
      else:
        log.info(msg)

def start_zk_worker():
  """
    conntect to zookeeper for initalization
  """
  gj_zookeeper = GJZookeeper()
  th1 = threading.Thread(target = start_agent_worker, name = "thread_1", args = (gj_zookeeper,))

  th1.start()
  th1.join()

def start_agent_worker(gj_zookeeper):
  """
    listen config changed, parse command
  """
  import socket

  address = ('', 8877)
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # s = socket.socket()
  s.bind(address)
  s.listen(5)
  print "listen on port 8877 ..."

  while True:
    ss, addr = s.accept()
    print "receive connetcion from " ,addr
    content = ""
    while True:
      try :
        data = ss.recv(512)
        if not data:
          print "close connetcion " ,addr
          ss.close()
          break
        content = content + data
        print "receive message from %s : %s"%(addr[0],data)
      except Exception,e:
        print "receive error from %s : %s"%(addr[0],str(e))
        ss.close()
        break   
    parse_trigger_package(content, gj_zookeeper)

  s.close()

def parse_trigger_package(data,gj_zookeeper):
  try:
    cmd = data.split('|')[0]
    content = data.split('|')[1]
  except Exception, e:
    print "ERROR :",str(e)
    return 
  if cmd == "ADD":
    file_object = open("self_rules.xml")
    try:
      content = file_object.read()
    finally:
      file_object.close()
    print content
    gj_zookeeper.create_conf_znode(content)
  else:
    pass


def main():
  start_zk_worker()

if __name__ == "__main__":
  main()
  import time
  time.sleep(20000)