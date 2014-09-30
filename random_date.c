// Functions of type randomDateFunction
// Different ways of producing a random date from a given one

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "astrotest.h"
#include "lcg.h"

//--------------------------------------------------------------------  
// Function 'r': Same date, but random time of the day
//--------------------------------------------------------------------  
double random_time(struct _hordat hordat[], int size, int j) {
  return floor( hordat[j].jd_ut ) + randLCG();
  }         
  
//--------------------------------------------------------------------  
// Function 'R': Generate a completely random sample
// To roughly respect the different population densities,
// we use two date ranges 
// (the concrete dates can be overwritten with parameter jd,
//  which expects three doubles)
// Range 1: 1500 up to <1805 (2268932-2380334)  
// Range 2: 1805 up to 1970  (2380334-2440964)
// Dates < 1805 get a random date in range1
// Dates >= 1805 get a random date in range2
//--------------------------------------------------------------------
double random_date(struct _hordat hordat[], int size, int j) {
  double day = 
     (hordat[j].jd_ut < params.jd2) ?  
       (params.jd2-params.jd1)*randLCG()+params.jd1 : 
       (params.jd3-params.jd1)*randLCG()+params.jd2;
  return day + randLCG(); // Another random number for the time of day    
  }         
  
//--------------------------------------------------------------------  
// Function 'Q': Like 'R', but with quantiles computed from the data
// The test plan parameter 'quantile' defines the number of quantiles
// For each date, a random date located in its own quantile is generated
//--------------------------------------------------------------------  
_date_quantiles quantiles;  
static void prepare_quantiles( _hordat hordat[], int size) {
  quantiles.date = malloc( (quantiles.size + 1 ) * sizeof( double ) );
// Create a temporary copy of hordat on the stack, for sorting purposes
  _hordat s_hordat[size];  
  memcpy( (void *) s_hordat, (void*) hordat, size*sizeof( _hordat ) );
// Define the comparator for hordat items:
  int cmphor( const void* _h1, const void* _h2) {
    const _hordat* h1 = (const _hordat *) _h1;
    const _hordat* h2 = (const _hordat *) _h2;
    return h1->jd < h2->jd ? -1 : ( h1->jd == h2->jd ? 0 : +1 );
    }  
  qsort( s_hordat, size, sizeof( _hordat ), cmphor );  
  quantiles.date[0]              = s_hordat[0].jd; 
  quantiles.date[quantiles.size] = s_hordat[size-1].jd; 
  for (int i=1; i<quantiles.size; i++) {
    int n = ceil( size * i / quantiles.size );
    if (( (size * i) % quantiles.size == 0) && (n+1 < size)) {
      quantiles.date[i] = (s_hordat[n].jd+s_hordat[n+1].jd)/2;
      }
    else {
      quantiles.date[i] = s_hordat[n].jd;
      }    
    }
  
  if (debug_level) {
    fprintf(dbg, "Quantile dates:\n");
    for (int i=0;i<=quantiles.size;i++) {
      fprintf(dbg,"  %15.5f\n",quantiles.date[i]);
      }
    }
  
  }
  
double random_date_by_quantile(struct _hordat hordat[], int size, int j) {    
  
  if (j==0) {
    prepare_quantiles( hordat, size );    
    }
  
  int i;
  for (i=1; i<quantiles.size; i++) {
    if (quantiles.date[i] > hordat[j].jd) {
      break;
      }
    }
    
  return quantiles.date[i-1]
           + (quantiles.date[i]-quantiles.date[i-1]) * randLCG( )
           + randLCG( );  // For the time
  
  }


//--------------------------------------------------------------------  
// Function 'f': Same date, but time from another horoscope
//--------------------------------------------------------------------  
double random_shuffle(struct _hordat hordat[], int size, int j) {
  int i = randLCG()*size; 
  double jd_alt = hordat[i].jd_ut;
  return floor( hordat[j].jd_ut ) + (jd_alt - floor(jd_alt));    
  }         
  
//--------------------------------------------------------------------  
// Function 'F': Randomize local mean daytime
//--------------------------------------------------------------------  
double random_shuffle_lt(struct _hordat hordat[], int size, int j) {
  int i = randLCG()*size; 
  double jd_alt = hordat[i].jd_ut;
// For local time:
// Switch to local time of the other horoscope
// For non-local time:
// Switch to non-local time of the other horoscope
  if (hordat[i].is_local_time) {  
// Local time of the other horosocope:  
    double lt_alt = (jd_alt - floor(jd_alt))*360 + hordat[i].lon; 
// date j, but with local time of date i, corrected to longitude j
    return floor( hordat[j].jd_ut ) + (lt_alt - hordat[j].lon)/.36e3;    
    }
  else {
// Not local time:     
// Current date, but time UT of another horoscope       
    return floor( hordat[j].jd_ut ) + jd_alt - floor(jd_alt);
    }      
  }         
  
