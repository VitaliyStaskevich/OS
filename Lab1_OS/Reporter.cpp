#include "employee.h"
#include <iostream>
#include <fstream>
#include <cstring>

int main(int argc, char* argv[]) {
    std::ifstream fin(argv[1], std::ios::binary);
    if (!fin) {
        std::cerr << "Unable to open the input file \"" << argv[1] << "\"\n";
        return -1; 
    }
    const char* extension = ".txt";
    size_t len1 = std::strlen(argv[2]);
    size_t len2 = std::strlen(extension);

    char* concatenated = new char[len1 + len2 + 1];
    strcpy(concatenated, argv[2]);
    strcpy(concatenated + len1, extension);
    std::ofstream fout(concatenated);

    if (!fout) {
        std::cerr << "Unable to create the output file \"" << argv[2] << ".txt\"\n";
        return -1; 
    }
    int wage = atoi(argv[3]);
    fout << "Report from a file \"" << argv[1] << "\"\n";
    int employeeAmount;
    fin.read(reinterpret_cast<char*>(&employeeAmount), sizeof(int));
    employee entry;
    for (int i = 0; i < employeeAmount; ++i) {
        fin.read(reinterpret_cast<char*>(&entry), sizeof(employee));
        fout << entry.num << ' ' << entry.name << ' ' << entry.hours << ' ' << entry.hours * wage << '\n';
    }
    fin.close();
    fout.close();
    delete[] concatenated;
    return 0;
}