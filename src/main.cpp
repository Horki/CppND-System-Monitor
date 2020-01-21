#include <chrono>
#include <iomanip>
#include <iostream>
#include <ncurses.h>
#include <sstream>
#include <string>
#include <thread>
#include <ctime>
#include <vector>

#include "ProcessContainer.h"
#include "SysInfo.h"
#include "util.h"

char* getCString(const std::string & str) {
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return cstr;
}

void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win) {
  sys.setAttributes();

  mvwprintw(sys_win, 2, 2, getCString(("OS: " + sys.getOSName())));
  mvwprintw(sys_win, 3, 2, getCString(("Kernel version: " + sys.getKernelVersion())));
  mvwprintw(sys_win, 4, 2, getCString("CPU: "));
  wattron(sys_win, COLOR_PAIR(1));
  wprintw(sys_win, getCString(Util::getProgressBar(sys.getCpuPercent())));
  wattroff(sys_win, COLOR_PAIR(1));
  mvwprintw(sys_win, 5, 2, getCString(("Other cores:")));
  wattron(sys_win, COLOR_PAIR(1));

  std::vector<std::string> val = sys.getCoresStats();

  for (size_t i = 0; i < val.size(); ++i)
    mvwprintw(sys_win, (6 + i), 2, getCString(val[i]));

  wattroff(sys_win, COLOR_PAIR(1));
  mvwprintw(sys_win, 10 + 4, 2, getCString(("Memory: ")));
  wattron(sys_win, COLOR_PAIR(1));
  wprintw(sys_win, getCString(Util::getProgressBar(sys.getMemPercent())));
  wattroff(sys_win, COLOR_PAIR(1));
  mvwprintw(sys_win, 11 + 4, 2, getCString(("Total Processes:" + sys.getTotalProc())));
  mvwprintw(sys_win, 12 + 4, 2, getCString(("Running Processes:" + sys.getRunningProc())));
  mvwprintw(sys_win, 13 + 4, 2, getCString(("Up Time: " + Util::convertToTime(sys.getUpTime()))));
  wrefresh(sys_win);
}

void getProcessListToConsole(const std::vector<std::string> & processes,
                             WINDOW* win) {
  wattron(win, COLOR_PAIR(2));

  mvwprintw(win, 1, 2,  "PID:");
  mvwprintw(win, 1, 9,  "User:");
  mvwprintw(win, 1, 16, "CPU[%%]:");
  mvwprintw(win, 1, 26, "RAM[MB]:");
  mvwprintw(win, 1, 35, "Uptime:");
  mvwprintw(win, 1, 44, "CMD:");

  wattroff(win, COLOR_PAIR(2));

  for (size_t i = 0; i < processes.size(); ++i)
    mvwprintw(win, 2 + i, 2, getCString(processes[i]));
}

void printMain(const SysInfo & sys, ProcessContainer & procs) {
  /* Start curses mode */
	initscr();

  // not printing input values
  noecho();

  // Terminating on classic ctrl + c
  cbreak();

  // Enabling color change of text
  start_color();

  int yMax = 0;
  int xMax = 0;

  // getting size of window measured in lines and columns(column one char length)
  getmaxyx(stdscr, yMax, xMax);
  yMax = 17;
	WINDOW *sys_win  = newwin(yMax + 4, xMax - 1, 0,  0);
	WINDOW *proc_win = newwin(15, xMax - 1, 18 + 4, 0);

  init_pair(1, COLOR_BLUE,  COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);

  size_t counter = 0;
  while (true) {
    box(sys_win,  0, 0);
    box(proc_win, 0, 0);
    procs.refreshList();
    std::vector<std::vector<std::string>> processes = procs.getList();
    // auto processes = procs.getList();
    writeSysInfoToConsole(sys, sys_win);
    getProcessListToConsole(processes[counter], proc_win);
    wrefresh(sys_win);
    wrefresh(proc_win);
    refresh();
    sleep(1);
    if (counter == (processes.size() - 1)) {
      counter = 0;
    } else {
      ++counter;
    }
  }
	endwin();
}

int main() {
  //Object which contains list of current processes, Container for Process Class
  ProcessContainer procs;
  // Object which containts relevant methods and attributes regarding system details
  SysInfo sys;
  printMain(sys, procs);
}
