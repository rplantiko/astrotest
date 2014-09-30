// Current version of this software
extern const double VERSION;

typedef struct _date_quantiles {
  double *date;
  int size;
  } _date_quantiles;
  
extern _date_quantiles quantiles;  

// Astronomical properties of a single horoscope
typedef struct _hordat {
  double jd_ut, // Julian Date in UT
         jd,    // Julian Date in ET
         lon,   // longitude, east = positive
         lat;   // latitude, north = positive
  bool is_local_time; // flag set for local time
  } _hordat;

// One of several possible functions to generate random dates
typedef 
  double(*randomDateFunction)(struct _hordat hordat[], int size, int j);
  
// ... and here they are
double random_time(struct _hordat hordat[], int size, int j);
double random_date(struct _hordat hordat[], int size, int j);
double random_shuffle(struct _hordat hordat[], int size, int j);
double random_shuffle_lt(struct _hordat hordat[], int size, int j);
double random_date_by_quantile(struct _hordat hordat[], int size, int j);
  
// Read the (few) command line options
void get_cmdline_options(int,char* argv[],char*, char*);

// Functions for reading horoscopes from file
int read_source_file( char*, struct _hordat *);
void get_data( char*, struct _hordat *);

// Compute the score
double get_score( struct _hordat* );
// Total score (mean value of all single scores)
double get_total_score( struct _hordat *, int);

// Central function: How many random sets perform better?
int cmp2rnd( struct _hordat *, int, int, double refScore, 
             double (*)(struct _hordat *,int,int) );

// Auxiliary function for score functions
double zodiacalLength( int ipl, double jd);
double mundanePosition( int pl, _hordat* hor);
double arcdiff(double x, double y);
double red360(double x);

// Global parameters for a run
typedef struct _params {
  char source_file[255];
  char ephepath[255];
  int test_runs, ref_size;
  char hsys;
  bool continue_with_moshier;
  randomDateFunction rand_date_fn;
  double jd1,jd2,jd3;
  double version;  // Current version of this run
  } _params;
  
// Arguments for score functions per term
typedef struct _termFunctionArgs {
  int pl1, pl2;
  double start, end;
  } _termFunctionArgs;
    
// Elementary score function
typedef double (*termFunction)(_hordat*,_termFunctionArgs*);  

typedef struct _scoreFunctionTerm {
  double coeff;
  termFunction f;
  _termFunctionArgs args;
  } _scoreFunctionTerm;
  
typedef struct _computationalData {
  bool compute_houses;
  int pl2ipl_size;  
  int pl2ipl_max_size;  
  int* pl2ipl;
  } _computationalData;  

typedef struct _scoreFunction {
  int size;
  int max_size;
  _scoreFunctionTerm* terms;
  _computationalData cdata;
  } _scoreFunction;

extern _params params; 
extern _scoreFunction scoreFunction;
extern double (*getScoreFn)(struct _hordat*);  

void initializations();
void read_plan(FILE* plan, _params* params, _scoreFunction* scoreFunction);
void parse_single_line( char* line, char* parsing_block, _params* params, _scoreFunction* scoreFunction);
void parse_plan_parameter(char* line, _params* params);
void parse_score_function(char* line, _scoreFunction* scoreFunction);
void addTermToScoreFunction(_scoreFunctionTerm* term,_scoreFunction* scoreFunction);
randomDateFunction getRandomDateFunction(char key);

// --- Buffering
typedef struct _planet_data {
  double x;
  bool computed;
} _planet_data; 
  
extern _planet_data *pl_buf,*mp_buf;
extern double jd_pl_buf, mp_buf_armc, mp_buf_eps;
extern _hordat hor_mp_buf;
extern int debug_level;
extern FILE* dbg; // File for debug info

double get_score_debug( struct _hordat *hordat);
  
void reset_planet_buffer();
int get_pl(int ipl);

// --- Term functions
double zodiacalLengthInRange(_hordat* hor, _termFunctionArgs* args);
double zodiacalAspect(_hordat* hor, _termFunctionArgs* args);
double mundanePositionInRange(_hordat* hor, _termFunctionArgs* args);
double mundaneAspect(_hordat* hor, _termFunctionArgs* args);

// different error codes
typedef struct _EXIT {
  int 
    ALLOC_ERROR, 
    FILE_ACCESS_ERROR,
    SYNTAX_ERROR,
    VERSION_ERROR,
    UNKNOWN_HOUSE_SYSTEM,
    MISSING_PARAMETERS,
    PARAMETER_ERROR,
    EPHEMERIS_CALL_ERROR;
  } _EXIT;
extern const _EXIT EXIT;
  
typedef struct _ALLOCDATA {
  int INITIAL_SIZE, DELTA_SIZE;
  } _ALLOCDATA;
extern const _ALLOCDATA SCORE_FUNCTION, PL2IPL;
  
