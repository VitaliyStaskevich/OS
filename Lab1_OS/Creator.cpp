#include "employee.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>

int main(int argc, char* argv[]){

    const char* extension = ".bin";
    size_t len1 = std::strlen(argv[1]);
    size_t len2 = std::strlen(extension);

    char* concatenated = new char[len1 + len2 + 1];
    strcpy(concatenated, argv[1]);
    strcpy(concatenated + len1, extension);

	std::ofstream fout(concatenated, std::ios::binary);

    if (!fout) {
        std::cerr << "Error opening the file for writing!" << std::endl;
        return -1;
    }

	int employeeAmount = atoi(argv[2]);
	std::vector <employee> records(employeeAmount);

    for (int i = 0; i < employeeAmount; ++i) {
        std::cout << "Enter the ID of an employee number " << i + 1 << ":";
        std::cin >> records[i].num;
        std::cout << "Enter the name of an employee number " << i + 1 << ":";
        std::cin >> records[i].name;
        std::cout << "Enter the hours of an employee number " << i + 1 << ":";
        std::cin >> records[i].hours;
    }

    fout.write(reinterpret_cast<char*>(&employeeAmount), sizeof(int)); // amount of records

    for (int i = 0; i < employeeAmount; ++i) {
        fout.write(reinterpret_cast<char*>(&records[i]), sizeof(employee));
    }

    fout.close();
    delete[] concatenated;
    return 0;
}