#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

const int DELAY = 25000;
const int MAX_MESSAGE_LENGTH = 20;
const int EXIT_DELAY = 500;

enum Action{
    read = 1,
    stop = 2
};
int main()
{
    STARTUPINFO si = {0};
    PROCESS_INFORMATION pi{nullptr};
    std::string binFilename;
    int recordsNumber = 0;
    int processesNumber = 0;

    std::cout << "Enter the .bin file name: ";
    std::cin >> binFilename;
    binFilename+=".bin";
    std::cout << "Enter the number of records: ";
    std::cin >> recordsNumber;
    std::cout << "Enter the number of processes: ";
    std::cin >> processesNumber;

    if(recordsNumber < 0)
    {
        std::cerr << "Number of records should be positive";
        return 1;
    }

    HANDLE hCurrentProcess = CreateMutex(nullptr, TRUE, "Current Process Owner");
    HANDLE hReadAccessSemaphore = CreateSemaphore(nullptr, 0, recordsNumber, "Read Access Semaphore");
    HANDLE hWriteAccessSemaphore = CreateSemaphore(nullptr, recordsNumber, recordsNumber, "Write Access Semaphore");

    HANDLE *hProcesses = new HANDLE[processesNumber];
    HANDLE *hStartEvents = new HANDLE[processesNumber];

    for(int i = 0; i < processesNumber; i++)
    {
        char* num;
        std::cout << "Starting process number " << i << "\n";
        hStartEvents[i] = CreateEvent(nullptr, TRUE, FALSE, (LPCSTR) (num));
    }

    std::string request;
    for (int i = 0; i < processesNumber; i++)
    {
        request = std::string("sender.exe") + " " + binFilename + " " + std::to_string(i);
        if(!CreateProcess(nullptr, &request[0], nullptr, nullptr, TRUE, CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi))
        {
            std::cerr << "Failed to create process" << std::endl;
        }
        CloseHandle(pi.hThread);
        hProcesses[i] = pi.hProcess;
    }
    WaitForMultipleObjects(processesNumber, hStartEvents, TRUE, INFINITE);
    ReleaseMutex(hCurrentProcess);
    int action {0};
    DWORD response;
    char message[MAX_MESSAGE_LENGTH];
    while(action != stop) {
        WaitForSingleObject(hCurrentProcess, INFINITE);
        response = WaitForSingleObject(hReadAccessSemaphore, DELAY);
        if(response == WAIT_TIMEOUT) {
            std::cerr << "No response (timeout)";
            break;
        }

        std::cout << "Choose an action: " << std::endl;
        std::cout << "1 - Read" << std::endl;
        std::cout << "2 - Exit" << std::endl;
        std::cin >> action;

        std::ifstream binInputStream;
        int bytesRead = 0;
        switch(action) {
            case read:
                binInputStream.open(binFilename, std::ios::binary);
                binInputStream.seekg(bytesRead);

                if(binInputStream.tellg() != 0 || !binInputStream.eof()) {
                    binInputStream.read((char *) &message, sizeof(message));
                    std::cout << "Message: ";
                    for(char i: message) std::cout << i;
                    std::cout << std::endl;
                    binInputStream.close();
                    bytesRead += MAX_MESSAGE_LENGTH;
                    ReleaseSemaphore(hWriteAccessSemaphore, 1, nullptr);
                }
                else {
                    continue;
                }
                break;
            case stop:
                std::cout << "Exiting..." << std::endl;
                Sleep(EXIT_DELAY);
                break;
            default:
                std::cout << "Invalid action chosen, try again" << std::endl;
                continue;
        }
    }

    CloseHandle(hCurrentProcess);
    CloseHandle(hReadAccessSemaphore);
    CloseHandle(hWriteAccessSemaphore);

    for (std::size_t i = 0; i < processesNumber; i++)
        CloseHandle(hStartEvents[i]);
    delete[] hStartEvents;

    for (std::size_t i = 0; i < processesNumber; i++)
        CloseHandle(hProcesses[i]);
    delete[] hProcesses;

    return 0;
    return 0;
}
