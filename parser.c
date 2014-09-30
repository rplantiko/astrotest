// --- Parsing the execution plan, preparing the score function

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "astrotest.h"
#include "lcg.h"

// Set initial parameters
void initializations( int argc, char* argv[], _hordat* *hordat ) {
  
  FILE *plan;
  char source_file[255], plan_file[255];
  
  // Read in the (few) command line options
  get_cmdline_options(argc,argv,source_file,plan_file);  
  if (plan_file[0]!='\0') {
    if (!(plan = fopen( plan_file, "r"))) {
      fprintf(stderr,"Can't open file '%s' for reading\n",plan_file);
      exit(EXIT.FILE_ACCESS_ERROR);
      }
    }
  else 
    plan = stdin;
  
  read_plan(plan,&params,&scoreFunction);
  
  if (plan != stdin) fclose(plan);

// Command line is stronger (if given)
  if (source_file[0]!='\0') strcpy( params.source_file,source_file);
   
// More checks    
  if (scoreFunction.size == 0) {
    fprintf(stderr,"No score function given");
    exit(EXIT.MISSING_PARAMETERS);
    }

// time/location tuples for horoscopes
  *hordat = malloc(params.ref_size*sizeof(_hordat));
  if (*hordat == NULL) {
    fprintf(stderr,"Could not allocate %d bytes in heap space\n",params.ref_size*sizeof(_hordat));
    exit(EXIT.ALLOC_ERROR);
    }
  
// Attach debug score function if in debug mode
  if (debug_level > 0) {
    getScoreFn = get_score_debug;
    }
  
  }

// Read execution plan 
void read_plan(FILE* plan, _params* params, _scoreFunction* scoreFunction) { 
  
  char line[255];
  char parsing_block[255];
  
  *parsing_block = '\0';
  while(fgets(line,255,plan) != NULL) {
    parse_single_line(line,parsing_block,params,scoreFunction);    
    }
}      
  

void parse_single_line( char* line, char* parsing_block, _params* params, _scoreFunction* scoreFunction) {
  
    char *s;
  
    if (line[0]=='#') return;  // Skip comments
    
// Detect (block)-begin
    if (  (s = strstr(line,"-begin")) && 
         ((*(s+6)=='\0')||(*(s+6)=='\n'))) {        
      strncpy(parsing_block,line,s-line);
      parsing_block[s-line] = '\0';
      if (strcmp(parsing_block,"score")==0) {  
        scoreFunction->size  = 0;
        scoreFunction->terms = malloc( SCORE_FUNCTION.INITIAL_SIZE * sizeof( _scoreFunctionTerm ) );
        scoreFunction->max_size = SCORE_FUNCTION.INITIAL_SIZE;
        }
      return;
    }
   
// Detect (block)-end    
    if ((s = strstr(line,"-end")) && 
        ((*(s+4)=='\0')||(*(s+4)=='\n'))) {        
      *parsing_block = '\0';
      return;
    }
        
    if (*parsing_block) { 
// Handle line in a block        
      if (strcmp(parsing_block,"score")==0) {
        parse_score_function(line,scoreFunction);
        }
      }  
    else {      
// Line outside of a block        
      parse_plan_parameter(line,params);
      }    
    
}




