@rem Batch files can't have more then 9 cmd line arguments, unfortunately
@rem Workaround: Explicit file names
@set PATH=%PATH%;W:\MinGW\bin
gcc.exe -std=gnu99 -Wall tests.o lcg.o parser.o globals.o score.o random_date.o source_file.o debug.o SWEDLL32.LIB -o tests.exe