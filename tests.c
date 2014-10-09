// Unit tests for astrotest.c

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include "astrotest.h"
#include "tests.h"

const int NTESTS = 7;

void parseSingleLineDetectComment() {
  char parse_block[50] = "";
  strcpy(descr,"Comment lines should be skipped");
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  assert_equals_char(parse_block,"score","'score-begin' should set parse_block name to 'score'");
  parse_single_line("#1*lp(1,2,3)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 0, NULL );
  assert_equals_char(parse_block,"","'score-end' should reset parse_block name to ''");
  }

void parseSingleLineDetectZodiacalLengthInRange() {
  char parse_block[50] = "";
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  assert_equals_char(parse_block,"score","'score-begin' should set parse_block name to 'score'");
  parse_single_line("2*zl(1,2,3)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 1, "Result should contain precisely 1 line" );
  assert_equals_ptr( scoreFunction.terms[0].f, zodiacalLengthInRange, 
                     "Parser shoud detect zodiacalLengthInRange() function" );
  assert_equals_int( (int) scoreFunction.terms[0].coeff, 2, "Must detect coefficient 2");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl1], 1,
                     "Must detect planet number 1");
  assert_equals_int( (int) scoreFunction.terms[0].args.start, 2,
                     "Must detect start of region");                    
  assert_equals_int( (int) scoreFunction.terms[0].args.end, 3,
                     "Must detect end of region");                    
  }

void parseSingleLineDetectZodiacalAspect() {
  char parse_block[50] = "";
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  assert_equals_char(parse_block,"score","'score-begin' should set parse_block name to 'score'");
  parse_single_line("3*zll(0,1,85,95)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 1, "Result should contain precisely 1 line" );
  assert_equals_ptr( scoreFunction.terms[0].f, zodiacalAspect, 
                     "Parser should detect zodiacalAspect() function" );
  assert_equals_int( (int) scoreFunction.terms[0].coeff, 3, "Must detect coefficient 3");
  assert_equals_int( scoreFunction.cdata.pl2ipl_size, 2,
                     "Must have 2 entries in planet table");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl1], 0,
                     "Must detect planet number 1");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl2], 1,
                     "Must detect planet number 2");
  assert_equals_int( (int) scoreFunction.terms[0].args.start, 85,
                     "Must detect start of region");                    
  assert_equals_int( (int) scoreFunction.terms[0].args.end, 95,
                     "Must detect end of region");                    
  }

void parseSingleLineDetectMundanePositionInRange() {
  char parse_block[50] = "";
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  assert_equals_char(parse_block,"score","'score-begin' should set parse_block name to 'score'");
  parse_single_line("2*mp(1,2,3)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 1, "Result should contain precisely 1 line" );
  assert_equals_ptr( scoreFunction.terms[0].f, mundanePositionInRange, 
                     "Parser shoud detect mundanePositionInRange() function" );
  assert_equals_int( (int) scoreFunction.terms[0].coeff, 2, "Must detect coefficient 2");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl1], 1,
                     "Must detect planet number 1");
  assert_equals_int( (int) scoreFunction.terms[0].args.start, 2,
                     "Must detect start of region");                    
  assert_equals_int( (int) scoreFunction.terms[0].args.end, 3,
                     "Must detect end of region");                    
  }

void parseSingleLineDetectMundaneAspect() {
  char parse_block[50] = "";
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  assert_equals_char(parse_block,"score","'score-begin' should set parse_block name to 'score'");
  parse_single_line("3*mpp(0,1,85,95)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 1, "Result should contain precisely 1 line" );
  assert_equals_ptr( scoreFunction.terms[0].f, mundaneAspect, 
                     "Parser should detect mundaneAspect() function" );
  assert_equals_int( (int) scoreFunction.terms[0].coeff, 3, "Must detect coefficient 3");
  assert_equals_int( scoreFunction.cdata.pl2ipl_size, 2,
                     "Must have 2 entries in planet table");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl1], 0,
                     "Must detect planet number 1");
  assert_equals_int( scoreFunction.cdata.pl2ipl[scoreFunction.terms[0].args.pl2], 1,
                     "Must detect planet number 2");
  assert_equals_int( (int) scoreFunction.terms[0].args.start, 85,
                     "Must detect start of region");                    
  assert_equals_int( (int) scoreFunction.terms[0].args.end, 95,
                     "Must detect end of region");                    
  }
  
void ignoreSourceScoreBlock() {
  char parse_block[50] = "";
  parse_single_line("source-score-begin",parse_block,&params,&scoreFunction);
  parse_single_line("3*mpp(1,2,85,95)",parse_block,&params,&scoreFunction);
  parse_single_line("source-score-end",parse_block,&params,&scoreFunction);
  parse_single_line("score-begin",parse_block,&params,&scoreFunction);
  parse_single_line("3*mpp(0,1,85,95)",parse_block,&params,&scoreFunction);
  parse_single_line("score-end",parse_block,&params,&scoreFunction);
  parse_single_line("source-score-begin",parse_block,&params,&scoreFunction);
  parse_single_line("3*mpp(1,2,85,95)",parse_block,&params,&scoreFunction);
  parse_single_line("source-score-end",parse_block,&params,&scoreFunction);
  assert_equals_int( scoreFunction.size, 1, "Result should contain precisely 1 line (source-score lines must be ignored" );    
  }  

