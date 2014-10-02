// Functions around the computation of the score at run-time
// Including Buffering of planet and house computations

#include <stdio.h>
#include <stdbool.h>
#include "astrotest.h"
#include "lcg.h"
#include <assert.h>

// We are using the Swiss Ephemeris library on a windows box
#define USE_DLL
#include "\sweph\src\swephexp.h"

//--------------------------------------------------------------------  
// Buffer for planet data
//--------------------------------------------------------------------  
_planet_data *pl_buf,*mp_buf;
double jd_pl_buf, mp_buf_armc, mp_buf_eps;
_hordat hor_mp_buf;
double (*getScoreFn)(struct _hordat*) = get_score;  

//--------------------------------------------------------------------  
// The single score function 
//--------------------------------------------------------------------  
double get_score( struct _hordat *hordat) {

   double score = 0;
   
   reset_planet_buffer( );
   
   for (int i=0; i<scoreFunction.size;i++) { 
     score += scoreFunction.terms[i].f( hordat, & scoreFunction.terms[i].args );
     }
   
   return (double) score;

  }  
  

   
//--------------------------------------------------------------------  
// Get the medium score for a set of horoscopes
//--------------------------------------------------------------------  
  double get_total_score( struct _hordat hordat[], 
                          int size) {
    double total = 0;
    
    for (int i=0;i<size;i++) {  
      total += getScoreFn( &(hordat[i]) );
      }    

    return total / size;

    }                               

//--------------------------------------------------------------------  
// Difference x-y of two angular values on the circle
// Result is reduced to semi-open interval [-180,+180[
//--------------------------------------------------------------------  
double arcdiff(double x, double y) {
  double d = (x-y)/.36e3+.5;  
  return (d-floor(d))*.36e3-.18e3;
  }
  
//--------------------------------------------------------------------  
// Reduce a degree value to the interval 0..360
//--------------------------------------------------------------------  
double red360(double x) {
  return (x-floor(x))*360;
  }  
   
//--------------------------------------------------------------------  
// Compute planetary longitude for a jd_et
// Buffer the results
//--------------------------------------------------------------------  
double zodiacalLength( int pl, double jd_et) {
  
  int i;     
  long iflag=0, iret=0;
  double xx[6];  
  char serr[256];
  
  assert(pl_buf != 0); // Buffer for planetary positions not assigned

// Switching to a new jd: reset buffer  
  if (jd_et != jd_pl_buf) {
    jd_pl_buf = jd_et;      
    for (i=0;i<scoreFunction.cdata.pl2ipl_size;i++) pl_buf[i].computed = false;         
    }
// Compute longitude if necessary    
  if (!pl_buf[pl].computed) {
    iret = swe_calc(jd_et, scoreFunction.cdata.pl2ipl[pl], iflag, xx, serr);
    if ( iret < 0 || 
         ( iret > 0 && ! params.continue_with_moshier ) ) {
      fprintf(stderr, "%s\n", serr);
      exit(EXIT.EPHEMERIS_CALL_ERROR);
      }
    pl_buf[pl].x = xx[0];
    pl_buf[pl].computed = true;
    }
  return pl_buf[pl].x; 
  }

