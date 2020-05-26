#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  int mem_total = mem_free = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), 'KB', '');
      std::replace(line.begin(), line.end(), ' ', '');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string key;
      int value;
      if (linestream >> key >> value) {
        if (key == "MemTotal") {
          mem_total = value
        } else if (key == "MemFree") {
          mem_free = value;
          break;
        }
      }
    }
  }
  if (mem_total == 0) {
    return 0;
  }
  return (mem_total - mem_free) * 1.0 / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string uptime;
    linestream >> uptime;
    return stol(uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.isopen()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string value;
    for (int i = 0; i < 13; i++) {
      istringstream >> value;
    }
    return (stol(istringstream >> value) + stol(istringstream >> value)) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> states = CpuUtilization();
  return stol(states[CPUStates::kUser_]) + stol(states[CPUStates::kNice_]) + \
          stol(states[CPUStates::kSystem_]) + stol(states[CPUStates::kIRQ_]) + \
          stol(states[CPUStates::kSoftIRQ_]) + stol(states[CPUStates::kSteal_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> states = CpuUtilization();
  return stol(states[CPUStates::kIdle_]) + stol(states[CPUStates::kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string key;
    linestream >> key;
    string value;
    vector<string> states;
    while (linestream >> value) {
      states.push_back(value);
    }
    return states;
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int num_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      if (linestream >> key >> value) {
        if (key == "processes") {
          num_processes = stoi(value);
          break;
        }
      }
    }
  }
  return num_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int num_running_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, value;
      if (linestream >> key >> value) {
        if (key == "procs_running") {
          num_processes = stoi(value);
          break;
        }
      }
    }
  }
  return num_running_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.isopen()) {
    string line;
    std::getline(stream, line);
    return line;
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), 'KB', '');
      std::replace(line.begin(), line.end(), ' ', '');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string key, value;
      if (linestream >> key >> value) {
        if (key == "VmSize") {
          return string((int)(value * 0.001));
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key;
      if (linestream >> key) {
        if (key == "UID:") {
          int num_start = num_end = -1;
          for (int i = 0; i < line.length(); i++) {
            if (num_start == -1) {
              if (std::all_of(line[i].begin(), line[i].end, isdigit)){
                num_start = i;
              }
            } else {
              if (!std::all_of(line[i].begin(), line[i].end, isdigit)) {
                num_end = i;
                break;
              }
            }
          }
          return line.substr(num_start, num_end);
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  int uid = stoi(Uid(pid));
  std::ifstream stream(kPasswordPath);
  if (stream.isopen()) {
    string line;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      string user, c;
      int curr_pid;
      if (linestream >> user >> c >> curr_pid) {
        if (curr_pid == uid) {
          return user;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string value;
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    return stol(value) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}