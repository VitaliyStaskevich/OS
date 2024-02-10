#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include "employee.h"

int main()
{
    char arg[1000]; // forming commands
    std::string binaryFileName;
    std::string outputFileName;
    int employeeAmount;
    int wage;

    std::cout << "Enter the name of a binary file:\n";
    std::cin >> binaryFileName;

    std::cout << "Enter the number of employees:\n";
    std::cin >> employeeAmount;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(STARTUPINFO);
    sprintf(arg, "%s %s %d", "Creator.exe", binaryFileName.c_str(), employeeAmount);
    if (!CreateProcess(nullptr, arg, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to execute Creator.exe\n";
        return -1;
    }
    WaitForSingleObject( pi.hProcess, INFINITE );

    std::cout << "Binary file:\n";
    binaryFileName+=".bin";

    std::ifstream fin(binaryFileName, std::ios::binary);
    if (!fin) {
        std::cerr << "Unable to open the input file \"" << binaryFileName << "\"\n";
        return -1;
    }

    fin.read(reinterpret_cast<char*>(&employeeAmount), sizeof(int));
    employee entry;

    std::cout << "The contents of a binary file: \n";

    for (int i = 0; i < employeeAmount; ++i) {
        fin.read(reinterpret_cast<char*>(&entry), sizeof(employee));
        std::cout << entry.num << ' ' << entry.name << ' ' << entry.hours << '\n';
    }

    std::cout << "Enter the name of an output file:\n";
    std::cin >> outputFileName;

    std::cout << "Enter the salary per hour:\n";
    std::cin >> wage;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    sprintf(arg, "%s %s %s %d", "Reporter.exe", binaryFileName.c_str(), outputFileName.c_str(), wage);
    if (!CreateProcess(nullptr, arg, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to execute Reporter.exe\n";
        return -1;
    }
    WaitForSingleObject( pi.hProcess, INFINITE );

    outputFileName+=".txt";
    std::ifstream finReport(outputFileName);
    if (!finReport) {
        std::cerr << "Unable to open the input file \"" << outputFileName << "\"\n";
        return -1;
    }

    std::string line;
    for (int i = 0; i <= employeeAmount; ++i)
    {
        std::getline(finReport, line);
        std::cout << line << "\n";
    }

    finReport.close();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

