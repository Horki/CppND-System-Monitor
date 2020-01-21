#ifndef SYS_INFO_H
#define SYS_INFO_H

#include <string>
#include <iostream>
#include <vector>
#include <ProcessParser.h>

class SysInfo {
private:
  std::vector<std::string>              lastCpuStats;
  std::vector<std::string>              currentCpuStats;
  std::vector<std::string>              coresStats;
  std::vector<std::vector<std::string>> lastCpuCoresStats;
  std::vector<std::vector<std::string>> currentCpuCoresStats;
  std::string                           cpuPercent;
  float                                 memPercent;
  std::string                           OSname;
  std::string                           kernelVer;
  long                                  upTime;
  int                                   totalProc;
  int                                   runningProc;
  int                                   threads;
public:
  SysInfo();
  void                     setAttributes();
  void                     setLastCpuMeasures();
  std::string              getMemPercent()    const;
  long                     getUpTime()        const;
  std::string              getThreads()       const;
  std::string              getTotalProc()     const;
  std::string              getRunningProc()   const;
  std::string              getKernelVersion() const;
  std::string              getOSName()        const;
  std::string              getCpuPercent()    const;
  void                     getOtherCores(int _size);
  void                     setCpuCoresStats();
  std::vector<std::string> getCoresStats()    const;
};

#endif
