# Command Line Library

Command Line is **c++ Linux library** which gives you a fast solution
if you need to call parts of your code during application execution. 
Do that by just typing name of your function an arguments.
This is most useful for early testing and debugging.
It can operate in the same console as your application 
or/and as a network server. It's easy to integrate into your existing code.

# Building

Uses cmake. If you want to cross compile pass export full name of the
compiler under **CXX** env variable. By default **g++** is used.
Project contains recursive submodules, init them with: **git submodule update --init --recursive**
Output files will be placed in "bin_compiler_name" directory:
  - **libcmdline.so** - library
  - **example** - basic integration example
  - **client** - network client
  
# Integration

You should be able to integrate Command Line with your existing code
by editing one  of your source files.
Take a look at example.cpp
What you need is :
  - CmdWrapper.h copied into one of your include directories
  - wrapper initialization 
  - your functions which will actually do something
In result you should end up with your functions being registered as commands.

# Usage

Copy libcmdline.so to your other libraries or set LD_LIBRARY_PATH environment variable.
Start your application.
If you chose to "RunLocal" in your integration then you can type your commands as soon as you will see that Command Line has been started.
If you chose to "RunServer" in your integration then you can run **client** ([server location] port). Your commands typed there will
be sent to your application.

# License

MIT