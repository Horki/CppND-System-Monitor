#pragma once

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
#include <time.h>
#include <unistd.h>
#include <vector>

#include "constants.h"
#include "util.h"

static float getSysActiveCpuTime(const std::vector<std::string>&);
static float getSysIdleCpuTime(const std::vector<std::string>&);

class ProcessParser {
private:
  std::ifstream stream;
public:
  static std::string getCmd(std::string pid);
  static std::vector<std::string> getPidList();
  static std::string getVmSize(std::string pid);
  static std::string getCpuPercent(std::string pid);
  static long int getSysUpTime();
  static std::string getProcUpTime(std::string pid);
  static std::string getProcUser(std::string pid);
  static std::vector<std::string> getSysCpuPercent(std::string coreNumber = "");
  static float getSysRamPercent();
  static std::string getSysKernelVersion();
  static int getTotalThreads();
  static int getTotalNumberOfProcesses();
  static int getNumberOfRunningProcesses();
  static std::string getOSName();
  static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string> values2);
  static bool isPidExisting(std::string pid);
  // This one is missing
  static int getNumberOfCores();
};

// TODO: Define all of the above functions below:
// $ cat /proc/${pid}/status | grep VmData
std::string ProcessParser::getVmSize(std::string pid) {
  if (!ProcessParser::isPidExisting(pid)) return "";
  std::string line;
  std::string name = "VmData";
  std::string value;
  float result = 0.f;
  std::ifstream stream;
  Util::getStream((Path::basePath() + pid + "/" + Path::statusPath()), stream);
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    std::istringstream buf(line);
    std::istream_iterator<std::string> it_out(buf), end;
    std::vector<std::string> values(it_out, end);
    result = (stof(values[1])) / float(1024);
    break;
  }
  return std::to_string(result);
}

// $ cat /proc/cpuinfo | grep "cpu cores" # not this
// $ cat /proc/cpuinfo | grep "siblings" 
int ProcessParser::getNumberOfCores() {
  std::string line;
  // string name = "cpu cores";
  std::string name = "siblings";
  std::ifstream stream;
  Util::getStream((Path::basePath() + "cpuinfo"), stream);
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    std::istringstream buf(line);
    std::istream_iterator<std::string> it_out(buf), end;
    std::vector<std::string> values(it_out, end);
    return stoi(values[2]);
  }

  return 0;
}

// $ cat /proc/{pid}/cmdline
std::string ProcessParser::getCmd(std::string pid) {
  if (!ProcessParser::isPidExisting(pid)) return "...";
  std::string line;
  std::ifstream stream;
  Util::getStream((Path::basePath() + pid + "/" + Path::cmdPath()), stream);
  getline(stream, line);
  return line;
}

std::vector<std::string> ProcessParser::getPidList() {
  DIR* dir;
  if (!(dir = opendir("/proc"))) throw std::runtime_error(strerror(errno));
  std::vector<std::string> result;

  while (dirent * d = readdir(dir)) {
    // Check if is a directory
    if (d->d_type != DT_DIR) continue;

    // Is it digit number directory(aka PID)
    if (std::all_of(d->d_name, d->d_name + strlen(d->d_name), [](char c) {
          return isdigit(c);
        })) {
      result.push_back(d->d_name);
    }
  }
  if (closedir(dir)) throw std::runtime_error(strerror(errno));

  return result;
}

// $ cat /proc/${pid}/stat
std::string ProcessParser::getCpuPercent(std::string pid) {
  if (!ProcessParser::isPidExisting(pid)) return "";
  std::string line;
  std::string value;
  float result;
  std::ifstream stream;
  Util::getStream((Path::basePath() + pid + "/" + Path::statPath()), stream);
  getline(stream, line);
  std::string str = line;
  std::istringstream buf(str);
  std::istream_iterator<std::string> it_out(buf), end;
  std::vector<std::string> values(it_out, end);

  // acquiring relevant times for calculation of active occupation of CPU for selected process
  float utime = stof(values[13]) / float(sysconf(_SC_CLK_TCK));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::getSysUpTime();
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime/freq);
  result = 100.0 * ((total_time / freq) / seconds);

  return std::to_string(result);
}

// $ cat /proc/uptime
long int ProcessParser::getSysUpTime() {
  std::string line;
  std::ifstream stream;
  Util::getStream(Path::basePath() + Path::upTimePath(), stream);
  getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<std::string> it_out(buf), end;
  std::vector<std::string> values(it_out, end);

  return stoi(values[0]);
}

// $ cat /proc/${pid}/stat # get proc uptime
std::string ProcessParser::getProcUpTime(std::string pid) {
  if (!ProcessParser::isPidExisting(pid)) return "";
  std::string line;
  std::string value;
  std::ifstream stream;

  Util::getStream((Path::basePath() + pid + "/" + Path::statPath()), stream);
  getline(stream, line);
  std::string str = line;
  std::istringstream buf(str);
  std::istream_iterator<std::string> it_out(buf), end;
  std::vector<std::string> values(it_out, end);

  return std::to_string(float(stof(values[13]) / sysconf(_SC_CLK_TCK)));
}

