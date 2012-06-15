
#include "aseclr_sctx.hpp"
#include "aseclr_global.hpp"

#define DBG(x)

namespace aseclr {

ase_clr_script_engine::ase_clr_script_engine(ase_unified_engine_ref& ue,
  const ase_variant& gl)
  : ueref(ue), clrpr_endnode(0), asepr_endnode(0), global(gl)
{
  DBG(fprintf(stderr, "GLOBAL vtbl=%p vtype=%d\n", global.get_vtable_address(),
    global.get_type()));
  ase_clr_set_engine_ref(global, this);
  ase_clr_ase_variant_impl::init_arg ciarg;
  ciarg.sc = this;
  ciarg.obj = nullptr;
  std::auto_ptr<ase_clr_ase_variant_implnode> cend(
    new ase_clr_ase_variant_implnode(ciarg));
  std::auto_ptr<aseproxy_node> aend(new aseproxy_node);
  clrpr_endnode = cend.release();
  asepr_endnode = aend.release();
}

ase_clr_script_engine::~ase_clr_script_engine()
{
  dispose_proxies();
  delete asepr_endnode;
  delete clrpr_endnode;
  #ifdef _DEBUG
  System::GC::Collect();
  #endif
}

void
ase_clr_script_engine::dispose_proxies()
{
  DBG(fprintf(stderr, "clr: disposeproxies\n"));
  global = ase_variant();
  if (clrpr_endnode) {
    ase_clr_ase_variant_implnode *p = clrpr_endnode;
    do {
      p->value.dispose();
      p = p->get_next();
    } while (p != clrpr_endnode);
  }
  if (asepr_endnode) {
    aseproxy_node *p = asepr_endnode;
    do {
      p->value.set(ase_variant());
      p = p->get_next();
    } while (p != asepr_endnode);
  }
  DBG(fprintf(stderr, "clr: disposeproxies done\n"));
}

const char *const *
ase_clr_script_engine::get_global_names()
{
  static const char *names[] = { "CLR" };
  return names;
}

ase_variant
ase_clr_script_engine::get_global_object(const char *name)
{
  return global;
}

void
ase_clr_script_engine::import_ase_global(const ase_variant& val)
{
}

ase_variant
ase_clr_script_engine::load_file(const char *glname, const char *fname)
{
  return ase_variant();
}

ase_script_engine *
ase_clr_script_engine::get_real_engine()
{
  return this;
}

}; // namespace aseclr


