
#include <stdint.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "logfile.h"



bool getTimestampValue(std::string& timestamp)
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm now_tm;
    localtime_r(&now, &now_tm);
    std::ostringstream timestamp_o;
    timestamp_o << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S");
    timestamp = timestamp_o.str();
    return true;
}



bool getTimestampFilename(std::string& timestamp, std::chrono::minutes min_interval)
{
    static std::time_t last_bucket = -1;
    static std::string cached_timestamp;

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    const std::time_t interval_sec = min_interval.count() * 60;

    std::time_t current_bucket = now / interval_sec;

    if (current_bucket != last_bucket)
    {
        last_bucket = current_bucket;

        std::time_t bucket_start = current_bucket * interval_sec;

        std::tm now_tm{};
        localtime_r(&bucket_start, &now_tm);   // oder gmtime_r

        std::ostringstream timestamp_o;
        timestamp_o << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S");
        cached_timestamp = timestamp_o.str();
    }

    timestamp = cached_timestamp;
    return true;
}



bool writeLogfile(const std::string& filename, const std::string& timestamp_filename, const std::string& timestamp_value, const double* value, uint8_t num)
{
    std::string path;
    path = "/var/strato-software/" + filename + "_" + timestamp_filename + ".txt";

    // if (!std::filesystem::exists(path))
    // {
    //     std::ofstream createfile(path);
    //     createfile << 0;
    //     createfile.close();
    // }

    std::ofstream writefile(path, std::ios::out | std::ios::app); // anhängen
    if (!writefile.is_open())
    {
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }

    writefile << timestamp_value;
    for (size_t i = 0; i < num; i++)
    {
        writefile << ";" << value[i];
    }
    writefile << ";" << std::endl;
    writefile.close();

    // std::cout << path << std::endl;
    return true;
}

