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
    int seconds = (seconds % 3600) % 60;
    return std::to_string(hours) + ":" + std::to_string(mins) + ":" + std::to_string(seconds);
}