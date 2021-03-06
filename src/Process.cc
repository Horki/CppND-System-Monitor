#include <Process.h>
  
Process::Process(const std::string & pid) {
  this->pid    = pid;
  this->user   = ProcessParser::getProcUser(pid);
  this->mem    = ProcessParser::getVmSize(pid);
  this->cmd    = ProcessParser::getCmd(pid);
  this->upTime = ProcessParser::getProcUpTime(pid);    
  this->cpu    = ProcessParser::getCpuPercent(pid);
}

void        Process::setPid(int pid)   { this->pid = std::to_string(pid); }

std::string Process::getPid()    const { return this->pid;                }

std::string Process::getUser()   const { return this->user;               }

std::string Process::getCmd()    const { return this->cmd;                }

int         Process::getCpu()    const { return stoi(this->cpu);          }

int         Process::getMem()    const { return stoi(this->mem);          }

std::string Process::getUpTime() const { return this->upTime;             }

std::string Process::getProcess() {
  if (!ProcessParser::isPidExisting(this->pid)) return "";

  this->mem    = ProcessParser::getVmSize(this->pid);
  this->upTime = ProcessParser::getProcUpTime(this->pid);
  this->cpu    = ProcessParser::getCpuPercent(this->pid);

  return (this->pid + "  "
          + this->user + "   "
          + this->mem.substr(0, 5) + "     "
          + this->cpu.substr(0, 5) + "     "
          + this->upTime.substr(0, 5) + "    "
          + this->cmd.substr(0, 30));
}

