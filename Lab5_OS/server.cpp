#include <process.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <conio.h>
#include <algorithm>
#include <string>
#include "employee.h"

int empCount;
employee *emps;
HANDLE *handleReadyEvents;
CRITICAL_SECTION empsCS;
bool *empIsModifying;
int employeeSize = sizeof(employee);

const std::string pipeName = "\\\\.\\pipe\\pipe_name";
const int BUFF_LENGTH = 10;
const int MESSAGE_LENGTH = 10;
char buff[BUFF_LENGTH];


int generateAmountOfClients()
{
    srand(time(0));
    return (rand() % 5) + 2;
}

employee *findEmployee(const int ID)
{
    employee key;
    key.num = ID;
    return static_cast<employee*>(bsearch(&key, emps, empCount, employeeSize, empCmp));
}
void sortEmployees()
{
    qsort(emps, empCount, employeeSize, empCmp);
}

void logData(std::string fileName)
{
    std::fstream fin(fileName.c_str(), std::ios::out | std::ios::binary);
    fin.write(reinterpret_cast<const char *>(emps), employeeSize * empCount);

    std::cout << "Data has been logged.\n";
    fin.close();
}

void readDataSTD()
{
    emps = new employee[empCount];

    std::cout << "Input ID, name and working hours of each employee: ";
    for (int i = 0; i < empCount; ++i)
    {
        std::cout << "Employee number " << i + 1 << ": ";
        std::cin >> emps[i].num >> emps[i].name >> emps[i].hours;
    }
}

void startProcesses(const int amount)
{
    for (int i = 0; i < amount; ++i)
    {
        std::string cmdArgs = "client.exe ";
        std::string eventName = "READY_EVENT";
        itoa(i + 1, buff, BUFF_LENGTH);

        eventName += buff;
        cmdArgs += eventName;

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        si.cb = sizeof(STARTUPINFO);
        ZeroMemory(&si, sizeof(STARTUPINFO));

        handleReadyEvents[i] = CreateEvent(NULL, TRUE,
                                           FALSE, eventName.c_str());

        char tempArg[60];
        strcat(tempArg, cmdArgs.c_str());

        if (!CreateProcess(NULL, tempArg, NULL, NULL,
                           FALSE, CREATE_NEW_CONSOLE,
                           NULL, NULL, &si, &pi))
            {
                std::cout << "Creation process error.\n";

                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
    }
}


DWORD WINAPI messaging(LPVOID p)
{
    employee *errorEmp = new employee;
    errorEmp -> num = -1;

    HANDLE hPipe = (HANDLE) p;
    while (true)
    {
        DWORD readBytes;
        char message[MESSAGE_LENGTH];

        bool isRead = ReadFile(hPipe, message,
                               MESSAGE_LENGTH, &readBytes, NULL);

        if (!isRead)
        {
            if (ERROR_BROKEN_PIPE == GetLastError()) {
                std::cout << "Client disconnected." << std::endl;
            }
            else
            {
                std::cerr << "Error in reading a message." << std::endl;
            }
            break;
        }

        if (strlen(message)!= 0)
        {
            char command = message[0];
            message[0] = ' ';
            int id = atoi(message);
            DWORD bytesWritten;
            EnterCriticalSection(&empsCS);
            employee *empToSend = findEmployee(id);
            LeaveCriticalSection(&empsCS);

            if (NULL == empToSend)
            {
               empToSend = errorEmp;
            }
            else
            {
                int ind = empToSend - emps;
                if (empIsModifying[ind])
                {
                    empToSend = errorEmp;
                }
                else
                {
                    if (command == 'r')
                    {
                        std::cout << "Requested to read ID " << id << ".";
                    }
                    else if (command == 'w')
                    {
                        std::cout << "Requested to modify ID " << id << ".";
                        empIsModifying[ind] = true;
                    }
                    else
                    {
                        std::cout << "Unknown request. ";
                    }
                }
            }
            bool isSent = WriteFile(hPipe, empToSend,employeeSize, &bytesWritten, NULL);
            if (isSent)
            {
                std::cout << "Answer is sent.\n";
            }
            else
            {
                std::cout << "Error while sending answer.\n";
            }

            if (empToSend != errorEmp && 'w' == command)
            {
                isRead = ReadFile(hPipe, empToSend, employeeSize, &readBytes, NULL);
                if (!isRead)
                {
                    std::cerr << "Error in reading a message." << std::endl;
                    break;
                } else {
                    std::cout << "Employee record changed.\n";
                    empIsModifying[empToSend - emps] = false;
                    EnterCriticalSection(&empsCS);
                    sortEmployees();
                    LeaveCriticalSection(&empsCS);
                }
            }
        }
    }
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    delete errorEmp;
    return 0;
}

void openPipes(int clientCount)
{
    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientCount];
    for (int i = 0; i < clientCount; ++i)
    {
        hPipe = CreateNamedPipe(pipeName.c_str(), PIPE_ACCESS_DUPLEX,
                                PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                PIPE_UNLIMITED_INSTANCES, 0,
                                0, INFINITE, NULL);
        if (!ConnectNamedPipe(hPipe, NULL))
        {
            std::cout << "No connected clients.\n";
            break;
        }
        if (INVALID_HANDLE_VALUE == hPipe)
        {
            std::cerr << "Creating named pipe failed.\n";
            getch();
            return;
        }
        hThreads[i] = CreateThread(NULL, 0, messaging, (LPVOID)hPipe, 0, NULL);
    }
    std::cout << "Clients connected to the pipe.\n";
    WaitForMultipleObjects(clientCount, hThreads, TRUE, INFINITE);
    std::cout << "All clients have disconnected.\n";
    delete[] hThreads;
}

int main() {
    std::string filename;
    std::cout << "Input name of file: ";
    std::cin >> filename;
    filename +=".bin";
    std::cout << "Input count of employees: ";
    std::cin >> empCount;

    readDataSTD();
    logData(filename);
    sortEmployees();

    int amountOfClients = generateAmountOfClients();
    empIsModifying = new bool[empCount];

    for (int i = 0; i < empCount; ++i)
    {
        empIsModifying[i] = false;
    }

    InitializeCriticalSection(&empsCS);
    HANDLE handleStartALL = CreateEvent(NULL, TRUE, FALSE, "START_ALL");

    handleReadyEvents = new HANDLE[amountOfClients];
    startProcesses(amountOfClients);

    WaitForMultipleObjects(amountOfClients, handleReadyEvents, TRUE, INFINITE);
    std::cout << "All processes are ready.\nStarting.\n";

    SetEvent(handleStartALL);
    openPipes(amountOfClients);

    for (int i = 0; i < empCount; ++i)
    {
        emps[i].print();
    }

    std::cout << "Press any key to exit...\n";
    getch();
    DeleteCriticalSection(&empsCS);

    delete[] empIsModifying;
    delete[] handleReadyEvents;
    delete[] emps;

    return 0;
}
