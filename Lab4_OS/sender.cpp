#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

void sendMsg(std::string message, const std::string& binFileName, std::ofstream& binOutputStream, int MESSAGE_LENGTH, char* messageCharset, HANDLE* hReadAccessSemaphore) {
    std::cout << "Input message: "; std::cin >> message;
    for (std::size_t i = 0; i < MESSAGE_LENGTH; i++)
        messageCharset[i] = '_';
    message.copy(messageCharset, message.length());
    binOutputStream.open(binFileName, std::ios::binary | std::ios::app);
    binOutputStream.write((char*)&messageCharset, sizeof(messageCharset));
    binOutputStream.close();
    ReleaseSemaphore(hReadAccessSemaphore, 1, nullptr);
}

enum Option{
    write = 1,
    stop = 2
};

const int EXIT_DELAY = 500;


int main(int argc, char* argv[]) {
    std::ofstream   binOutputStream;
    std::string     binFileName{ argv[1] };
    std::string     message;
    char            messageCharset[20]{    };
    const int       MESSAGE_LENGTH{ 20 };
    int             option{ 0 };
    int             processId{std::stoi(argv[2]) };
    HANDLE          hReadAccessSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Read Access Semaphore");
    HANDLE          hWriteAccessSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Write Access Semaphore");
    HANDLE          hCurrentProcess = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "Current Process Owner2");
    HANDLE          senderStartEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "START PROCESS" + processId);

    SetEvent(senderStartEvent);
    std::cout << "You are currently in a sender process number " << processId << std::endl;

    while (option != stop) {
        WaitForSingleObject(hWriteAccessSemaphore, INFINITE);
        WaitForSingleObject(hCurrentProcess, INFINITE);
        std::cout << "Options: \n" <<
                  "1. Write messages in " + binFileName + " (maximum length of a message is 20 characters) \n" <<
                  "2. Exit program \n";
        std::cin >> option;
        switch (option) {
            case write:
                option = 0;
                sendMsg(message, binFileName, binOutputStream, MESSAGE_LENGTH, messageCharset, &hReadAccessSemaphore);
                ReleaseSemaphore(hWriteAccessSemaphore, 1, nullptr);
                break;
            case stop:
                std::cout << "Exiting process..." << std::endl;
                ReleaseMutex(hCurrentProcess);
                ReleaseSemaphore(hReadAccessSemaphore, 1, nullptr);
                Sleep(EXIT_DELAY);
                break;
            default:
                std::cout << "Invalid option, try again\n";
                continue;
        }
    }
    return 0;
}
