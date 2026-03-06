// #include <iostream>
// #include <string>
// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <array>
// #include <algorithm> 

// #include "muonpi.h"



// MUONPI::MUONPI()
// {}


// bool MUONPI::init()
// {
//     std::string path = "/var/muondetector/currentWorkingFileInformation.conf";
//     std::ifstream readfile(path);
//     if (!readfile.is_open())
//     {
//         std::cerr << "could not open file " << path << std::endl;
//         return false;
//     }

//     std::string line;
//     if (!std::getline(readfile, line))
//         return false;
//     if (!std::getline(readfile, line))
//         return false;
//     // if (!line.empty() && line.back() == '\n')
//     //     line.pop_back();

//     readfile.close();

//     // std::cout << line << std::endl;

//     path = line;
//     readfile.open(path);
//     if (!readfile.is_open())
//     {
//         std::cerr << "could not open file " << path << std::endl;
//         return false;
//     }

//     if (!std::getline(readfile, line))
//         return false;
    
//     readfile.close();
    
//     // std::cout << line << std::endl;
//     logfile_path = path;
//     std::cout << "MUONPI inited" << std::endl;
//     return true;
// }


// bool MUONPI::getLogfilePath()
// {
//     std::string path = "/var/muondetector/currentWorkingFileInformation.conf";
//     std::ifstream readfile(path);
//     if (!readfile.is_open())
//     {
//         std::cerr << "could not open file " << path << std::endl;
//         return false;
//     }

//     std::string line;
//     if (!std::getline(readfile, line))
//         return false;
//     if (!std::getline(readfile, line))
//         return false;

//     readfile.close();

//     logfile_path = line;
//     return true;
// }


// // bool MUONPI::getAttribute(const std::string attribute, double& value)
// // {
// //     std::ifstream readfile(logfile_path);
// //     if (!readfile.is_open())
// //     {
// //         std::cerr << "could not open file " << logfile_path << std::endl;
// //         return false;
// //     }

// //     const uint16_t N = 30;
// //     std::array<std::string, N> buffer;
// //     uint16_t index = 0;
// //     std::string line;

// //     // Datei durchlesen, nur die letzten N Zeilen behalten
// //     while (std::getline(readfile, line))
// //     {
// //         buffer[index % N] = line;
// //         index++;
// //     }
// //     readfile.close();

// //     // Rückwärts durch die letzten N Zeilen iterieren
// //     uint16_t start = std::min(index, static_cast<uint16_t>(N));
// //     for (uint16_t i = start - 1; i >= 0; i--)
// //     {
// //         const std::string& l = buffer[(index - 1 - i + N) % N];
// //         if (l.size() >= 20 + attribute.size() && l.substr(20, attribute.size()) == attribute)
// //         {
// //             // drittes Token extrahieren
// //             std::istringstream iss(l);
// //             std::string token;
// //             uint16_t token_index = 0;
// //             while (iss >> token)
// //             {
// //                 if (token_index == 2)
// //                 {
// //                     // std::cout << "rateXOR gefunden" << std::endl;
// //                     try {
// //                         value = std::stod(token);  // String zu double konvertieren
// //                         return true;               // erfolgreich
// //                     } catch (...) {
// //                         std::cerr << "attribute not convertible: " << attribute << std::endl;
// //                         return false;              // Fehler beim Konvertieren
// //                     }
// //                 }
// //                 token_index++;
// //             }
// //         }
// //     }
// //     std::cerr << "attribute not found: " << attribute << std::endl;
// //     return false;  // Attribut nicht gefunden
// // }

// bool MUONPI::getAttribute(const std::string& attribute, double& value)
// {
//     std::ifstream readfile(logfile_path);
//     if (!readfile.is_open())
//     {
//         std::cerr << "could not open file: " << logfile_path << std::endl;
//         return false;
//     }

//     constexpr size_t read_lines_no = 62;
//     std::array<std::string, read_lines_no> buffer;
//     size_t index = 0;
//     std::string line;

//     // Nur die letzten read_lines_no Zeilen behalten
//     while (std::getline(readfile, line))
//     {
//         buffer[index % read_lines_no] = std::move(line);
//         index++;
//     }

//     const size_t count = std::min(index, read_lines_no);

//     // Rückwärts suchen (letztes Vorkommen gewinnt)
//     for (int i = static_cast<int>(count) - 1; i >= 0; --i)
//     {
//         const std::string& l = buffer[(index - 1 - i + read_lines_no) % read_lines_no];

//         // Attribut muss nach Timestamp + Leerzeichen beginnen
//         const size_t attr_pos = l.find(attribute);
//         if (attr_pos == std::string::npos)
//             continue;

//         // Tokens per Position extrahieren (schneller als stringstream)
//         size_t first_space  = l.find(' ', attr_pos);
//         if (first_space == std::string::npos)
//             continue;

//         size_t second_space = l.find(' ', first_space + 1);
//         if (second_space == std::string::npos)
//             continue;

//         std::string_view value_str(
//             l.data() + first_space + 1,
//             second_space - first_space - 1
//         );

//         try
//         {
//             value = std::stod(std::string(value_str));
//             return true;
//         }
//         catch (...)
//         {
//             std::cerr << "attribute not convertible: " << attribute << std::endl;
//             return false;
//         }
//     }

//     std::cerr << "attribute not found: " << attribute << std::endl;
//     return false;
// }


// bool MUONPI::getXOR(double& value)
// {
//     return getAttribute("rateXOR", value);
// }


// bool MUONPI::getAND(double& value)
// {
//     return (getAttribute("rateAND", value));
// }


// bool MUONPI::getCoordinates(double* value)
// {
//     bool success_latitude = getAttribute("geoLatitude", value[0]);
//     bool success_longitude = getAttribute("geoLongitude", value[1]);
//     bool success_height = getAttribute("geoHeightMSL", value[2]);
//     return success_latitude && success_longitude && success_height;
// }

