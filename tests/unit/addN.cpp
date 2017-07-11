#include "coreir.h"
#include "coreir-lib/stdlib.h"
#include "coreir-pass/passes.h"


using namespace CoreIR;


int main() {
  
  // New context
  Context* c = newContext();
  
  //Put this addN in the global namespace
  Namespace* g = c->getGlobal();
  
  //Declare a TypeGenerator (in global) for addN
  g->newTypeGen(
    "addN_type", //name for the typegen
    {{"width",AUINT},{"N",AUINT}}, //generater parameters
    [](Context* c, Args args) { //Function to compute type
      uint width = args.at("width")->get<ArgUint>();
      uint N = args.at("N")->get<ArgUint>();
      return c->Record({
        {"in",c->BitIn()->Arr(width)->Arr(N)},
        {"out",c->Bit()->Arr(width)}
      });
    }
  );


  Generator* addN = g->newGeneratorDecl("addN",g->getTypeGen("addN_type"),{{"width",AUINT},{"N",AUINT}});
  
  addN->setGeneratorDefFromFun([](ModuleDef* def,Context* c, Type* t, Args args) {
    uint width = args.at("width")->get<ArgUint>();
    uint N = args.at("N")->get<ArgUint>();
    assert((N & (N-1)) == 0); //Check if power of 2
    assert(N!=1);

    Namespace* stdlib = c->getNamespace("stdlib");
    Generator* add2 = stdlib->getGenerator("add");
    Generator* addN = c->getGlobal()->getGenerator("addN");
    
    Arg* aWidth = c->argUint(width);
    
    def->addInstance("join",add2,{{"width",aWidth}});
    def->connect("join.out","self.out");
    
    if (N == 2) {
      def->connect("self.in.0","join.in.0");
      def->connect("self.in.1","join.in.1");
    }
    else {
      //Connect half instances
      Arg* aN2 = c->argUint(N/2);
      def->addInstance("addN_0",addN,{{"width",aWidth},{"N",aN2}});
      def->addInstance("addN_1",addN,{{"width",aWidth},{"N",aN2}});
      for (uint i=0; i<N/2; ++i) {
        def->connect({"self","in",to_string(i)},{"addN_0","in",to_string(i)});
        def->connect({"self","in",to_string(i+N/2)},{"addN_1","in",to_string(i)});
      }
      def->connect("addN_0.out","join.in.0");
      def->connect("addN_1.out","join.in.1");
    }
  });
  
  // Define Add12 Module
  Type* add12Type = c->Record({
    {"in8",c->BitIn()->Arr(13)->Arr(8)},
    {"in4",c->BitIn()->Arr(13)->Arr(4)},
    {"out",c->Bit()->Arr(13)}
  });

  Namespace* stdlib = CoreIRLoadLibrary_stdlib(c);
  Module* add12 = g->newModuleDecl("Add12",add12Type);
  ModuleDef* def = add12->newModuleDef();
    def->addInstance("add8_upper",addN,{{"width",c->argUint(13)},{"N",c->argUint(8)}});
    def->addInstance("add4_lower",addN,{{"width",c->argUint(13)},{"N",c->argUint(4)}});
    def->addInstance("add2_join",stdlib->getGenerator("add"),{{"width",c->argUint(13)}});
    def->connect("self.in8","add8_upper.in");
    def->connect("self.in4","add4_lower.in");
    def->connect("add8_upper.out","add2_join.in.0");
    def->connect("add4_lower.out","add2_join.in.1");
    def->connect("add2_join.out","self.out");
  add12->setDef(def);
  add12->print();
  
  bool err = false;
  cout << "Checking saving and loading pregen" << endl;
  saveModule(add12, "_add12.json",&err);
  if (err) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  loadModule(c,"_add12.json", &err);
  if(err) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }
  
  cout << "Running Generators" << endl;
  rungenerators(c,add12,&err);
  if (err) c->die();
  add12->print();
 
  cout << "Flattening everything" << endl;
  flatten(c,add12,&err);
  add12->print();
  add12->getDef()->validate();

  cout << "Checking saving and loading postgen" << endl;
  saveModule(add12, "_add12Gen.json",&err);
  if (err) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  Module* m = loadModule(c,"_add12Gen.json", &err);
  if(err) {
    cout << "Could not Load from json!!" << endl;
    c->die();
  }
  m->print();

  deleteContext(c);
}
