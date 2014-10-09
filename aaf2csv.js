// Extracts the essential horoscope data from .aaf records
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

function aaf2csv(stdin,stdout,stderr) {
  
  var record, counter = 0;
  
  while ((record = readNextRecord(stdin))) {            
    counter++;
    try {
      result = convert( record );
      stdout.WriteLine( 
          result.jd_ut.toFixed(7) + ";" 
        + result.lon.toFixed(5) + ";" 
        + result.lat.toFixed(5) + ";" 
        + result.identifier );    
      } catch (e) {
          var msg = ( e instanceof Error) ? e.description || e.message : e;
          stderr.WriteLine( record.A93 );
          stderr.WriteLine( ">>> " + msg );
          return 8; // FAILURE
          }        
  }    
  if (counter == 0) {
    stderr.WriteLine( "No AAF records detected" );
    return 4;  // Warning
    }
    
  return 0;  // OK
  
  function convert(record) {
    
    result = {};
    
    if ( !("B93" in record) || (record.B93[1] == '*') || (record.B93[2] == '*') ) {
      throw "Missing or incomplete B93 record";
      }
    
    result.lon = parseLongitude( record.B93[2] );
    result.lat = parseLatitude( record.B93[1] );
    
    result.identifier = getIdentifier( record.A93 );
    result.jd_ut = getJulianDate( record );
    
    return result;
    
    }

  
  
  var nextLine = "";  // Needed as read buffer
  
  function readNextRecord() {
    var record = {};  
    while (!stdin.AtEndOfStream) {
      if (readNextLine()=="break") break;
      }
    if (!("ZNAM" in record)) record.ZNAM = "*";  
    return ("A93" in record) ? record : null;  
    
    function readNextLine() {
      var line = readLine();
      // Only lines starting with #A93 or #B93 specifier
      if (!(parts = line.match(/\s*#(A93|B93|ZNAM|SUB):\s*(.*)$/))) return;
      // #A93 not yet reached: Continue
      if (!("A93" in record) && (parts[1] != "A93")) return;
      // #SUB: Stop
      if (parts[1]=="SUB") return "break";
      // Reached #A93 of *next* record: stop
      if (("A93" in record) && (parts[1] == "A93")) {
        nextLine = line;  // Preserve read line of next record
        return "break";
        }
      record[parts[1]] = parts[2].split(/\s*,\s*/);
      if (("ZNAM" in record) && ("B93" in record)) return; // Done
    }  
  
  }

// Read line from input, sometimes with a one-line lookahead
  function readLine() {
    var line;
    if (nextLine) {
      line = nextLine;
      nextLine = "";
    }
    else {
      line = stdin.ReadLine(); 
    }       
    return line;
  } 
   
} 

// Sexagesimal parser for latitude
function parseLatitude(latString) {
  return parseDMS(latString,"N","S",":","latitude");
}

// Sexagesimal parser for longitude
function parseLongitude(lonString) {
  return parseDMS(lonString,"E","W",":","longitude");  
}


// Sexagesimal parser for time difference
function parseTimeZoneDiff(zdiffString) {
  return parseDMS(zdiffString,"HE","HW",":","time zone diff");
  }  

// Read time  
function getTime( tstring ) {
  var time = parseDMS( tstring, "h:","","m:","time");
  return time;
  }
  
// General sexagesimal parser
function parseDMS(dmsString,dsep_plus,dsep_minus,msep,term) {
  var m = dmsString.match(/(\d{1,3})([^\d\s])(\d{1,2})(?:([^\d\s])(\d{1,2}))?/);
  if (m && m.length > 2) {
    var value = m[1] * 1 + 
                m[3] / 60 + 
                ((m[4] && 
                 msep.indexOf(m[4].toUpperCase())>-1 && 
                 m[5] ) ? 
                  m[5] / 3600 
                  : 0);
    
    var sign = m[2].toUpperCase( );
    if (dsep_plus.indexOf(sign)>-1) return value;
    if (dsep_minus.indexOf(sign)>-1) return -value;
    }
  throw "Can't parse " + term + " value '" + dmsString + "'";  
  }
  

// Build full name from A93[0] and A93[1] as record identifier   
function getIdentifier( A93 ) {  
  return A93[1] == "*" ? 
     A93[0] : 
     A93[0] + ", " + A93[1].replace(/^\s*/,"");
  }  
  
// Read Julian Date  
function getJulianDate( record ) {
  var juldat = record.B93[0];
  if (juldat!='*' && !isNaN(parseFloat((juldat))))
// If the Julian Date is specified in the record: Retrieve it  
    return juldat*1;
  else 
// B93[0] == '*': Force recomputation of the Julian Date
    return computeJulianDate( record );
  }  
  
// Forced computation of Julian Date  
function computeJulianDate( record ) {
  
  var date = record.A93[3].match(/(\d+)\.(\d+)\.(\d{4})\s*([gj])?/i);
  if (!date) {
    throw "Can't parse date from '" + record.A93[3] + "'";
    }

  var gregflag = (date[4] && date[4].toLowerCase() == "g") || 
     (date[3]*1 + date[2]/12 + date[1]/31 > 1583.285);
  var time = getTime( record.A93[4] ) - getTimeDiff( record );  
  return swe_julday(date[3],date[2],date[1],time,gregflag);
  }
  

// Julian Date from calendar date (adjusted from swedate.c in Swiss Ephemeris)  
function swe_julday(year, month, day, ut, gregflag) {
  var jd;
  var u,u0,u1,u2;
  u = year*1;
  if (1*month < 3) u -=1;
  u0 = u + 4712.0;
  u1 = month*1 + 1.0;
  if (u1 < 4) u1 += 12.0;
  jd = Math.floor(u0*365.25)
     + Math.floor(30.6*u1+0.000001)
     + 1*day + ut/24.0 - 63.5;
  if (gregflag) {
    u2 = Math.floor(Math.abs(u) / 100) - Math.floor(Math.abs(u) / 400);
    if (u < 0.0) u2 = -u2;
    jd = jd - u2 + 2;            
    if ((u < 0.0) && (u/100 == floor(u/100)) && (u/400 != floor(u/400)))
      jd -=1;
  }
  return jd;
}

// Returns the time zone difference to UT in hours
function getTimeDiff(record) {
  
  if (record.ZNAM == "*" || record.ZNAM == "" || record.ZNAM == "LMT") {    
    return parseTimeZoneDiff( record.B93[3] );
    }
  else if (record.ZNAM in getTimeDiff.timeZones) {
    return (
      getTimeDiff.timeZones[record.ZNAM] +
      getDaylightSavingTime( record.B93[4] )
      );
    }
  else {
    throw "Time zone '" + record.ZNAM + "'not implemented\n";
    }  
    
}

// Add more time zones if needed!
getTimeDiff.timeZones = {
  GMT : 0,
  MET : 1
  };

// DST indicator
function getDaylightSavingTime(dst_indicator) {
  var i = "120m*L".indexOf(dst_indicator);
  if (i>-1) return i>1 ? 0 : i+1;
  throw "Unknown daylight saving time indicator '" + dst_indicator + "'";
  }