//--------------------------------------------------------------------  
// Compute the house position, as mundane position, 
// i.e. 0 = cusp I, 90 = cusp IV, 180 = cusp VII, 270 = cusp X
//--------------------------------------------------------------------  
double mundanePosition( int pl, _hordat* hor) {

  int rc;
	double xpin[2],xx[6];
	char serr[256];

  assert(mp_buf != 0); // Buffer for mundane positions not assigned

// Switching to a new horoscope: reset buffer  
  if (!memcmp(hor,&hor_mp_buf,sizeof(_hordat))) {
    hor_mp_buf = *hor;      
    for (int i=0;i<scoreFunction.cdata.pl2ipl_size;i++) {
      mp_buf[i].computed = false;         
      }
    mp_buf_armc = red360( swe_sidtime( hor->jd_ut )*15 + hor->lon );
    rc = swe_calc(hor->jd, SE_ECL_NUT, 0, xx, serr);
    if (rc!=0) {
      fprintf(stderr,"%s",serr);
      exit(EXIT.EPHEMERIS_CALL_ERROR);
      }
    mp_buf_eps = xx[0];
    }
    
// Compute mundane position if necessary    
  if (!mp_buf[pl].computed) {    
    xpin[0] = zodiacalLength( pl, hor->jd );
    xpin[1] = 0;
    mp_buf[pl].x = swe_house_pos( mp_buf_armc, hor->lat, mp_buf_eps, params.hsys, xpin, serr);
    mp_buf[pl].computed = true;        
    }

  return mp_buf[pl].x;

  }


double zodiacalLengthInRange(_hordat* hor, _termFunctionArgs* args) {  
  double l = zodiacalLength( args->pl1, hor->jd );
  if (debug_level) {
    fprintf(dbg,"      l:%6.2f, in range (%.2lf-%.2lf): %s\n",
      l,args->start,args->end, 
      (arcdiff(l,args->start)>0 && arcdiff(args->end,l)>0) ? "yes" : "no" );
    }  
  return arcdiff(l,args->start)>0 && arcdiff(args->end,l)>0;
  }


double zodiacalAspect(_hordat* hor, _termFunctionArgs* args) {
  double 
    l1 = zodiacalLength( args->pl1, hor->jd ),
    l2 = zodiacalLength( args->pl2, hor->jd ),
    diff = arcdiff(l2,l1);          
    if (debug_level) {
      fprintf(dbg,"      l1:%6.2f, l2:%6.2f, diff: %6.2f, aspect (%.2f-%.2f) holds: %s\n",
        l1, l2, diff, args->start, args->end,
        (diff > args->start && diff < args->end) ? "yes" : "no" );
      }  
    return diff > args->start && diff < args->end; 
  }  

double mundanePositionInRange(_hordat* hor, _termFunctionArgs* args) {
  double mp = mundanePosition( args->pl1, hor );  
  if (debug_level) {
    fprintf(dbg,"      mp:%6.2f, in range (%.2f-%.2f): %s\n",
      mp, args->start, args->end, 
      (arcdiff(mp,args->start)>0 && arcdiff(args->end,mp)>0) ? "yes" : "no" );
    }  
  return arcdiff(mp,args->start)>0 && arcdiff(args->end,mp)>0;
  }
  
double mundaneAspect(_hordat* hor, _termFunctionArgs* args) {
  double 
    mp1 = mundanePosition( args->pl1, hor ),
    mp2 = mundanePosition( args->pl2, hor ),
    diff = arcdiff(mp2,mp1);          
    if (debug_level) {
      fprintf(dbg,"      mp1:%6.2f, mp2:%6.2f, diff: %6.2f, m. aspect (%.2f-%.2f) holds: %s\n",
        mp1, mp2, diff, args->start, args->end,
        (diff > args->start && diff < args->end) ? "yes" : "no" );
      }  
    return diff > args->start && diff < args->end; 
  }

// --- For a single computation, reset the planet buffer
void reset_planet_buffer() {
  if (!pl_buf) {
    pl_buf = malloc( scoreFunction.cdata.pl2ipl_size * sizeof( _planet_data ) );
    }
  if (!mp_buf) {
    mp_buf = malloc( scoreFunction.cdata.pl2ipl_size * sizeof( _planet_data ) );
    }
  for (int i=0;i<scoreFunction.cdata.pl2ipl_size;i++) {
    pl_buf[i].computed = mp_buf[i].computed = false; 
    }
  jd_pl_buf = 0.;
  hor_mp_buf = (_hordat) {0};
  mp_buf_eps = mp_buf_armc = 0;
  }  
  