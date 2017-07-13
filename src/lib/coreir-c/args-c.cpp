#include "coreir-c/coreir.h"
#include "coreir.h"

#include "common-c.hpp"

namespace CoreIR {


extern "C" {

  COREParam COREGetArgKind(COREArg* arg) {
    Arg* t = rcast<Arg*>(arg);
    return static_cast<COREParam>(t->getKind());
  }
  
  uint COREArgUintGet(COREArg* a) {
    Arg* arg = rcast<Arg*>(a);
    //Get will assert if wrong arg kind
    return arg->get<ArgUint>();
  }
  
  int COREArgIntGet(COREArg* a) {
    Arg* arg = rcast<Arg*>(a);
    //Get will assert if wrong arg kind
    return arg->get<ArgInt>();
  }
  
  const char* COREArgStringGet(COREArg* a) {
    Arg* arg = rcast<Arg*>(a);
    //Get will assert if wrong arg kind
    const string& s = arg->get<ArgString>();
    return s.c_str();
  }
  
  //Create Arg for uint
  COREArg* COREArgUint(COREContext* c,uint u) {
    Arg* ga = rcast<Context*>(c)->argUint(u);
    return rcast<COREArg*>(ga);
  }
  
  //Create Arg for int
  COREArg* COREArgInt(COREContext* c,int i) {
    Arg* ga = rcast<Context*>(c)->argInt(i);
    return rcast<COREArg*>(ga);
  }
  
  //Create Arg for String
  COREArg* COREArgString(COREContext* c,char* str) {
    Arg* ga = rcast<Context*>(c)->argString(string(str));
    return rcast<COREArg*>(ga);
  }

}

}
