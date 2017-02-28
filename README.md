# Command Line Library

Command Line is **c++ linux library** which gives you a fast solution
if you need to call parts of your code during application execution. 
Do that by just typing name of your function an arguments.
This is most usefull for early testing and debugging.
It can operate in the same console as your application 
or/and as a network server. It's easy to integerate into your existing code.

# Building

Run build.sh script. If you want to cross compile pass full name of the
compilator in script's argument. By default **g++** is used.
Output files will be placed in "bin_compilator_name" directory:
  - **libcmdline.so** - library
  - **test** - example of basic integration
  - **cmd_client** - network client 

# Integration

You should be able to integrate Command Line with your existing code
by editing one  of your source files.
Take a look at wrapper_and_usage_example.cpp
What you need is :
  - CmdLine library wrpapper 
  - wrapper initialisation 
  - your functions which will actually do something

In result you should end up with your functions being registered as commands.

# Usage

Copy libcmdline.so to your other libraries or set LD_LIBRARY_PATH environment variable.

Start your application.

If you chose to "run_local" in your integration then start typing your commnads as soon
as you will see that Command Line has been started

If you chose to "run_server" in your integration then as you will see that Command Line Server has been started
sucesfully you can run **cmd_client** ([server location] port). Your commands typed there will
be sent to your application.

# Licence

MIT