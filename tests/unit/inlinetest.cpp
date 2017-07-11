#include "coreir.h"
#include "coreir-lib/stdlib.h"
#include "coreir-pass/passes.h"


using namespace CoreIR;


int main() {
  
  // New context
  Context* c = newContext();
  
  //Put this add4 in the global namespace
  Namespace* g = c->getGlobal();
  
  //Declare a TypeGenerator (in global) for add4
  g->newTypeGen(
    "add4_type", //name for the typegen
    {{"width",AUINT}}, //generater parameters
    [](Context* c, Args args) { //Function to compute type
      uint width = args.at("width")->get<ArgUint>();
      return c->Record({
        {"in",c->BitIn()->Arr(width)->Arr(4)},
        {"out",c->Bit()->Arr(width)}
      });
    }
  );


  Generator* add4 = g->newGeneratorDecl("add4",g->getTypeGen("add4_type"),{{"width",AUINT}});
  
  add4->setGeneratorDefFromFun([](ModuleDef* def,Context* c, Type* t, Args args) {
    uint n = args.at("width")->get<ArgUint>();
    
    Namespace* stdlib = c->getNamespace("stdlib");
    auto add2 = stdlib->getGenerator("add");
    Wireable* self = def->sel("self");
    Wireable* add_00 = def->addInstance("add00",add2,{{"width",c->argUint(n)}});
    Wireable* add_01 = def->addInstance("add01",add2,{{"width",c->argUint(n)}});
    Wireable* add_1 = def->addInstance("add1",add2,{{"width",c->argUint(n)}});
    
    def->connect(self->sel("in")->sel(0),add_00->sel("in")->sel(0));
    def->connect(self->sel("in")->sel(1),add_00->sel("in")->sel(1));
    def->connect(self->sel("in")->sel(2),add_01->sel("in")->sel(0));
    def->connect(self->sel("in")->sel(3),add_01->sel("in")->sel(1));

    def->connect(add_00->sel("out"),add_1->sel("in")->sel(0));
    def->connect(add_01->sel("out"),add_1->sel("in")->sel(1));

    def->connect(add_1->sel("out"),self->sel("out"));
  });
 
  Type* t = g->getTypeGen("add4_type")->getType({{"width",c->argUint(13)}});
  CoreIRLoadLibrary_stdlib(c);
  
  Module* add = g->newModuleDecl("Add",t);
  ModuleDef* def = add->newModuleDef();
    Instance* inst = def->addInstance("i0",add4,{{"width",c->argUint(13)}});
    for (uint i=0; i<4; ++i) {
      def->connect(inst->sel("in")->sel(i),def->getInterface()->sel("in")->sel(i));
    }
    def->connect(inst->sel("out"),def->getInterface()->sel("out"));
    inst->runGenerator();
    inlineInstance(inst);
  add->setDef(def);
  add->print();
  deleteContext(c);

}
