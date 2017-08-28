#include "coreir.h"
#include "coreir-lib/cgralib.h"

#include "coreir-passes/common.h"


using namespace CoreIR;


int main() {
  
  // New context
  Context* c = newContext();
  
  //Find linebuffer in the cgra namespace
  Namespace* cgralib = CoreIRLoadLibrary_cgralib(c);
  Generator* linebuffer = cgralib->getGenerator("Linebuffer");

  // Define lb32 Module
  Type* lb32Type = c->Record({
    {"in",c->BitIn()->Arr(16)},
    {"out",c->Bit()->Arr(16)->Arr(2)->Arr(3)}
  });


  Module* lb32 = c->getGlobal()->newModuleDecl("lb32", lb32Type);
  ModuleDef* def = lb32->newModuleDef();
    def->addInstance("lb32_inst", linebuffer, {{"bitwidth",c->argInt(16)},
	  {"stencil_width",c->argInt(2)},{"stencil_height",c->argInt(3)},
					   {"image_width",c->argInt(512)}});
    def->connect("self.in", "lb32_inst.in");
    def->connect("self.out", "lb32_inst.out");
  lb32->setDef(def);
  lb32->print();

  cout << "Checking saving and loading pregen" << endl;
  if (!saveToFile(c->getGlobal(), "_lb32.json", lb32)) {
    cout << "Could not save to json!!" << endl;
    c->die();
  }
  
  cout << "Running Generators" << endl;
  cout << "Flattening everything" << endl;
  c->runPasses({"rungenerators","flatten"});
  lb32->getDef()->validate();
    
  CoreIR::Module* m = nullptr;
  if (!loadFromFile(c, "_lb32.json", &m)) {
    cout << "Could not load from json!!" << endl;
    c->die();
  }
  ASSERT(m, "Could not load top: _lb32");
  m->print();
  
  deleteContext(c);
}
