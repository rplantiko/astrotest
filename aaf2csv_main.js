// Extracts the essential horoscope data from .aaf records
// Usage
// cscript //nologo aaf2csv.wsf <data.aaf >data.csv
// Conversion problems will be reported via stderr

/* 

AAF = astrological exchange format, a format for 
   exchanging horoscope datasets between different computers
   and different software, see definition here:
   http://www.astro-forum.de/aaf/index.htm 
 
This program reads a .aaf file containing horoscope data
and extracts the essential parts of each horscope into 
a .csv line.
 
Here, "essential" means the bare data necessary to recompute
the whole horoscope, i.e. date&time&location.
 
Each result line represents on .aaf record, holding

  <jd_ut>;<lon>;<lat>;<identification>

 where 

 - jd_ut is the Julian Date number, with fractional part 
   representing the time in UT (not ET!), 
   as specified by the AAF formaat
 - lon is the geographical longitude in degrees, 
   eastern longitudes having a '+', 
   western longitudes are '-'.
 - lat is the geographical latitude in degrees, 
   northern latitudes are '+',
   southern latitudes are '-'.

*/

if (WScript.Arguments.length > 0) {
  WScript.Echo("Extra arguments will be ignored\nUsage: cscript //nologo aaf2csv.wsf <source.aaf >target.csv");
  }

var rc = aaf2csv( WScript.StdIn, WScript.StdOut,WScript.StdErr );
WScript.Quit( rc );