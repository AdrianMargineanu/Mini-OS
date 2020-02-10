# Mini-OS
This is a simulation of a program manages the run time of other programs on a single core CPU and 4 MiB RAM.

## Introdution
The Operation Systems runs at the same time more programs at the same time and it should manage the memory and run time on the CPU in such way that it all programs to finish. This software allocs the virtual memory for the programs that should be run and and decides what program should run at this moment of time based on the priority and PID.

## Input 
The program reads the input from the a file.The file should have the following commands: 
  - add <mem_size_in_bytes> <exec_time_in_ms> <priority> (that adds a program in the priority queue and the memory of the cumputer)
  - get <PID> (print the information about the program with the sad PID)
  - push <PID> <4_signed_bytes_data> (occupies the sad number of bytes from the programs memory)
  - pop <PID> (free the first 4 bites from the program memory)
  - print stack <PID> (print the memory occupied by the program)
  - print waiting (print the priority queue of programs that waint to be run)
  - print finished (print the information about the programs has finshed)
  - run <some_time> (run a certain time on the CPU)
  - finish (runs all the programs till the program is finished)
  
## Run 
Download the files and use comand "make" to compile. And use this comand to run the program.
```
./minios <input_file> <output_file>
```

  
