#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>

struct employee
{
    int num;
    char name[10];
    double hours;

    void print()
    {
        std::cout << "ID: " << num << "\tName: " << name << "\tHours: " << hours << '\n';
    }
};

int empCmp(const void* p1, const void* p2)
{
    return (static_cast<const employee*>(p1))-> num - (static_cast<const employee*>(p2))->num;
}

#endif
