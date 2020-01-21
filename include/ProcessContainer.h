#ifndef PROCESS_CONTAINER_H
#define PROCESS_CONTAINER_H

#include <Process.h>
#include <vector>
#include <string>

class ProcessContainer {
private:
  std::vector<Process> _list;
public:
  ProcessContainer();
  void refreshList();
  std::string printList();
  std::vector<std::vector<std::string>> getList();
  // std::vector<std::string> getList();
};

#endif
