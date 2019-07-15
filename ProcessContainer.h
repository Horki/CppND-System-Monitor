#pragma once

#include "Process.h"
#include <vector>
#include <string>

class ProcessContainer {
private:
  std::vector<Process> _list;
public:
  ProcessContainer() {
    this->refreshList();
  }
  void refreshList();
  std::string printList();
  std::vector<std::vector<std::string>> getList();
  // std::vector<std::string> getList();
};

void ProcessContainer::refreshList() {
  std::vector<std::string> pidList = ProcessParser::getPidList();
  this->_list.clear();
  for (auto pid : pidList) {
    Process proc(pid);
    this->_list.push_back(proc);
  }
}

std::string ProcessContainer::printList() {
  std::string result = "";
  for (auto v : _list) {
    result += v.getProcess();
  }
  return result;
}

// Last 10 processes
// std::vector<std::string> ProcessContainer::getList() {
//   std::vector<std::string> values;
//   for (size_t i = (this->_list.size() - 10); i < this->_list.size(); ++i) {
//     values.push_back(this->_list[i].getProcess());
//   }
//   return values;
// }

std::vector<std::vector<std::string>> ProcessContainer::getList() {
  std::vector<std::vector<std::string>> values;
  std::vector<std::string> stringifiedList;
  for (size_t i = 0; i < ProcessContainer::_list.size(); i++) {
    stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
  }
  size_t lastIndex = 0;
  for (size_t i = 0; i < stringifiedList.size(); ++i) {
    if (i % 10 == 0 && i > 0) {
      std::vector<std::string> sub(&stringifiedList[i - 10], &stringifiedList[i]);
      values.push_back(sub);
      lastIndex = i;
    }
    if (i == (ProcessContainer::_list.size() - 1) && (i-lastIndex) < 10) {
      std::vector<std::string> sub(&stringifiedList[lastIndex], &stringifiedList[i + 1]);
      values.push_back(sub);
    }
  }
  return values;
}
