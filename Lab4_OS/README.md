# Lab4 (Synhronisation of processes)
## Compilation
~~~
g++ -o sender.exe sender.cpp
~~~
~~~
g++ -o receiver.exe receiver.cpp
~~~
~~~
receiver.exe
~~~

Or use a run.bat file
## Functionality
A program for message passing between processes via a shared file. The program should consist of one Receiver process and one Sender process. The Sender process sends messages to the Receiver process.

Requirements for the `Receiver` Process:
- Input from the console the name of the binary file and the number of records in it.
- Create a binary file for messages with a maximum message length of 20 characters.
- Input from the console the number of Sender processes.
- Launch the specified number of Sender processes, passing each the name of the message file.
- Wait for a signal indicating readiness from all Sender processes.
- Cycle through actions as commanded from the console: read a message from the binary file or terminate the process.

Requirements for the `Sender` Process:
- Open the file for sending messages, with the filename obtained from the command line.
- Send a signal to the Receiver process indicating readiness.
- Cycle through actions as commanded from the console: send a message to the Receiver process or terminate its own execution.