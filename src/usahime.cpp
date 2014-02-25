#include "data.h"
#include <stdio.h>
#include <windows.h>
#include <assert.h>

#ifdef __cplusplus
  #define EXTERN extern "C"
#else
  #define EXTERN extern
#endif

struct RGSSXTable{
  #define def(a)  void *a;
  RGSSX_FORWARD(def);
  #undef def  
} dll;

__asm__(".set ofs, 0\n\t");

#define make_export(a)\
  __asm__(".global _" #a "\n\t");\
  __asm__("_" #a ":\n\t");\
  __asm__("mov $_dll, %eax\n\t");\
  __asm__("jmp *ofs(%eax)\n\t");\
  __asm__(".set ofs, ofs+4");
  
RGSSX_FORWARD(make_export)
#undef make_export
  typedef int (*init_t)(HINSTANCE);
  typedef int (*eval_t)(const char *);
  eval_t oldeval;  

EXTERN int MyRGSSEval(const char *str){
  static int run = 0;
  if(!run){
    run = 1;
    (eval_t(oldeval))("begin eval File.read './gc.txt'; rescue Exception => ex; open('1.txt', 'w') do |f| f.write ex.backtrace.join($/); f.write($/);f.write(ex.to_s); end end ");
  }
  return (eval_t(oldeval))(str);
}

void init(){
    HINSTANCE h = LoadLibrary("rgss103j_usahime.dll");
    #define setHandler(a) assert(dll.a = (void *)GetProcAddress(h, #a));
    RGSSX_FORWARD(setHandler);
    #undef setHandler
}

EXTERN BOOL APIENTRY DllMain(HINSTANCE, int reason, LPVOID){
 if(reason == DLL_PROCESS_ATTACH){
   init();
   oldeval = (eval_t)dll.RGSSEval;
   dll.RGSSEval = (void *)MyRGSSEval;
 }
 return TRUE;
}
