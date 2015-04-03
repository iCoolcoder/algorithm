#!/usr/bin/python
# coding: utf-8
# zktest.py

import logging
from os.path import basename, join

from zkclient import ZKClient, zookeeper, watchmethod

logging.basicConfig(
    level = logging.DEBUG,
    format = "[%(asctime)s] %(levelname)-8s %(message)s"
)

log = logging

class GJZookeeper(object):

    ZK_HOST = "localhost:2181"
    ROOT = "/app"
    WORKERS_PATH = join(ROOT, "workers")
    MASTERS_NUM = 1
    TIMEOUT = 10000

    def __init__(self, verbose = True):
        self.VERBOSE = verbose
        self.path = None
        self.is_master = True

        self.zk = ZKClient(self.ZK_HOST, timeout = self.TIMEOUT)
        self.say("login ok!")
        # init
        self.__init_zk()
        # register
        self.register()

    def __init_zk(self):
        """
        create the zookeeper node if not exist
        """
        nodes = (self.ROOT, self.WORKERS_PATH)
        for node in nodes:
            if not self.zk.exists(node):
                try:
                    self.zk.create(node, "")
                except:
                    pass

    def register(self):
        @watchmethod
        def watcher(event):
            self.say("first watcher")
        self.path = self.zk.create(self.WORKERS_PATH + "/worker", "1", flags=zookeeper.EPHEMERAL | zookeeper.SEQUENCE)
        if not self.zk.exists(self.path, watcher):
            print "not exist"
        self.zk.set(self.path, "3", -1)
        self.zk.exists(self.path, watcher)
        self.zk.delete(self.path, -1)
        print "_________________________"

    def say(self, msg):
        """
        print messages to screen
        """
        if self.VERBOSE:
            if self.path:
                log.info("[ %s ] %s" % (self.path , msg))
            else:
                log.info(msg)

def main():
    gj_zookeeper = GJZookeeper()

if __name__ == "__main__":
    main()
    import time
    time.sleep(1000)