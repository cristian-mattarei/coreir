#include "args.hpp"
#include "common.hpp"
#include "error.hpp"
#include "context.hpp"

namespace CoreIR {

bool ArgBool::operator==(const Arg& r) const {
  if (!Arg::operator==(r)) return false;
  return this->b == static_cast<const ArgBool&>(r).b;
}

bool ArgInt::operator==(const Arg& r) const {
  if (!Arg::operator==(r)) return false;
  return this->i == static_cast<const ArgInt&>(r).i;
}

bool ArgUint::operator==(const Arg& r) const {
  if (!Arg::operator==(r)) return false;
  return this->u == static_cast<const ArgUint&>(r).u;
}

bool ArgString::operator==(const Arg& r) const {
  if (!Arg::operator==(r)) return false;
  return this->str == static_cast<const ArgString&>(r).str;
}

bool ArgType::operator==(const Arg& r) const {
  if (!Arg::operator==(r)) return false;
  return this->t == static_cast<const ArgType&>(r).t;
}
string ArgType::toString() const { return t->toString();}

bool operator==(const Args& l, const Args& r) {
  if (l.size() != r.size() ) return false;
  for (auto largmap : l) {
    auto rargmap = r.find(largmap.first);
    if (rargmap == r.end() ) return false;
    if (!(*(rargmap->second) == *(largmap.second))) return false;
  }
  return true;
}

//bool checkArgs(Args args, Params params) {
//  if (args.size() != params.size()) return false;
//  for (auto parammap : params) {
//    auto arg = args.find(parammap.first);
//    if (arg == args.end()) return false;
//    if (arg->second->getKind() != parammap.second) return false;
//  }
//  return true;
//}

}//CoreIR namespace

using namespace CoreIR;

//TODO sketchy because I am overloading a version of unordered_map
size_t std::hash<Args>::operator() (const Args& args) const {
  
  size_t ret = 0;
  //Need to combine these in an order independent way, so just xor
  for (auto it : args) {
    size_t hash = 0;
    hash_combine(hash,it.first);
    Arg* arg = it.second;
    switch(arg->getKind()) {
      case ASTRING : {
        string arg_s = arg->get<ArgString>();
        hash_combine(hash,arg_s);
        break;
      }
      case ABOOL : {
        bool arg_b = arg->get<ArgBool>();
        hash_combine(hash,arg_b);
        break;
      }
      case AUINT : {
        int arg_u = arg->get<ArgUint>();
        hash_combine(hash,arg_u);
        break;
      }
      case AINT : {
        int arg_i = arg->get<ArgInt>();
        hash_combine(hash,arg_i);
        break;
      }
      case ATYPE : {
        Type* arg_t = arg->get<ArgType>();
        hash_combine(hash,arg_t);
        break;
      }
      default : 
        assert(false);
    }
    ret ^= hash;
  }
  return ret;
}
