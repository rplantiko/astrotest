// Monte Carlo Test for significance of an astrological hypothesis
//
// The hypothesis is
//   "The value of the score function for this particular set
//    of horoscopes is higher than could be expected from chance"
//
// Chance is simulated by producing many random horoscope sets of
// equal size, and computing their score function as well.
//
// The percentage of random sets having equal or higher score
// than the given set, can be considered a signicance estimate
//
// A value < 5% can be considered as a (moderate) confirmation
// of the hypothesis. The smaller, the better the confirmation
//
// (C) 2007-2014 Rüdiger Plantiko

#include <stdbool.h>
#include <time.h>

// We are using the Swiss Ephemeris library on a windows box
#define USE_DLL
#include "\sweph\src\swephexp.h"

// astrotest functions
#include "astrotest.h"
// functions for the LCG random number generator
#include "lcg.h"

// -------------------------------------------------------------------
int main(int argc, char *argv[]) {

  _hordat *hordat;
   int hordat_size;

  initializations(argc,argv,&hordat );
  
// Set ephemeris path if given      
   if (strlen(params.ephepath)>0) {
     swe_set_ephe_path( params.ephepath );
     }  

  double starttime = (double)clock(), endtime;

// Read the set of reference horoscopes from file
// Each line starts with three doubles, separated by ';'
// <jd_ut>;<eastern longitude>;<northern latitude>
  hordat_size = read_source_file( params.source_file, hordat);

// Print values to confirm
  printf("Generating %d random samples of size %d...\n",params.test_runs,hordat_size);
  printf("Using randLCG() with seed values (%ld %ld)\n",s1,s2);

// Compute score of the reference sample
  double refScore = get_total_score( hordat, hordat_size );
  printf( "Reference sample has score: %f\n", refScore );

// Compare #samples times with chance
  int betterScore = cmp2rnd( hordat, hordat_size,
                             params.test_runs,
                             refScore,
                             params.rand_date_fn );

// Result
  printf( "Result: %d samples of %d have a score >= the reference.\np = %2.3f\n",
          betterScore,
          params.test_runs,
          (double)betterScore/params.test_runs );

  endtime = (double) clock();
  printf("Total elapsed execution time: %8.3lf sec\n",(endtime - starttime)/CLOCKS_PER_SEC);

// Leave with return code 0
  return 0;

  }


//--------------------------------------------------------------------
// Core function: Compare against chance
//--------------------------------------------------------------------
  int cmp2rnd( struct _hordat hordat[], // Reference data
               int size,                // Number of reference data
               int sample_size,         // Number of test runs
               double refScore,
               double (*rand_date)(struct _hordat*,int,int)
                                        // Funktion für Zufallszeit
               )
    {

    int betterScore = 0;

    for (int i=0;i<sample_size;i++) {
      double randomScore = 0;
      for (int j=0;j<size;j++) {
// Copy reference horoscope to work area
        struct _hordat testhor = hordat[j];
// New random date
        testhor.jd_ut = (*rand_date)(hordat,size,j);
// Adapt ET, too
        testhor.jd    = testhor.jd_ut + swe_deltat( testhor.jd_ut );
// Compute score
        randomScore += getScoreFn( &testhor);
        }
// Mean total score
      randomScore /= size;
// Now we can say whether random sample #i is better or not:
      betterScore   += (randomScore >= refScore);
      }

    return betterScore;
    }


