#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
    int hours = seconds / 3600;
    int mins = (seconds % 3600) / 60;
    int secs = (seconds % 3600) % 60;
    string hours_s = std::to_string(hours);
    if (hours_s.length() == 1) {
        hours_s = "0" + hours_s;
    }
    string mins_s = std::to_string(mins);
    if (mins_s.length() == 1) {
        mins_s = "0" + mins_s;
    }
    string secs_s = std::to_string(secs);
    if (secs_s.length() == 1) {
        secs_s = "0" + secs_s;
    }
    return hours_s + ":" + mins_s + ":" + secs_s;
}