#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <cstdio>

#include <ProcessParser.h>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
private:
  std::string pid;
  std::string user;
  std::string cmd;
  std::string cpu;
  std::string mem;
  std::string upTime;

public:
  Process(const std::string &);
  void        setPid(int);
  std::string getPid()    const;
  std::string getUser()   const;
  std::string getCmd()    const;
  int         getCpu()    const;
  int         getMem()    const;
  std::string getUpTime() const;
  std::string getProcess();
};

#endif
