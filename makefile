CC   = @gcc.bat 
OBJ  = lcg.o parser.o globals.o score.o random_date.o source_file.o debug.o
LIBS =  SWEDLL32.LIB  
BIN  = astrotest.exe
RM = del

.PHONY: all all-before all-after clean clean-custom

all: all-before astrotest.exe tests.exe test.exe all-after

clean: clean-custom
	${RM} $(OBJ) astrotest.o tests.o astrotest.exe tests.exe $(BIN)

astrotest.exe: astrotest.o $(OBJ)
	@rem $(CC) *.o $(LIBS) -o $(BIN)
	mk_astrotest.bat

tests.exe: tests.o $(OBJ)
	mk_tests.bat

astrotest.o: astrotest.c astrotest.h 
	$(CC) -c astrotest.c -o astrotest.o
	
parser.o: parser.c astrotest.h 
	$(CC) -c parser.c -o parser.o
	
globals.o: globals.c astrotest.h 
	$(CC) -c globals.c -o globals.o

random_date.o: random_date.c astrotest.h 
	$(CC) -c random_date.c -o random_date.o
		
source_file.o: source_file.c astrotest.h 
	$(CC) -c source_file.c -o source_file.o
		
score.o: score.c astrotest.h 
	$(CC) -c score.c -o score.o

tests.o: tests.c tests.h astrotest.h
	$(CC) -c tests.c -o tests.o
			
debug.o: debug.c astrotest.h
	$(CC) -c debug.c -o debug.o
			
lcg.o: lcg.c lcg.h	
	$(CC) -c lcg.c -o lcg.o