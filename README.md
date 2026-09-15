# Custom_OS
Custom operating system I wrote to see what the basics of OS development include

## Overview

a Small UNIX based terminal operating system with a few basic commands such as "help" to list all possible commands and "cd" to move between directories, "clear" to start with a blank screen as well as "echo" to repeat what was typed in. These are just a few examples. I created this project to get an insight to how some operating systems work and how some procedures are handled. This is a small simulation of how a real operating system would function.

## Features

- Custom boot loader
- Custom operating system
- basic commands and instance file system

## Technologies Used

- C
- GCC Compiler
- QEMU Virtual Machine

## How It Works

When use the make file to create a .iso file of the OS, if the .iso already exists and no changes were made then use "make run" to start the virtual machine. The OS can be used standalone on any device as it has its own boot file, but be cautious as the boot file is not optimized at all and might cause some errors. Preferably use on a virtual machine for safety.

## Commands

Operating System starts at the root directory "r >".
- help: display a list of all commands that can be used.
- ls: list all files within the current directory (ls dir: lists all directories. ls file: lists all files)
- echo: prints input to the terminal (echo Hello World!)
- clear: clears the screen and start with a fresh new console.
- mkfile: creates a new file with the name given (mkfile Test.txt)
- mkdir: creates a new directory with the name given (mkdir Documents)
- cd: changes the current directory to a given directory (cd Documents) and can also move back one directory (cd ..)
- write: Writes information to a file (write Test Info > Test.txt) and appends information to the end of a file (write Extra test info >> Test.txt)
- read: Read the contents of a file (read Test.txt)
- reboot: restarts the virtual machine and clears instance directories and files.
- shutdown: terminates the virtual machine.

## What I Learned

# What I learned in the making of this project
- How terminal based operating systems work and handle input
# Problems I encountered in the making of this project
- Input buffer overflowing if the user inputs more than 256 characters
- Input handling with multi part commands

## Future Improvements

- Persistent storage system
- Different file types
- File/text editor
- Built in compiler for code
