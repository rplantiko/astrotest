@rem Batch files can't have more then 9 cmd line arguments, unfortunately
@rem Workaround: Explicit file names
@set PATH=%PATH%;W:\MinGW\bin
gcc.exe -std=gnu99 -Wall test.o SWEDLL32.LIB -o test.exe