void detectOpposition() {

  _hordat h = { 2451545., 0.,0.};
  _termFunctionArgs args = { 0, 1, 170, 185 };  
  pl_buf = malloc( 2 * sizeof( _planet_data ) );
  
  pl_buf[0].computed = pl_buf[1].computed = true;
    
  jd_pl_buf = h.jd;
  
  pl_buf[0].x = 30;
  pl_buf[1].x = 201;
  assert_equals_int( (int) zodiacalAspect( &h, &args),
                      1,
                      "Aspect 30..201 must be in range 170..185");

  pl_buf[0].x = 30;
  pl_buf[1].x = 199;
  assert_equals_int( (int) zodiacalAspect( &h, &args),
                      0,
                      "Aspect 30..199 must not be in range 170..185");
   
  pl_buf[0].x = 30;
  pl_buf[1].x = 214;
  assert_equals_int( (int) zodiacalAspect( &h, &args),
                      1,
                      "Aspect 30..214 must be in range 170..185");

  pl_buf[0].x = 30;
  pl_buf[1].x = 216;
  assert_equals_int( (int) zodiacalAspect( &h, &args),
                      0,
                      "Aspect 30..216 must not be in range 170..185");

  }
  
bool test(int i,testfn fn, char* description) {
  int rc;
  setup( );  
  if (( rc = setjmp( _try ) )==0) {
    fn();   
    }
  printf("%s %d - %s\n", rc == PASSED ? "ok" : "not ok",i,description);
  if (*msg != '\0') printf("# %s\n",msg);
  teardown( );
  return rc == PASSED;
  }
  
void setup( ) {
  
// Initialize global data from test framework  
  *msg = '\0';
  strcpy(descr,"[No description maintained]");
  
// Initialize global data from application
   const _scoreFunction scoreFunction0 = { };
   scoreFunction = scoreFunction0;
   const _params params0 = {};
   params = params0;

   if (pl_buf) {
     free( pl_buf );
     pl_buf = NULL;
      }
   if (mp_buf) {
     free( mp_buf );
     mp_buf = NULL;
     }
   reset_planet_buffer( );
  
}


void teardown( ) {
  if (scoreFunction.terms) {
    free(scoreFunction.terms);
    scoreFunction.terms = NULL;
    scoreFunction.size = 0;
    scoreFunction.max_size = 0;
    }
  if (scoreFunction.cdata.pl2ipl) {
    free(scoreFunction.cdata.pl2ipl);
    scoreFunction.cdata.pl2ipl = NULL;
    scoreFunction.cdata.pl2ipl_size = 0;
    scoreFunction.cdata.pl2ipl_max_size = 0;
    }
  if (pl_buf) {
    free(pl_buf);
    pl_buf = NULL;
    jd_pl_buf = 0;    
    } 
}  

int main( int argc, char** argv) {
  
  int ok = 0;
  printf("1..%d\n",NTESTS); 
  
  if (test(1,parseSingleLineDetectComment, "No action on comment")) ok++;
  if (test(2,parseSingleLineDetectZodiacalLengthInRange,"Detect single function zl(1,2,3)")) ok++;
  if (test(3,parseSingleLineDetectZodiacalAspect,"Detect single function zll(0,1,85,95)")) ok++;
  if (test(4,parseSingleLineDetectMundanePositionInRange,"Detect single function mp(1,2,3)")) ok++;
  if (test(5,parseSingleLineDetectMundaneAspect,"Detect single function mpp(0,1,85,95)")) ok++;
  if (test(6,ignoreSourceScoreBlock,"Ignore source code block(s)")) ok++;
  if (test(7,detectOpposition,"Detect opposition aspect")) ok++;
  
  
  
  
  printf("# %d/%d ok\n",ok,NTESTS);
  
}

void assert_equals_int( int act, int exp, char* _msg) {
  if (act != exp) {
    if (_msg == NULL || *_msg == '\0') {
      sprintf(msg,"Expected %d / actual %d",exp,act);
      }
    else {
      sprintf(msg,_msg,exp,act);
      }  
    longjmp(_try,FAIL);
    }
  }
  
void assert_equals_bool( bool act, bool exp, char* _msg) {
  assert_equals_int( (int)act, (int)exp, _msg);
  }  

void assert_equals_char( char* act, char* exp, char* _msg) {
  if (strcmp(act,exp)!=0) {
    if (_msg == NULL || *_msg == '\0') {
      sprintf(msg,"Expected '%s' / actual '%s'",exp,act);
      }
    else {
      sprintf(msg,_msg,exp,act);
      }  
    longjmp(_try,FAIL);
    }
  }

void assert_equals_ptr( void* act, void* exp, char* _msg) {
  if (act!=exp) {
    if (_msg == NULL || *_msg == '\0') {
      sprintf(msg,"Expected '%p' / actual '%p'",exp,act);
      }
    else {
      sprintf(msg,_msg,exp,act);
      }  
    longjmp(_try,FAIL);
    }
  }
