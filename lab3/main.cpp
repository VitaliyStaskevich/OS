#include <iostream>
#include <vector>
#include <Windows.h>
#include <process.h>


struct Params {
    int* arr;
    int id;
    int size;
    HANDLE hEndEvent;
    HANDLE hCantWorkEvent;
    HANDLE hContinueEvent;
    CRITICAL_SECTION* cs;
};

DWORD WINAPI marker(LPVOID param) {
    Params* par = static_cast<Params*>(param);
    srand(par->id);
    while(true)
    {
        int val = rand() % par->size;
        EnterCriticalSection(par->cs);
        if(par->arr[val] == 0){
            Sleep(5);
            par->arr[val] = par -> id;
            Sleep(5);
            LeaveCriticalSection(par->cs);
            continue;
        }
        else{
            int marked {};
            int markedID {};
            for(int i = 0; i < par->size; ++i){
                if(par->arr[i] !=0){
                    ++marked;
                    if(par->arr[i] == par->id){
                        ++markedID;
                    }
                }
            }

            std::cout << "\nThread ID: " << par->id
                      << "\nNumbers marked: " << marked
                      << "\nNumbers marked by this thread: " << markedID
                      << "\nFailed to mark at index: " << val << '\n';
            LeaveCriticalSection(par->cs);

            HANDLE hEvents[2] = {par->hContinueEvent, par->hEndEvent};
            SetEvent(par->hCantWorkEvent);
            if(WaitForMultipleObjects(2, hEvents, FALSE, INFINITE) != WAIT_OBJECT_0 + 1){
                continue;
            }
            else{
                EnterCriticalSection(par->cs);
                std::cout << "\nThread number " << par->id << " finished its work \n";
                for(int i = 0; i < par->size; ++i){
                    if(par->arr[i] == par->id){
                        par->arr[i] = 0;
                    }
                }
                LeaveCriticalSection(par->cs);
                return 0;
            }
        }
    }

    return 0;
}


void Print(int* arr, int n){
    std::cout << "Array: ";
    for(int i = 0; i < n; ++i){
        std::cout << arr[i] << ' ';
    }
    std::cout << '\n';
}

void removeElement(HANDLE* &arr, int size, int index) {
    if (index < 0 || index >= size) {
        std::cout << "Invalid index\n";
        return;
    }

    HANDLE* newArr = new HANDLE[size - 1];
    for (int i = 0, j = 0; i < size; ++i) {
        if (i != index) {
            newArr[j++] = arr[i];
        }
    }

    delete[] arr;
    arr = newArr;
}

int main()
{
    int n {};
    std::cout << "Enter the amount of elements: \n";
    std::cin >> n;

    if (n <= 0) {
        std::cout << "Array size should be positive: \n";
        return -1;
    }

    int* arr = new int[n] {};

    std::cout << "Enter the amount of threads: \n";
    int threads {};
    std::cin >> threads;

    if (threads <= 0 || threads >= 12) {
        std::cout << "The amount of threads should be between 1 and 12 \n";
        return -1;
    }

    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    HANDLE* markers = new HANDLE[threads];
    HANDLE* handlesCantContinue = new HANDLE[threads];
    HANDLE* handlesContinue = new HANDLE[threads];
    HANDLE* handlesEnd = new HANDLE[threads];

    for (size_t i = 0; i < threads; ++i){
        HANDLE cantWorkEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
        HANDLE continueEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
        HANDLE endEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
        if (cantWorkEvent == NULL || endEvent == NULL || continueEvent == NULL) {
            std::cout << "Invalid handle, event creation failed \n";
            return -1;
        }
        handlesCantContinue[i] = cantWorkEvent;
        handlesContinue[i] = continueEvent;
        handlesEnd[i] = endEvent;

        Params* par = new Params;
        par -> arr = arr;
        par -> size = n;
        par -> id = i+1;
        par -> cs = &cs;
        par -> hEndEvent = endEvent;
        par -> hCantWorkEvent = cantWorkEvent;
        par -> hContinueEvent = continueEvent;

        EnterCriticalSection(&cs);
        markers[i] = CreateThread(NULL, 0,  marker, par, 0, NULL);

        if(markers[i] == NULL){
            std::cout << "Invalid handle, event creation failed \n";
            delete par;
            return -1;
        }
        LeaveCriticalSection(&cs);
    }

    bool* isActive = new bool[threads];
    for(int i = 0; i < threads; ++i){
        isActive[i] = true;
    }
    int initialThreads = threads;

    while(true){
        DWORD dwWaitResult = WaitForMultipleObjects(threads, handlesCantContinue, TRUE, INFINITE);
        Print(arr,n);

        int inputID {};
        bool validID = false;
        while (!validID) {
            std::cout << "Input a thread's ID to stop: \n";
            std::cin >> inputID;
            --inputID;

            if(inputID > initialThreads || inputID < 0 || !isActive[inputID]){
            std::cout << "Input id is invalid, try again \n";
            }
            else{
                validID = true;
            }
        }
        int offset{};
        for(int i = 0; i < inputID; ++i){
            if(isActive[i]){
                ++offset;
            }
        }
        isActive[inputID] = false;

        SetEvent(handlesEnd[inputID]);
        WaitForSingleObject(markers[inputID],INFINITE);
        threads--;



        HANDLE* temp = new HANDLE[threads];

        CloseHandle(handlesCantContinue[offset]);
        memmove(temp, handlesCantContinue, offset * sizeof(HANDLE));
        memmove(&temp[offset], &handlesCantContinue[offset + 1], (threads - offset) * sizeof(HANDLE));

        delete[] handlesCantContinue;
        handlesCantContinue = temp;



        std::cout << "Array after the thread is stopped: \n";
        Print(arr,n);

        if(threads == 0){
            std::cout << "The last thread was stopped\n";
            break;
        }

        CloseHandle(handlesContinue[offset]);


        int index{};
        for (int i = 0; i <= threads; ++i) {
            if (handlesContinue[i] == handlesContinue[offset]){
            index = i;
            break;
            }
        }
        removeElement(handlesContinue, threads+1, index);

        for (int i = 0; i < threads; ++i){
            SetEvent(handlesContinue[i]);
        }
    }


    for(int i = 0; i < initialThreads; ++i){
         CloseHandle(markers[i]);
         CloseHandle(handlesEnd[i]);
    }

    delete[] markers;
    delete[] handlesEnd;
    delete[] handlesContinue;
    delete[] handlesCantContinue;
    delete[] arr;
    delete[] isActive;

    return 0;
}
