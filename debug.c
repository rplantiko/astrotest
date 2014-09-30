#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "astrotest.h"

FILE* dbg;

// Debug function, replaces the standard function get_score( )
double get_score_debug( struct _hordat *hordat) {
  
   static int count = 0;
   
   if (!debug_level) {
      return get_score(hordat);
      }
   
   if (++count > 200) {
      fprintf(dbg,"Limit of 200 score evaluations reached.\n Debug-output stopped.\n");
      debug_level = 0;      
      return get_score(hordat);
      }
  
   if (!dbg) {
     dbg = fopen("prot.txt","w");
     if (dbg == NULL) {
       fprintf(stderr,"Couldn't open debug prot file prot.txt for writing");
       exit(EXIT.FILE_ACCESS_ERROR);
       }
     } 
  
   double score = 0;
   
   reset_planet_buffer( );

   fprintf( dbg, "(%f %f %f)\n  ", hordat->jd, hordat->lon, hordat->lat );   
   for (int i=0; i<scoreFunction.size;i++) { 
     fprintf(dbg,"%5.2f ",scoreFunction.terms[i].f( hordat, & scoreFunction.terms[i].args ));
     score += scoreFunction.terms[i].f( hordat, & scoreFunction.terms[i].args );
     }
   fprintf( dbg, "\n  sum = %f\n", score );     
   
   return (double) score;  
  
  }