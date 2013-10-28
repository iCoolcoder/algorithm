#coding:utf-8
import win32serviceutil
import win32service
import win32event
import logging
import os
import inspect
  
class PythonService(win32serviceutil.ServiceFramework):
    _svc_name_ = "PythonService"
    _svc_display_name_ = "PythonDisableBitLocker"
    _svc_description_ = "Python Service to stop BitLocker."
  
    def __init__(self, args):
        win32serviceutil.ServiceFramework.__init__(self, args)
        self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)
        self.logger = self._getLogger()
        self.isAlive = True
          
    def _getLogger(self):          
        logger = logging.getLogger('[PythonService]')
          
        this_file = inspect.getfile(inspect.currentframe())
        dirpath = os.path.abspath(os.path.dirname(this_file))
        handler = logging.FileHandler(os.path.join(dirpath, "service.log"))
          
        formatter = logging.Formatter('%(asctime)s %(name)-12s %(levelname)-8s %(message)s')
        handler.setFormatter(formatter)
          
        logger.addHandler(handler)
        logger.setLevel(logging.INFO)
          
        return logger
  
    def SvcDoRun(self):
        import time
        self.logger.error("svc do run....")
        while self.isAlive:
            self.logger.error("I am alive.")
            result = os.popen("sc query MBAMAgent").read()
            if 'RUNNING' in result:
                result = os.popen("sc stop MBAMAgent").read()
            time.sleep(10)
        # ???????
        #win32event.WaitForSingleObject(self.hWaitStop, win32event.INFINITE)   
              
    def SvcStop(self):
        # ???SCM??????
        self.logger.error("svc do stop....")
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
        # ????   
        win32event.SetEvent(self.hWaitStop)
        self.isAlive = False
  
if __name__=='__main__':
    win32serviceutil.HandleCommandLine(PythonService)
