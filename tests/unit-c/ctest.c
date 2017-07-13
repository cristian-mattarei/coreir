#include "coreir-c/coreir.h"
#include "coreir-lib/stdlib.h"
#include "assert.h"

#include "stdio.h"
#include "stdlib.h"

void print(char* a) {
  printf("%s",a); printf("\n"); fflush(stdout);
}

int main() {
  COREContext* c = CORENewContext();
  CORELoadLibrary_stdlib(c);
  COREBool err = false;
  
  char** rkeys = malloc(2*sizeof(char*));
  rkeys[0] = "in";
  rkeys[1] = "out";
  COREType** rvalues = malloc(2*sizeof(COREType*));
  rvalues[0] = COREBitIn(c);
  rvalues[1] = COREBit(c);

  void* recordparams = CORENewMap(c,rkeys,rvalues,2,STR2TYPE_ORDEREDMAP);

  COREType* lut4type = CORERecord(c,recordparams);
  CORENamespace* ns = COREGetGlobal(c);
  char** cpkeys = malloc(2*sizeof(char*));
  cpkeys[0] = "in";
  cpkeys[1] = "out";
  int** cpparams = malloc(2*sizeof(int*));
  uint param = COREUintParam;
  cpparams[0] = 2;
  cpparams[1] = &param;

  void* cp = CORENewMap(c,cpkeys,cpparams,2,STR2PARAM_MAP);

  COREModule* lut4 = CORENewModule(ns,"LUT4",lut4type,cp);

  COREModule* m = CORENewModule(ns,"Main",COREBitIn(c),NULL);

  COREModuleDef* mdef = COREModuleNewDef(m);

  char** ckeys = malloc(2*sizeof(char*));
  ckeys[0] = "in";
  ckeys[1] = "out";
  COREArg** cargs = malloc(2*sizeof(COREArg*));
  cargs[0] = COREArgUint(c,13);
  cargs[1] = COREArgUint(c,13);

  void* config = CORENewMap(c,ckeys,cargs,2,STR2ARG_MAP);

  COREWireable* inst = COREModuleDefAddModuleInstance(mdef, "ctop",lut4,config);
  (void) inst;
  //TODO once the C api is changed
  /*
  COREWireable* sel1 = COREInstanceSelect(inst,"in");
  COREWireable* sel2 = COREWireableSelect(sel1,"4");
  int num_selects
  const char** selpath = COREWireableGetSelectPath(sel2, &num_selects);
  assert(num_selects==2);
  assert(strcmp(selpath[0],"in")==0);
  assert(strcmp(selpath[1],"4")==0);
  */

  COREModuleSetDef(m,mdef);

  COREPrintModule(m);


  CORESaveModule(m,"_simple.json",&err);
  if (err) {
    COREPrintErrors(c);
    printf("Cannot open _simple.json\n");
    return 1;
  }

  COREPrintModule(m);

  COREDeleteContext(c);
  c = CORENewContext(c);
  CORELoadLibrary_stdlib(c);
  m = CORELoadModule(c,"_simple.json",&err);
  if (err) {
    COREPrintErrors(c);
    printf("Cannot load _simple.json\n");
    return 1;
  }
  printf("Loaded Module!\n");
  COREPrintModule(m);

  return 0;
}
