const int FAIL   = 1;
const int PASSED = 0;

jmp_buf _try;
char msg[255],descr[255];

typedef void(testfn)();
void setup(),
     teardown(),
     assert_equals_int(int,int,char*),
     assert_equals_bool(bool,bool,char*),
     assert_equals_char(char*,char*,char*),
     assert_equals_ptr( void* act, void* exp, char* _msg);
