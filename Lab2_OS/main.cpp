#include <iostream>
#include <vector>
#include <Windows.h>
#include <process.h>

struct ArrayData{
    std::vector <int> arr;
    int min;
    int max;
    double average;
};

DWORD WINAPI FinderThread(LPVOID lpParam)
{
    ArrayData* arrayData = static_cast<ArrayData*>(lpParam);
    arrayData -> min = arrayData -> arr [0];
    arrayData -> max = arrayData -> arr [0];
    for(int i=1; i<arrayData->arr.size();++i){
        if(arrayData->arr[i]<arrayData->min){
                arrayData->min = arrayData->arr[i];
            }
        Sleep(7);
        if(arrayData->arr[i]>arrayData->max){
                arrayData->max= arrayData->arr[i];
            }
        Sleep(7);
    }
    std::cout << "Min element in the array: " << arrayData->min << '\n';
    std::cout << "Max element in the array: " << arrayData->max << '\n';
    return 0;
}

DWORD WINAPI CounterThread(LPVOID lpParam)
{
    ArrayData* arrayData = static_cast<ArrayData*>(lpParam);
    int sum{};
    for(int i=0; i<arrayData->arr.size();++i){
        sum+=arrayData->arr[i];
        Sleep(12);
     }
     arrayData->average = static_cast<double>(sum)/static_cast<double>(arrayData->arr.size());
     std::cout << "Average of the numbers in the array is " << arrayData->average << '\n';
}

int main()
{
    int n{};
    std::cout << "Enter the size of an array:\n";

    std::cin >> n;
    if(n<0)
    {
        std::cerr << "Size of an array should be above zero\n";
        return -1;
    }
    std::vector<int> array(n);
    int element{};
    std:: cout << "Enter the elements:\n";
    for(int i=0; i<n; ++i){
        std::cin >> element;
        array[i]=element;
    }

    ArrayData arrayData;
    arrayData.arr=array;
    DWORD   dwThreadIdArray[2];
    HANDLE  hThreadArray[2];
    hThreadArray[0]= CreateThread(NULL, 0, FinderThread, &arrayData, 0, &dwThreadIdArray[0]);
    if (hThreadArray[0] == NULL) {
		std::cerr << "Failed to create the finder thread.\n";
		CloseHandle(hThreadArray[0]);
		return -1;
	}
    hThreadArray[1]= CreateThread(NULL, 0, CounterThread, &arrayData, 0, &dwThreadIdArray[1]);
    if (hThreadArray[1] == NULL) {
		std::cerr << "Failed to create the counter thread.\n";
		CloseHandle(hThreadArray[1]);
		return -1;
	}
	WaitForMultipleObjects(2, hThreadArray, TRUE, INFINITE);
	CloseHandle(hThreadArray[0]);
	CloseHandle(hThreadArray[1]);
	for(int i=0; i<array.size(); ++i){
        if(array[i]== arrayData.min || array[i] == arrayData.max){
            array[i]=arrayData.average;
        }
	}
	std::cout << "Array with the replaced elements:\n";
    for(int i=0; i<array.size(); ++i){
        std::cout<< array[i] << ' ';
    }
    std::cout << '\n';
    return 0;
}
