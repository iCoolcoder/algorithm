import wmi
import os
import sys
import platform
import time

def sys_version():  
    c = wmi.WMI () 
    #???????? 
    for sys in c.Win32_OperatingSystem(): 
        print("Version:%s" % sys.Caption.encode("UTF8"),"Vernum:%s" % sys.BuildNumber)
        print(sys.OSArchitecture.encode("UTF8"))#???32???64?? 
        print(sys.NumberOfProcesses) #??????????

def cpu_use(): 
    #5s???CPU???? 
    c = wmi.WMI() 
    while True: 
        for cpu in c.Win32_Processor(): 
             timestamp = time.strftime('%a, %d %b %Y %H:%M:%S', time.localtime()) 
             print '%s | Utilization: %s: %d %%' % (timestamp, cpu.DeviceID, cpu.LoadPercentage) 
             time.sleep(5)     
              
def disk(): 
    c = wmi.WMI ()    
    #?????? 
    for physical_disk in c.Win32_DiskDrive (): 
        for partition in physical_disk.associators ("Win32_DiskDriveToDiskPartition"): 
            for logical_disk in partition.associators ("Win32_LogicalDiskToPartition"): 
                print physical_disk.Caption.encode("UTF8"), partition.Caption.encode("UTF8"), logical_disk.Caption 
     
    #?????????? 
    for disk in c.Win32_LogicalDisk (DriveType=3): 
        print disk.Caption, "%0.2f%% free" % (100.0 * long (disk.FreeSpace) / long (disk.Size)) 
 
def network(): 
    c = wmi.WMI ()     
    #??MAC?IP?? 
    for interface in c.Win32_NetworkAdapterConfiguration (IPEnabled=1): 
        print "MAC: %s" % interface.MACAddress 
    for ip_address in interface.IPAddress: 
        print "ip_add: %s" % ip_address 
    print 
     
    #?????????? 
    for s in c.Win32_StartupCommand (): 
        print "[%s] %s <%s>" % (s.Location.encode("UTF8"), s.Caption.encode("UTF8"), s.Command.encode("UTF8"))  

    #????????? 
    for process in c.Win32_Process (): 
        print process.ProcessId, process.Name 


def main():
    sys_version()
    #cpu_use()
    disk()
    network()

if __name__ == '__main__':
    main()
    print(platform.system())


