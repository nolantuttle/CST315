Nolan Tuttle & Mathew Hobson	
4/6/25
Professor Citro
Project 5: Short-Term Process Scheduler

Github Repository Link: https://github.com/nolantuttle/CST315.git

Prerequisites:
To run the program, first ensure that the GCC compiler is installed on your Linux/UNIX device. This can be done by running the following commands:

1. Update package list:

sudo apt-get update

2. Install the build-essential package:

sudo apt install build-essential 


Compilation Instructions:

To compile the seaShell program on a Linux/UNIX operating system, use the following command in this directory's terminal:

gcc -o run seaShell.c 


Running the Program:

To run the executable program that was generated, run the following command:

./run


It is important to note that we added 3 demonstration processes that run idly: "sleep 2", "echo idle1 > /dev/null", and "sleep 4". When a command is entered, it will be put at the end of the queue with these commands in front, showing the process table each time a context switch is performed. Also, the ‘procs’ command will run and then exit the program when used.
