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
  WORKERS_PATH = join(ROOT, "workers")
  TIMEOUT = 10000
  RULE_PATH = "/opt/interlive/nginx/conf/self_rules.xml"

  def __init__(self, verbose = True):
    self.VERBOSE = verbose
    self.path = None

    self.APP_ROOT = "/Applications"
    self.APP_CONF = join(self.APP_ROOT,"NginxConf")

    self.zk = ZKClient(self.ZK_HOST, timeout = self.TIMEOUT)
    self.say("login zookeeper successfully!")
    # init
    self.create_roles_znode()
    # register
    self.register()
    self.watch_app_znode()

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

  def watch_app_znode(self):
    @watchmethod
    def watcher(event):
      self.say("first watcher")
      child_nodes = self.zk.get_children(self.APP_CONF)
      child_nodes.sort(reverse=True)
      data = self.zk.get(self.APP_CONF + "/" + child_nodes[0])
      self.say(data[0])
      file_object = open(self.RULE_PATH, 'w')
      try:
        file_object.write(data[0])
      finally:
        file_object.close()
      self.zk.get_children(self.APP_CONF, watcher)

    self.zk.get_children(self.APP_CONF, watcher)

  def say(self, msg):
    """
    print messages to screen
    """
    if self.VERBOSE:
      if self.path:
        log.info("[ %s ] %s" % (self.path, msg))
      else:
        log.info(msg)


def main():
  gj_zookeeper = GJZookeeper()


if __name__ == "__main__":
  main()
  import time
  time.sleep(20000)




