#ifndef PROCESS_PARSER_H
#define PROCESS_PARSER_H

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <ctime>
#include <unistd.h>
#include <vector>

#include <constants.h>
#include <util.h>

static float getSysActiveCpuTime(const std::vector<std::string>&);
static float getSysIdleCpuTime(const std::vector<std::string>&);

class ProcessParser {
private:
  std::ifstream stream;
public:
  static std::string getCmd(const std::string & pid);
  static std::vector<std::string> getPidList();
  static std::string getVmSize(const std::string & pid);
  static std::string getCpuPercent(const std::string & pid);
  static long int getSysUpTime();
  static std::string getProcUpTime(const std::string & pid);
  static std::string getProcUser(const std::string & pid);
  static std::vector<std::string> getSysCpuPercent(const std::string & coreNumber = "");
  static float getSysRamPercent();
  static std::string getSysKernelVersion();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static std::string getOSName();
  static std::string PrintCpuStats(const std::vector<std::string> & values1, const std::vector<std::string> & values2);
  static bool isPidExisting(const std::string & pid);
  // This one is missing
  static int getNumberOfCores();
};

// Special
// values from Sys Cpu Percent
static float getSysActiveCpuTime(const std::vector<std::string>& v) {
  return (stof(v[S_USER]) +
          stof(v[S_NICE]) +
          stof(v[S_SYSTEM]) +
          stof(v[S_IRQ]) +
          stof(v[S_SOFTIRQ]) +
          stof(v[S_STEAL]) +
          stof(v[S_GUEST]) +
          stof(v[S_GUEST_NICE]));
}
// values from Sys Cpu Percent
static float getSysIdleCpuTime(const std::vector<std::string> &v) {
  return (stof(v[S_IDLE]) + stof(v[S_IOWAIT]));
}


#endif
