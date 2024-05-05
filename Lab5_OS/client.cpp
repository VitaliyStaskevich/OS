#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string>
#include "employee.h"

const int COMMAND_LENGTH = 12;
const int MS_PIPE_WAIT = 2000;
const std::string IP_NAME = "START_ALL";
const std::string PIPE_NAME = "\\\\.\\pipe\\pipe_name";

void logErrorMessage(const std::string& message)
{
    std::cerr << message;
    getch();
}

void messaging(HANDLE handlePipe)
{
    std::cout << "Press Ctrl+Z to quit\n";

    while (true)
    {
        std::cout << "Input r to read or w to write a command and the ID of an employee: ";
        char command[COMMAND_LENGTH];
        std::cin.getline(command, COMMAND_LENGTH, '\n');

        if (std::cin.eof())
        {
            logErrorMessage("File is empty \n");
            return;
        }

        bool isSent;
        DWORD bytesWritten;
        isSent = WriteFile(handlePipe, command, COMMAND_LENGTH, &bytesWritten, NULL);

        if (!isSent)
        {
            logErrorMessage("Message couldn't be sent\n");
            return;
        }

        bool isRead;
        DWORD readBytes;
        employee tempEmployee;
        isRead = ReadFile(handlePipe, &tempEmployee, sizeof(tempEmployee), &readBytes, NULL);

        if (!isRead)
        {
            logErrorMessage("Error while receiving answer \n");
            continue;
        }

        if (tempEmployee.num < 0)
        {
            logErrorMessage("Employee has not been found or is being modified.\n");
            continue;
        }

        tempEmployee.print();

        if (command[0] == 'w')
        {
            std::cout << "Input the ID of employee: ";
            std::cin >> tempEmployee.num;
            std::cout << "Input the name of employee: ";
            std::cin >> tempEmployee.name;
            std::cout << "Input the working hours of employee: ";
            std::cin >> tempEmployee.hours;

            std::cin.ignore(2, '\n');

            isSent = WriteFile(handlePipe, &tempEmployee, sizeof(tempEmployee), &bytesWritten, NULL);

            if (!isSent)
            {
                logErrorMessage("Error occurred while sending\n");
                break;
            }

            std::cout << "New record has been sent.\n";
        }
    }
}


int main(int argc, char* argv[])
{
    HANDLE handleReadyEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[1]);
    HANDLE handleStartEvent = OpenEvent(SYNCHRONIZE, FALSE, IP_NAME.c_str());

    if (handleStartEvent == nullptr || handleReadyEvent == nullptr)
    {
        std::cerr << "Error in action with event\n";
        getch();
        return GetLastError();
    }

    SetEvent(handleReadyEvent);
    WaitForSingleObject(handleStartEvent, INFINITE);
    HANDLE handlePipe;

    std::cout << "Process has been started.\n";

    while (true)
    {
        handlePipe = CreateFile(PIPE_NAME.c_str(), GENERIC_WRITE | GENERIC_READ,
                                FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, 0, NULL);

        const bool FLAG = WaitNamedPipe(PIPE_NAME.c_str(), MS_PIPE_WAIT)
                          + (INVALID_HANDLE_VALUE != handlePipe);

        if (FLAG)
        {
            if (INVALID_HANDLE_VALUE != handlePipe)
            {
                break;
            }
            logErrorMessage(MS_PIPE_WAIT + "timed out\n");
            return 0;
        }
    }
    std::cout << "Pipe has been connected\n";
    messaging(handlePipe);
    return 0;
}
