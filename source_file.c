//--------------------------------------------------------------------  
// Read the reference sample data  
//--------------------------------------------------------------------  

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// We are using the Swiss Ephemeris library on a windows box
#define USE_DLL
#include "\sweph\src\swephexp.h"

#include "astrotest.h"

int read_source_file( char *filename, struct _hordat hordat[]) { 

  char line[255];
  int i=0;

// Open for reading
  FILE *data = fopen( filename, "r");
  if (data == NULL) {
    fprintf(stderr,"Can't open file %s for reading\n",filename);
    exit(EXIT.FILE_ACCESS_ERROR);
  }

// Read lines
  while(fgets(line,255,data) != NULL) {
    get_data(line, &(hordat[i]) );
    i++;
  }   

  fclose(data); 

  if (i == 0) {
    fprintf(stderr,"Didn't read any entries read from source file\n");
    exit(EXIT.FILE_ACCESS_ERROR);
  }

  printf("Read file %s\n%d records total.\n",filename,i);
  
// Return value = number of lines read
  return i;  
}

//--------------------------------------------------------------------  
// Read data of a single horoscope from the sample file
// Compute Julian Date in ET from UT
//--------------------------------------------------------------------  
void get_data( char *line, struct _hordat *hordat ) {
    
  double lon =0 , lat = 0, jd = 0;
  int is_local_time = 0; // Flag for local time

// Example format
// 2404990.67777778;11.1166666666667;59.2833333333333;Amundsen,Roald
// = just 3 decimals, no separate marker for local time, or...
// 2404990.67777778;11.1166666666667;59.2833333333333;1;Amundsen,Roald
// = 3 decimals + boolean marker for flag: "time is local"
// Rest of the line is arbitrary
  int i=sscanf(line, " %lf ; %lf ; %lf ; %d", &jd, &lon, &lat, &is_local_time);
  if (i < 3) {
    fprintf( stderr, "Read error: Couldn't read three decimals from line '%s'\n", line);
    exit(EXIT.FILE_ACCESS_ERROR); 
  }

// A value for jd of almost or below zero is most likely a scan error  
  if (jd < 0.001) {
    printf( "Error when reading line '%s'\nAborted.\n", line);
    exit(EXIT.FILE_ACCESS_ERROR);
  }
      
  hordat->jd_ut         = jd;
  hordat->lon           = lon;
  hordat->lat           = lat;
  hordat->is_local_time = (is_local_time > 0);
  hordat->jd = hordat->jd_ut + swe_deltat( hordat->jd_ut );
                  
  }

