#include "data.h"
#include <malloc.h>
#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <shlwapi.h>

#ifdef __cplusplus
  #define UEXT extern "C"
#else
  #define UEXT extern
#endif

struct RGSSXTable{
  #define def(a)  void *a;
  RGSSX_FORWARD(def);
  #undef def  
} dll;

__asm__(".set ofs, _dll\n\t");

#define make_export(a)\
  __asm__(".global _" #a "\n\t");\
  __asm__("_" #a ":\n\t");\
  __asm__("mov ofs, %eax\n\t");\
  __asm__("jmp *%eax\n\t");\
  __asm__(".set ofs, ofs+4\n\t");
  
RGSSX_FORWARD(make_export)
#undef make_export
  typedef int (*init_t)(HINSTANCE);
  typedef int (*eval_t)(const char *);
  eval_t oldeval;  

static int MyRGSSEval(const char *str){
  static int run = 0;
  if(!run){
    run = 1;
    (eval_t(oldeval))("begin eval File.read './gc.txt'; rescue Exception => ex; open('1.txt', 'w') do |f| f.write ex.backtrace.join($/); f.write($/);f.write(ex.to_s); end end ");
  }
  return (eval_t(oldeval))(str);
}
#define MAKE_NAMES(a)  (const char *)#a, 
static const char* names[] = {
  RGSSX_FORWARD(MAKE_NAMES)
  0
};
#undef MAKE_NAMES

static void init(HINSTANCE h){
    char *s = (char *)malloc(2048);
    char *t = (char *)malloc(2048);
    t[0] = 0;
    strcpy(t, PREFIX);
    GetModuleFileName(h, s, 2048);
    strcat(t, PathFindFileName(s));
    HINSTANCE ha = LoadLibrary(t);
    void **start = (void **)&(dll);
    for(const char **p = (const char **)names; *p; ++p, ++start){
      *start = (void *)GetProcAddress(ha, *p);
    }
    free(t);
    free(s);
}

UEXT BOOL APIENTRY DllMain(HINSTANCE h, int reason, LPVOID){
 if(reason == DLL_PROCESS_ATTACH){
   init(h);
   oldeval = (eval_t)dll.RGSSEval;
   dll.RGSSEval = (void *)MyRGSSEval;
 }
 return TRUE;
}