// $ cat /proc/${pid}/status | grep Uid:
// $ cat /etc/passwd | grep ${uid}
std::string ProcessParser::getProcUser(std::string pid) {
  if (!ProcessParser::isPidExisting(pid)) return "";
  // cat /proc/${pid}/status | grep Uid:
  std::string line;
  std::string name = "Uid:";
  std::string value;
  std::string result;
  std::ifstream stream;
  Util::getStream((Path::basePath() + pid + "/" + Path::statusPath()), stream);
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    std::istringstream buf(line);
    std::istream_iterator<std::string> it_out(buf), end;
    std::vector<std::string> values(it_out, end);
    result = values[1];
    break;
  }
  stream.close();

  // cat /etc/passwd | grep ${uid}
  Util::getStream("/etc/passwd", stream);
  name = "x:" + result;
  while (getline(stream, line)) {
    if (line.find(name) != std::string::npos) return line.substr(0, line.find(":"));
  }
  return "nonuser";
}

// $ cat /proc/stat | grep cpu${coreNumber}
std::vector<std::string> ProcessParser::getSysCpuPercent(std::string coreNumber) {
  std::string line;
  std::string name = "cpu" + coreNumber;
  std::ifstream stream;
  Util::getStream((Path::basePath() + Path::statPath()), stream);

  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    // remove cpuX from line string
    std::istringstream buf(line.erase(0, name.length()));
    std::istream_iterator<std::string> it_out(buf), end;
    // could return vector<size_t>
    std::vector<std::string> values(it_out, end);
    return values;
  }
  return std::vector<std::string>();
}

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

// $ cat /proc/meminfo
float ProcessParser::getSysRamPercent() {
  std::string line;
  std::string name1 = "MemAvailable:";
  std::string name2 = "MemFree:";
  std::string name3 = "Buffers:";
  std::ifstream stream;
  float total_mem = 0;
  float free_mem = 0;
  float buffers = 0;

  Util::getStream((Path::basePath() + Path::memInfoPath()), stream);
  while (getline(stream, line)) {
    if (total_mem != 0 && free_mem != 0) break;

    if (line.compare(0, name1.size(), name1) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> it_out(buf), end;
      std::vector<std::string> values(it_out, end);
      total_mem = stof(values[1]);
    }

    if (line.compare(0, name2.size(), name2) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> it_out(buf), end;
      std::vector<std::string> values(it_out, end);
      free_mem = stof(values[1]);
    }

    if (line.compare(0, name3.size(), name3) == 0) {
      std::istringstream buf(line);
      std::istream_iterator<std::string> it_out(buf), end;
      std::vector<std::string> values(it_out, end);
      buffers = stof(values[1]);
    }

  }
  return float(100.f * (1.0 - (free_mem / (total_mem - buffers))));
}

// $ cat /proc/version
std::string ProcessParser::getSysKernelVersion() {
  std::string line;
  // string name = "Linux version "; // No need it's one line only!
  std::ifstream stream;
  Util::getStream((Path::basePath() + Path::versionPath()), stream);
  getline(stream, line);
  std::istringstream buf(line);
  std::istream_iterator<std::string> it_out(buf), end;
  std::vector<std::string> values(it_out, end);
  return values[2];
}

// Get Number of threads for all running processes
// $ cat /proc/${pid}/status | grep Threads:
int ProcessParser::getTotalThreads() {
  int result = 0;
  std::string line;
  std::string name = "Threads:";
  std::vector<std::string> processes = ProcessParser::getPidList();
  for (const auto &pid : processes) {
    std::ifstream stream;
    Util::getStream((Path::basePath() + pid + "/" + Path::statusPath()), stream);
    while (getline(stream, line)) {
      if (line.compare(0, name.size(), name) != 0) continue;
      std::istringstream buf(line);
      std::istream_iterator<std::string> it_out(buf), end;
      std::vector<std::string> values(it_out, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

// $ cat /proc/stat | grep processes
int ProcessParser::getTotalNumberOfProcesses() {
  int result = 0;
  std::string line;
  std::string name = "processes";
  std::ifstream stream;
  Util::getStream(((Path::basePath() + Path::statPath())), stream);
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    std::istringstream buf(line);
    std::istream_iterator<std::string> it_out(buf), end;
    std::vector<std::string> values(it_out, end);
    result += stoi(values[1]);
    break;
    // return stoi(values[1]);
  }
  return result;
}

// $ cat /proc/stat | grep procs_running
int ProcessParser::getNumberOfRunningProcesses() {
  int result = 0;
  std::string line;
  std::string name = "procs_running";
  std::ifstream stream;
  Util::getStream(((Path::basePath() + Path::statPath())), stream);
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    std::istringstream buf(line);
    std::istream_iterator<std::string> it_out(buf), end;
    std::vector<std::string> values(it_out, end);
    result += stoi(values[1]);
    // break;
    // return stoi(values[1]);
  }
  return result;
}

// $ cat /etc/os-release | grep PRETTY_NAME
std::string ProcessParser::getOSName() {
  std::string line;
  std::string name = "PRETTY_NAME=";
  std::ifstream stream;
  Util::getStream("/etc/os-release", stream);

  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) != 0) continue;
    size_t found       = line.find("=");
    std::string result = line.substr(found + 1);
    result.erase(remove(result.begin(), result.end(), '"'), result.end());
    return result;
  }
  return "";
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1,
                                         std::vector<std::string> values2) {
  float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idleTime   = getSysIdleCpuTime(values2)   - getSysIdleCpuTime(values1);
  float totalTime  = activeTime + idleTime;
  // Fix in short intervals!
  if (totalTime == 0.f || activeTime == 0.f)        return std::to_string(activeTime);
  float result     = 100.0 * (activeTime / totalTime);
  return std::to_string(result);
}

bool ProcessParser::isPidExisting(std::string pid) {
  // Check if PID directory is still existing
  DIR * dir = opendir(std::string(Path::basePath() + pid).c_str());
  if (dir) {
    closedir(dir);
    return true;
  }
  return false;
}