void parse_plan_parameter(char* line, _params* params) {
  
  char name[50],value[255];
  auto void do_version(),do_hsys(),do_cont_with_mosh(),do_jd(),
            do_rand_date_fn(),do_test_runs(),do_seed(),do_source_file(),
            do_quantiles(),do_dbg_level(),do_ephepath( );
  auto bool check_single_param(char*,void(*)());
  
  if (sscanf(line," %[^:] : %s",name,value)==2) {
    if (check_single_param("version",do_version)) return;
    if (check_single_param("source-file",do_source_file)) return;
    if (check_single_param("test-runs",do_test_runs)) return;
    if (check_single_param("hsys",do_hsys)) return;
    if (check_single_param("cont-with-mosh",do_cont_with_mosh)) return;
    if (check_single_param("jd",do_jd)) return;
    if (check_single_param("quantiles",do_quantiles)) return;      
    if (check_single_param("rand-date-fn",do_rand_date_fn)) return;
    if (check_single_param("seed",do_seed)) return;
    if (check_single_param("dbg-level",do_dbg_level)) return;
    if (check_single_param("ephepath",do_ephepath)) return;
    printf("Warning: Unknown parameter '%s'\n",name);
    }
  
  bool check_single_param( char* exp_name, void(do_param)()) {
    if (strcmp(name,exp_name)==0) {
      do_param();
      return true;
      }
    return false;  
  }

  void do_version( ) {
    if (sscanf(value,"%lf",&(params->version))==0) {
      fprintf(stderr,"Couldn't parse version number '%s'\n",value);
      exit(EXIT.SYNTAX_ERROR);
    }
    if (params->version > VERSION) {
      fprintf(stderr,"Version %f not supported\n",params->version);
      exit(EXIT.VERSION_ERROR);
    }  
  }  
    
  void do_hsys( ) {
    if (strchr("PKORCAEVXHTBG",value[0])==NULL) {
      fprintf(stderr,"House system '%c' not supported\n",value[0]);
      exit(EXIT.UNKNOWN_HOUSE_SYSTEM);
      }
    params->hsys = value[0];  
  }      
  
  void do_cont_with_mosh( ) {
    int i;
    params->continue_with_moshier = (sscanf(value,"%d",&i)>0 && i>0);  
  } 
  
  void do_jd() {
    if (sscanf(value," %lf , %lf , %lf", 
          &(params->jd1),
          &(params->jd2),
          &(params->jd3) ) <3 ) {
      fprintf(stderr,"Couldn't read three Julian Date values from '%s'\n",value);
      exit(EXIT.MISSING_PARAMETERS);
      }
    }
    
  void do_quantiles() {
    if (sscanf(value," %d ",&quantiles.size)<1) {
      fprintf(stderr,"Couldn't read an integer value for number of quantiles from '%s'\n",value);
      exit(EXIT.MISSING_PARAMETERS);
      }
    if (quantiles.size==0) {
      fprintf(stderr,"0 is an illegal value for number of quantiles");
      exit(EXIT.PARAMETER_ERROR);
      }  
    }  
    
  void do_rand_date_fn() {
    params->rand_date_fn = getRandomDateFunction(value[0]);        
    }  
    
  void do_test_runs() {
    if (sscanf(value,"%d",&params->test_runs)!=1) {
      fprintf(stderr,"Can't read integer value in '%s'\n",value);
      exit(EXIT.MISSING_PARAMETERS);
      }
    }  
    
  void do_seed() {
    if (sscanf(value,"%ld , %ld",&s1,&s2)!=2) {
      fprintf(stderr,"Can't read two integer values for seed in '%s'\n",value);
      exit(EXIT.MISSING_PARAMETERS);
      }    
    }  
    
  void do_source_file() {
    if (sscanf(value," %s ",params->source_file)!=1) {
      fprintf(stderr,"Can't read one string for source file parameter in '%s'\n",value); 
      exit(EXIT.MISSING_PARAMETERS);
    }  
  }
  
  void do_dbg_level() {
    if (sscanf(value," %d ",&debug_level)!=1) {
      fprintf(stderr,"Debug level must be an integer '%s'\n",value); 
      exit(EXIT.PARAMETER_ERROR);
    }  
    if (debug_level < 0) {
      fprintf(stderr,"Debug level must be a positive integer '%s'\n",value); 
      exit(EXIT.PARAMETER_ERROR);
      }
    }  
    
  void do_ephepath( ) {
    if (sscanf(value," %[^\n] ",params->ephepath)!=1) {
      fprintf(stderr,"Can't read one string for ephepath parameter in '%s'\n",value); 
      exit(EXIT.PARAMETER_ERROR);
      }
    }  
}

randomDateFunction getRandomDateFunction(char key) {
  
   switch (key) {     
// Select the desired random date function
     case 'r':
       printf("Random date function: Choose random time\n");                       
       return random_time;
      case 'R':
       printf("Random date function: Choose random date and time\n");                       
       return random_date;
      case 'Q':
       printf("Random date function: Random date by quantile\n");                       
       return random_date_by_quantile;
      case 'f':      
       printf("Random date function: Choose random time\n");                       
       return random_shuffle;
      case 'F':      
       printf("Random date function: Choose random shuffle\n");                       
       return random_shuffle_lt;
     default:         
       fprintf(stderr,"Unknown code '%c' for random date function\n",key);
       exit(EXIT.MISSING_PARAMETERS);
     } 
}



