
#include <ase/ase.hpp>

#include <vector>

namespace asescr {

struct sentence {
  ase_string inlinec;
};

struct vardecl {
  ase_string name;
};

struct methoddef {
  ase_string name;
  std::vector<vardecl> vars;
  std::vector<sentence> body;
};

struct klassdef {
  ase_string name;
  std::vector<vardecl> kvars;
  std::vector<vardecl> ivars;
  std::vector<methoddef> kmethods;
  std::vector<methoddef> imethods;
};

}; // namespace asescr


