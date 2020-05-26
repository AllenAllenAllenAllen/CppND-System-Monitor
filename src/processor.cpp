#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long jiffies = LinuxParser::Jiffies();
    long active_jiffies = LinuxParser::ActiveJiffies();
    long jiffies_diff = jiffies - this->jiffies;
    if (jiffies_diff == 0) {
        return 0;
    }
    long active_diff = active_jiffies - this->active_jiffies;
    long uti = (active_diff) / jiffies_diff;
    this->active_jiffies = active_jiffies;
    this->jiffies = jiffies;
    return uti;
}