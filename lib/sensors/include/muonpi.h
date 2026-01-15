
#ifndef _MUONPI_H_
#define _MUONPI_H_

#include <string>

class MUONPI
{
public:
    MUONPI();

    bool init();
    bool getLogfilePath();
    bool getAttribute(const std::string attribute, double& value);
    bool getXOR(double& value);
    bool getAND(double& value);
    bool getCoordinates(double* value);

    std::string logfile_path {0};
};

#endif // _MUONPI_H_