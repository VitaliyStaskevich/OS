# Lab5 (Named pipes data handling)
## Compilation
~~~
g++ -o client.exe client.cpp
~~~
~~~
g++ -o server.exe server.cpp
~~~
~~~
server.exe
~~~

Or use a run.bat file

## Functionality
Process-Server and Client Communication via Named Pipes
This program facilitates communication between a server process and multiple client processes using named pipes. The server process manages a binary file containing employee records, while client processes can request modifications or read entries from this file.

### Server Process
The server process, referred to as Process-Server, performs the following tasks:

#### Input File Creation and Data Entry:
The user is prompted to input the filename for the binary file and employee data from the console.
The server creates a binary file based on the provided filename and inputs the employee data into it.
#### Display File:
The server displays the contents of the created binary file on the console.
#### Client Process Handling:
The user specifies the number of client processes to be launched.
The server launches the specified number of client processes, each communicating with the server via named pipes.
#### Client Request Handling:
The server manages client requests as follows:
- If a client requests a modification of a file record, access to that record is locked for other client processes until the modification is complete.
- If a client requests to read a file record, access to that record is blocked for other client processes if they request write access.
#### Display Modified File:
After all client processes have completed their operations, the server displays the modified binary file on the console.
#### Shutdown Command:
The server terminates its execution upon receiving a shutdown command from the console.
#### Client Process
The client process, referred to as Client, executes the following steps:

## Operation Loop:
The client continuously prompts the user to choose an operation: modify a file record, read a record, or exit the loop.
#### Modification Operation:
If the user chooses to modify a record:
- The client prompts for the record's key (employee ID).
- Sends a modification request to the server.
- Displays the record received from the server.
- Prompts for new values for the record fields.
- Sends the modified record to the server upon user command.
- Exits the record access upon user command.
#### Read Operation:
If the user chooses to read a record:
- The client prompts for the record's key (employee ID).
- Sends a read request to the server.
- Displays the record received from the server.
- Exits the record access upon user command.
#### Access Key:
Record access is based on a key, which is the employee ID.
