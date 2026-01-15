#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <algorithm> 

#include "muonpi.h"



MUONPI::MUONPI()
{}


bool MUONPI::init()
{
    std::string path = "/var/muondetector/currentWorkingFileInformation.conf";
    std::ifstream readfile(path);
    if (!readfile.is_open())
    {
        std::cerr << path << " already opened" << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(readfile, line))
        return false;
    if (!std::getline(readfile, line))
        return false;
    // if (!line.empty() && line.back() == '\n')
    //     line.pop_back();

    readfile.close();

    // std::cout << line << std::endl;

    path = line;
    readfile.open(path);
    if (!readfile.is_open())
    {
        std::cerr << path << " already opened" << std::endl;
        return false;
    }

    if (!std::getline(readfile, line))
        return false;
    
    readfile.close();
    
    // std::cout << line << std::endl;
    logfile_path = path;
    std::cout << "MUONPI INITED" << std::endl;
    return true;
}


bool MUONPI::getLogfilePath()
{
    std::string path = "/var/muondetector/currentWorkingFileInformation.conf";
    std::ifstream readfile(path);
    if (!readfile.is_open())
    {
        std::cerr << path << " already opened" << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(readfile, line))
        return false;
    if (!std::getline(readfile, line))
        return false;

    readfile.close();

    logfile_path = line;
    return true;
}


bool MUONPI::getAttribute(const std::string attribute, double& value)
{
    std::ifstream readfile(logfile_path);
    if (!readfile.is_open())
    {
        std::cerr << logfile_path << " already opened" << std::endl;
        return false;
    }

    const uint8_t N = 50;
    std::array<std::string, N> buffer;
    uint8_t index = 0;
    std::string line;

    // Datei durchlesen, nur die letzten N Zeilen behalten
    while (std::getline(readfile, line))
    {
        buffer[index % N] = line;
        index++;
    }
    readfile.close();

    // Rückwärts durch die letzten N Zeilen iterieren
    int start = std::min(index, static_cast<uint8_t>(N));
    for (int i = start - 1; i >= 0; i--)
    {
        const std::string& l = buffer[(index - 1 - i + N) % N];
        if (l.size() >= 20 + attribute.size() && l.substr(20, attribute.size()) == attribute)
        {
            // drittes Token extrahieren
            std::istringstream iss(l);
            std::string token;
            int token_index = 0;
            while (iss >> token)
            {
                if (token_index == 2)
                {
                    // std::cout << "rateXOR gefunden" << std::endl;
                    try {
                        value = std::stod(token);  // String zu double konvertieren
                        return true;               // erfolgreich
                    } catch (...) {
                        std::cerr << "attribute not convertible: " << attribute << std::endl;
                        return false;              // Fehler beim Konvertieren
                    }
                }
                token_index++;
            }
        }
    }
    std::cerr << "attribute not found: " << attribute << std::endl;
    return false;  // Attribut nicht gefunden
}


bool MUONPI::getXOR(double& value)
{
    return getAttribute("rateXOR", value);
}


bool MUONPI::getAND(double& value)
{
    return (getAttribute("rateAND", value));
}


bool MUONPI::getCoordinates(double* value)
{
    bool success_latitude = getAttribute("geoLatitude", value[0]);
    bool success_longitude = getAttribute("geoLongitude", value[1]);
    bool success_height = getAttribute("geoHeightMSL", value[2]);
    return success_latitude && success_longitude && success_height;
}

