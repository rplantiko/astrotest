/* Settings for astrotest.html */

/* Selectable planets, together with their Swiss Ephemeris object number */
var planets = 
  { "0" : "Sun",
    "1" : "Moon",
    "2" : "Mercury",
    "3" : "Venus",
    "4" : "Mars",
    "5" :"Jupiter",
    "6" : "Saturn",
    "7" : "Uranus",
    "8" : "Neptune",
    "9" : "Pluto" 
  };

/* Signs, for zodiacal positions */  
var signs = 
  { "0"   : "Aries",
    "30"  : "Taurus",
    "60"  : "Gemini",
    "90"  : "Cancer",
    "120" : "Leo",
    "150" : "Virgo",
    "180" : "Libra",
    "210" : "Scorpio",
    "240" : "Sagittarius",
    "270" : "Capricornus",
    "300" : "Aquarius",
    "330" : "Pisces"
  };
  
/* Houses, for mundane positions */  
var houses = 
  { "0"   : "I",
    "30"  : "II",
    "60"  : "III",
    "90"  : "IV",
    "120" : "V",
    "150" : "VI",
    "180" : "VII",
    "210" : "VIII",
    "240" : "IX",
    "270" : "X",
    "300" : "XI",
    "330" : "XII"
  };
  
/* Methods to generate random date */
var dateFunctions = {
    "r": "Same date, but random time of the day",
    "R": "Random date in two given intervals",
    "Q": "Random date, in quantiles",
    "f": "Same date, but time from another horoscope",
    "F": "Same date, but LMT from another horoscope (needs is_local_time indicator)"
  };
  
/* Available "raw" score functions */  
var scoreFunctions = {
  "zl"  : "Zod. length in region",
  "zll" : "Zod. aspect",
  "mp"  : "Mund. position in region",
  "mpp" : "Mund. aspect"
  };
 
/* House systems, as supported by the Swiss Ephemeris */
var houseSystems = {
  "P": 	"Placidus",
	"K": 	"Koch",
	"O": 	"Porphyrius",
	"R": 	"Regiomontanus",
	"C": 	"Campanus",
	"E":  "Equal (cusp 1 is Ascendant)",
	"V": 	"Vehlow equal (Asc. in middle of house 1)",
	"W": 	"Whole sign",
	"X": 	"axial rotation system",
	"H": 	"azimuthal or horizontal system",
	"T": 	"Polich/Page (“topocentric” system)",
	"B": 	"Alcabitus",
	"M": 	"Morinus",
	"U": 	"Krusinski-Pisa",
	"G": 	"Gauquelin sectors"  
  }; 
    
/* Some classical aspects */    
var aspects = {
  "0"   : "Conjunction",
  "30"  : "Semi-Sextile",
  "45"  : "Octile",
  "60"  : "Sextile",
  "90"  : "Square",
  "120" : "Trine",
  "150" : "Inconjunct",
  "180" : "Opposition"
  };
  
/* Aspect direction */  
var directions = {
  "+" : "sinister",
  "-" : "dexter",
  "*" : "both directions" 
  };  
       
/* Reference system for aspects (ecliptic = zodiacal, or OA on equator = mundane) */       
var references  = {
  "m" : "mundane",
  "z" : "zodiacal"
  };    
    
  
  
    