void parse_score_function( char* line, _scoreFunction* scoreFunction) {

  _scoreFunctionTerm term = {};
  int ipl1, ipl2;
  char fname[50],args[150],endc;
  
  if (sscanf(line," %lf * %[^(] ( %[^)] %c",&(term.coeff),fname,args,&endc)!=4
        || endc != ')') {
    fprintf(stderr,"Incorrect syntax in line '%s'\n", line);
    exit(EXIT.SYNTAX_ERROR);
    }
  
  if (fabs(term.coeff) < 1.e-8) {  // Wrongly scanned decimal often gives a value near 0
    printf("Term with zero coefficient will be ignored: '%s'\n",line);
    }
  
  if (strcmp(fname,"zl")==0) {
    term.f = zodiacalLengthInRange;
    if (sscanf(args," %d , %lf , %lf", 
          &ipl1, 
          &term.args.start, 
          &term.args.end)!=3) {
      fprintf(stderr,"Wrong number of arguments for zl function in '%s'\n",line);
      exit(EXIT.SYNTAX_ERROR);
      }
    term.args.pl1 = get_pl( ipl1 );      
    } 
  
  if (strcmp(fname,"mp")==0) {
    term.f = mundanePositionInRange;
    if (sscanf(args," %d , %lf , %lf", 
          &ipl1, 
          &term.args.start, 
          &term.args.end)!=3) {
      fprintf(stderr,"Wrong number of arguments for zl function in '%s'\n",line);
      exit(EXIT.SYNTAX_ERROR);
      }
    term.args.pl1 = get_pl( ipl1 );      
    } 
  
  if (strcmp(fname,"zll")==0) {
    term.f = zodiacalAspect;
    if (sscanf(args," %d , %d , %lf , %lf", 
          &ipl1, 
          &ipl2, 
          &term.args.start, 
          &term.args.end)!=4) {
      fprintf(stderr,"Wrong number of arguments for zll function in '%s'\n",line);
      exit(EXIT.SYNTAX_ERROR);
      }
    term.args.pl1 = get_pl( ipl1 );      
    term.args.pl2 = get_pl( ipl2 );           
    } 
  
  if (strcmp(fname,"mpp")==0) {
    term.f = mundaneAspect;
    if (sscanf(args," %d , %d , %lf , %lf", 
          &ipl1, 
          &ipl2, 
          &term.args.start, 
          &term.args.end)!=4) {
      fprintf(stderr,"Wrong number of arguments for zll function in '%s'\n",line);
      exit(EXIT.SYNTAX_ERROR);
      }
    term.args.pl1 = get_pl( ipl1 );      
    term.args.pl2 = get_pl( ipl2 );           
    } 

  if (!term.f) {
    fprintf(stderr,"Unknown funcion '%s' in '%s'\n",fname,line);
    exit(EXIT.SYNTAX_ERROR);
    }
    
  addTermToScoreFunction(&term,scoreFunction);  
  
}

void addTermToScoreFunction(_scoreFunctionTerm* term,_scoreFunction* scoreFunction) {
  int index = scoreFunction->size;
  if (++scoreFunction->size >= scoreFunction->max_size) {
    scoreFunction->max_size += SCORE_FUNCTION.DELTA_SIZE;
    _scoreFunctionTerm* newTerms = (_scoreFunctionTerm*) 
       realloc( scoreFunction->terms, scoreFunction->max_size * sizeof( _scoreFunctionTerm ) );
    if (newTerms == NULL) {
      fprintf(stderr,"Couldn't allocate enough memory for score function terms, requested %d bytes", 
        scoreFunction->max_size * sizeof( _scoreFunctionTerm ));
      exit(EXIT.ALLOC_ERROR);      
      }
    scoreFunction->terms = newTerms;  
    }  
  scoreFunction->terms[index] = *term;
  }


//--------------------------------------------------------------------  
// Read command line parameters (if specified)
//--------------------------------------------------------------------  
void get_cmdline_options(int argc, char *argv[], 
                        char *source_file, 
                        char *plan_file) {

// Default values, if no command line values given
  strcpy(source_file,"");    // source file name
  strcpy(plan_file,"");
  
  if (argc == 2) {
// One argument given: Assume plan file name
    strcpy(plan_file,argv[1]);
    }
  if (argc == 3) {
// Two arguments given: plan file, then source file name
    strcpy(plan_file,argv[1]);
    strcpy(source_file,argv[2]);
    }  
   
  if (argc > 3) {
    fprintf(stderr,"Bad number of arguments (%d)\n",argc-1);
    exit(EXIT.PARAMETER_ERROR);
    }

}

int get_pl(int ipl) {
  int pl;
  
// Search existing entry  
  for (int i=0;i<scoreFunction.cdata.pl2ipl_size;i++) {
    if (scoreFunction.cdata.pl2ipl[i] == ipl) return i;
    }

// Check size
   if ( scoreFunction.cdata.pl2ipl_size >= 
        scoreFunction.cdata.pl2ipl_max_size) {
       scoreFunction.cdata.pl2ipl_max_size += PL2IPL.DELTA_SIZE; 
       scoreFunction.cdata.pl2ipl = realloc( scoreFunction.cdata.pl2ipl, 
                                             scoreFunction.cdata.pl2ipl_max_size * sizeof( int ) );
     }

// Add new entry
   pl = scoreFunction.cdata.pl2ipl_size;   
   scoreFunction.cdata.pl2ipl[pl] = ipl;
   scoreFunction.cdata.pl2ipl_size++; 
   return pl;
       
  }
