// Global variables and constants
#include <stdio.h>
#include <stdbool.h>
#include "astrotest.h"

// Current version of this software
const double VERSION = 0.9;

_params params = {
  .source_file = "source.csv",
  .ephepath  = "",
  .test_runs = 1000,
  .ref_size  = 2000, // Initial value to allocate for ref data lines
  .hsys = 'P',      // Placidus houses are default
  .rand_date_fn = random_date,  // Default: random date in given interval
  .jd1 = 2268932.,  // Start of interval 1 in random date function 'R'
  .jd2 = 2380334.,  // Start of interval 2 ...
  .jd3 = 2440964.   // End of interval 2
  };  
 _scoreFunction scoreFunction = {};
 

// different error codes
const _EXIT EXIT = {
    .ALLOC_ERROR          = 1,
    .FILE_ACCESS_ERROR    = 2,
    .SYNTAX_ERROR         = 3,
    .VERSION_ERROR        = 4,
    .UNKNOWN_HOUSE_SYSTEM = 5,
    .MISSING_PARAMETERS   = 6,
    .PARAMETER_ERROR      = 7,
    .EPHEMERIS_CALL_ERROR = 8
  };
  
const _ALLOCDATA SCORE_FUNCTION  = { 
      .INITIAL_SIZE = 100,
      .DELTA_SIZE   = 100
    };

const _ALLOCDATA PL2IPL  = { 
      .INITIAL_SIZE = 100,
      .DELTA_SIZE   = 100
    };
    
int debug_level = 0;    