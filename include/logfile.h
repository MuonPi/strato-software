
#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include <stdint.h>
#include <string>

bool getTimestampValue(std::string& timestamp);
bool getTimestampFilename(std::string& timestamp, std::chrono::minutes min_interval);
bool writeLogfile(const std::string& filename, const std::string& timestamp_filename, const std::string& timestamp_value, const double* value, uint8_t num);

#endif // _LOGFILE_H_

