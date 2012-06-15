
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asermt_protocol.hpp>
#include <ase/aseatomic.hpp>
#include <ase/aseasn1.hpp>
#include <ase/asearray.hpp>
#include <ase/aseexcept.hpp>
#include <map>
#include <set>
#include <vector>
#include <cassert>

#define DBG(x)
#define DBG_WRONGOP(x) x
#define DBG_PKC(x)
#define DBG_WR(x)
#define DBG_DESTR(x)
#define DBG_FULL(x)
#define DBG_GC(x)

namespace {

/* ownership: remote_object --> remote_handle --> local_objcts */
struct remote_object;
struct remote_handle;
typedef size_t ptrint_type; /* sizeof(ptrint_type) == sizeof(void *) */
typedef std::map<ase_long, remote_object *> remoteobjmap_type;
typedef std::set<ase_long> remoteobj_destr_cand_type;
typedef std::map<ptrint_type, ase_variant> localobjmap_type;
typedef std::vector<ase_variant> wres_type;

enum remote_opcode {
  op_return = 0x01,
  op_throw,
  op_destroy = 0x21,
  op_getattr,
  op_getbool,
  op_getint,
  op_getlong,
  op_getdouble,
  op_getstring,
  op_issameobj,
  op_getprop,
  op_setprop,
  op_delprop,
  op_getelem,
  op_setelem,
  op_getlen,
  op_setlen,
  op_invname,
  op_invid,
  op_invself,
  op_getenum,
  op_enumnext,
};

struct remote_handle :
  public ase_intrusive_count<remote_handle>,
  public ase_object_serializer,
  public ase_object_deserializer {

  friend struct remote_object;

  remote_handle(bool iscl, const ase_serializer_table *ser);
  ~remote_handle();

  virtual bool serialize_object(const ase_variant& value, bool& remote_r,
    ase_long& id_r);
  virtual ase_variant deserialize_object(bool remote, ase_long id);

  static ase_long get_local_object_id(const ase_variant& v);

  void execute_request(const ase_variant& rpkt, const int op,
    wres_type& wres_r);
  ase_variant execute_request_internal(const ase_variant& rpkt,
    const int op, bool& exret_r);
  static void get_args(const ase_variant& pkt, ase_size_type offset,
    std::vector<ase_variant>& arr_r);
  void local_object_erase(const ase_variant& obj);

  void packet_write(const wres_type& wres, bool allow_gc);
  ase_variant packet_read();
  ase_variant remote_event_loop(bool *exret = 0);

  void garbage_collect() throw();
  void close_connection() throw();
  void check_connection();

  /* TODO: recursive mutex */
  remoteobjmap_type remoteobjmap;
  remoteobj_destr_cand_type remoteobj_destr_cand;
  localobjmap_type localobjmap;
  ase_auto_ptr<ase_io_buffered_stream> stream;
  const bool is_client;
  const ase_serializer_table *const serializer;

  int last_op; /* read: 1, write: 2 */

};

struct remote_object {

  remote_object(ase_long id, remote_handle *hnd);
  ~remote_object();

  static remote_object *get(const ase_variant& v) {
    return static_cast<remote_object *>(v.get_rep().p);
  }

  static void add_ref(const ase_variant& v);
  static void release(const ase_variant& v);
  static ase_int get_attributes(const ase_variant& v);
  static bool get_boolean(const ase_variant& v);
  static ase_int get_int(const ase_variant& v);
  static ase_long get_long(const ase_variant& v);
  static double get_double(const ase_variant& v);
  static ase_string get_string(const ase_variant& v);
  static bool is_same_object(const ase_variant& v, const ase_variant& val);
  static ase_variant get_property(const ase_variant& v, const char *name,
    ase_size_type len);
  static void set_property(const ase_variant& v, const char *name,
    ase_size_type len, const ase_variant& value);
  static void del_property(const ase_variant& v, const char *name,
    ase_size_type len);
  static ase_variant get_element(const ase_variant& v, ase_int idx);
  static void set_element(const ase_variant& v, ase_int idx,
    const ase_variant& value);
  static ase_int get_length(const ase_variant& v);
  static void set_length(const ase_variant& v, ase_int len);
  static ase_variant invoke_by_name(const ase_variant& v, const char *name,
    ase_size_type namelen, const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_by_id(const ase_variant& v, ase_int id,
    const ase_variant *args, ase_size_type nargs);
  static ase_variant invoke_self(const ase_variant& v, const ase_variant *args,
    ase_size_type nargs);
  static ase_variant get_enumerator(const ase_variant& v);
  static ase_string enum_next(const ase_variant& v, bool& hasnext_r);

  void create_pkt(wres_type& wres_r, const ase_variant& v, remote_opcode op);
  void create_pkt(wres_type& wres_r, const ase_variant& v, remote_opcode op,
    const ase_variant& a0);
  void create_pkt(wres_type& wres_r, const ase_variant& v, remote_opcode op,
    const ase_variant& a0, const ase_variant& a1);
  void create_pktv(wres_type& wres_r, const ase_variant& v, remote_opcode op,
    const ase_variant *args, ase_size_type nargs);
  void create_pktv(wres_type& wres_r, const ase_variant& v, remote_opcode op,
    const ase_variant& a0, const ase_variant *args, ase_size_type nargs);
  ase_variant call_remote(const wres_type& wres, bool *exret = 0);

  ase_atomic_count refcount;
  ase_long id;
  bool is_handle_owner;
  ase_shared_ptr<remote_handle> handle;
  static const char *const debugid;

};

remote_object::remote_object(ase_long id, remote_handle *hnd)
  : refcount(1), id(id), is_handle_owner(false),
  handle(hnd, true /* addref */)
{
  DBG_DESTR(fprintf(stderr, "ROBJECT constr %p\n", this));
  if (handle->is_client && handle->remoteobjmap.empty()) {
    DBG_DESTR(fprintf(stderr, "ROBJECT handle owner %p\n", this));
    /* handle_owner has responsibility to close handle */
    is_handle_owner = true;
  }
  handle->remoteobjmap.insert(std::make_pair(id, this));
}

remote_object::~remote_object()
{
  DBG_DESTR(fprintf(stderr, "ROBJECT destr %p\n", this));
  handle->remoteobjmap.erase(id);
}

void
remote_object::add_ref(const ase_variant& v)
{
  get(v)->refcount++;
}

void
remote_object::release(const ase_variant& v)
{
  remote_object *const p = get(v);
  if ((p->refcount--) == 1) {
    if (p->handle->stream.get()) {
      if (p->is_handle_owner) {
	DBG_GC(std::fprintf(stderr, "GC: handle owner(close): %p %lld\n",
	  p, p->id));
	p->handle->close_connection();
	delete p;
      } else {
	/* because ase-remoting is a flip-flop protocol, we can send op_destroy
	 * request only if we are about to write a request or a response.
	 * memorize that p is a candidate for a garbage. p is actually
	 * freed by garbage_collect(). */
	try {
	  DBG_GC(std::fprintf(stderr, "GC: mark destr: %p %lld\n", p, p->id));
	  p->handle->remoteobj_destr_cand.insert(p->id);
	  DBG_GC(std::fprintf(stderr, "GC: mark destr: done %p %lld\n", p,
	    p->id));
	} catch (...) {
	  DBG_GC(std::fprintf(stderr, "GC: mark destr: thrown %p %lld\n", p,
	    p->id));
	  p->handle->close_connection();
	  delete p;
	}
      }
    } else {
      delete p;
    }
  }
}

ase_int
remote_object::get_attributes(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro getattr\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getattr);
  return self->call_remote(wres).get_int();
}

bool
remote_object::get_boolean(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro getb\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getbool);
  return self->call_remote(wres).get_boolean();
}

ase_int
remote_object::get_int(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro geti\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getint);
  return self->call_remote(wres).get_int();
}

ase_long
remote_object::get_long(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro getl\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getlong);
  return self->call_remote(wres).get_long();
}

double
remote_object::get_double(const ase_variant& v)
{
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getdouble);
  return self->call_remote(wres).get_double();
}

ase_string
remote_object::get_string(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro gets\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getstring);
  return self->call_remote(wres).get_string();
}

bool
remote_object::is_same_object(const ase_variant& v, const ase_variant& val)
{
  DBG(fprintf(stderr, "ro same\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_issameobj, val);
  return self->call_remote(wres).get_boolean();
}

ase_variant
remote_object::get_property(const ase_variant& v, const char *name,
  ase_size_type len)
{
  DBG(fprintf(stderr, "ro gprop %s\n", name));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getprop, ase_string(name, len).to_variant());
  return self->call_remote(wres);
}

void
remote_object::set_property(const ase_variant& v, const char *name,
  ase_size_type len, const ase_variant& value)
{
  DBG(fprintf(stderr, "ro sprop\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_setprop, ase_string(name, len).to_variant(),
    value);
  self->call_remote(wres);
}

void
remote_object::del_property(const ase_variant& v, const char *name,
  ase_size_type len)
{
  DBG(fprintf(stderr, "ro dprop\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_delprop, ase_string(name, len).to_variant());
  self->call_remote(wres);
}

ase_variant
remote_object::get_element(const ase_variant& v, ase_int idx)
{
  DBG(fprintf(stderr, "ro gelem\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getelem, idx);
  return self->call_remote(wres);
}

void
remote_object::set_element(const ase_variant& v, ase_int idx,
  const ase_variant& value)
{
  DBG(fprintf(stderr, "ro selem\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_setelem, idx, value);
  self->call_remote(wres);
}

ase_int
remote_object::get_length(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro glen\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getlen);
  return self->call_remote(wres).get_int();
}

void
remote_object::set_length(const ase_variant& v, ase_int len)
{
  DBG(fprintf(stderr, "ro slen\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_setlen, len);
  self->call_remote(wres);
}

ase_variant
remote_object::invoke_by_name(const ase_variant& v, const char *name,
  ase_size_type namelen, const ase_variant *args, ase_size_type nargs)
{
  DBG(fprintf(stderr, "ro iname\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pktv(wres, v, op_invname,
    ase_string(name, namelen).to_variant(), args, nargs);
  return self->call_remote(wres);
}

ase_variant
remote_object::invoke_by_id(const ase_variant& v, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  DBG(fprintf(stderr, "ro iid\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pktv(wres, v, op_invid, id, args, nargs);
  return self->call_remote(wres);
}

ase_variant
remote_object::invoke_self(const ase_variant& v, const ase_variant *args,
  ase_size_type nargs)
{
  DBG(fprintf(stderr, "ro iself\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pktv(wres, v, op_invself, args, nargs);
  return self->call_remote(wres);
}

ase_variant
remote_object::get_enumerator(const ase_variant& v)
{
  DBG(fprintf(stderr, "ro genum\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_getenum);
  return self->call_remote(wres);
}

ase_string
remote_object::enum_next(const ase_variant& v, bool& hasnext_r)
{
  DBG(fprintf(stderr, "ro enext\n"));
  remote_object *const self = get(v);
  wres_type wres;
  self->create_pkt(wres, v, op_enumnext);
  return self->call_remote(wres, &hasnext_r).get_string();
}

ase_variant
remote_object::call_remote(const wres_type& wres, bool *exret)
{
  assert(handle.get());
  try {
    handle->packet_write(wres, true);
  } catch (...) {
    handle->close_connection();
    throw;
  }
  return handle->remote_event_loop(exret);
}

void
remote_object::create_pkt(wres_type& wres_r, const ase_variant& v,
  remote_opcode op)
{
  DBG_PKC(fprintf(stderr, "pk op=%d\n", op));
  wres_r.reserve(2);
  wres_r.push_back(op);
  wres_r.push_back(v);
}

void
remote_object::create_pkt(wres_type& wres_r, const ase_variant& v,
  remote_opcode op, const ase_variant& a0)
{
  DBG_PKC(fprintf(stderr, "pk op=%d\n", op));
  wres_r.reserve(3);
  wres_r.push_back(op);
  wres_r.push_back(v);
  wres_r.push_back(a0);
}

void
remote_object::create_pkt(wres_type& wres_r, const ase_variant& v,
  remote_opcode op, const ase_variant& a0, const ase_variant& a1)
{
  DBG_PKC(fprintf(stderr, "pk op=%d\n", op));
  wres_r.reserve(4);
  wres_r.push_back(op);
  wres_r.push_back(v);
  wres_r.push_back(a0);
  wres_r.push_back(a1);
}

void
remote_object::create_pktv(wres_type& wres_r, const ase_variant& v,
  remote_opcode op, const ase_variant *args, ase_size_type nargs)
{
  DBG_PKC(fprintf(stderr, "pk op=%d\n", op));
  wres_r.reserve(nargs + 2);
  wres_r.push_back(op);
  wres_r.push_back(v);
  for (ase_size_type i = 0; i < nargs; ++i) {
    wres_r.push_back(args[i]);
  }
}

void
remote_object::create_pktv(wres_type& wres_r, const ase_variant& v,
  remote_opcode op, const ase_variant& a0, const ase_variant *args,
  ase_size_type nargs)
{
  DBG_PKC(fprintf(stderr, "pk op=%d\n", op));
  wres_r.reserve(nargs + 3);
  wres_r.push_back(op);
  wres_r.push_back(v);
  wres_r.push_back(a0);
  for (ase_size_type i = 0; i < nargs; ++i) {
    wres_r.push_back(args[i]);
  }
}

remote_handle::remote_handle(bool iscl, const ase_serializer_table *ser)
  : is_client(iscl), serializer(ser), last_op(0)
{
  DBG_GC(fprintf(stderr, "HANDLE constr %p\n", this));
}

remote_handle::~remote_handle()
{
  DBG_GC(fprintf(stderr, "HANDLE destr %p\n", this));
}

ase_long
remote_handle::get_local_object_id(const ase_variant& v)
{
  const void *const p = v.get_rep().p;
  if (p) {
    return reinterpret_cast<ase_long>(p);
  }
  const void *const vt = v.get_vtable_address();
  return reinterpret_cast<ase_long>(vt);
}

/* we don't set debugid because we can't call debug_on_destroy_object */
const char *const remote_object::debugid = 0;

bool
remote_handle::serialize_object(const ase_variant& value, bool& remote_r,
  ase_long& id_r)
{
  if (value.get_vtable_address() == &ase_variant_impl<remote_object>::vtable &&
    static_cast<remote_object *>(value.get_rep().p)->handle.get() == this) {
    /* our remote */
    remote_r = true;
    remote_object *const robj = static_cast<remote_object *>(
      value.get_rep().p);
    id_r = robj->id;
    return true;
  } else if ((value.get_vtoption() & ase_vtoption_remote_by_value) != 0) {
    DBG(fprintf(stderr, "byvalue\n"));
    remote_r = false;
    id_r = 0;
    return false;
  } else {
    DBG(fprintf(stderr, "byref %s\n", value.get_string().data()));
    /* our local */
    remote_r = false;
    const ase_long id = get_local_object_id(value);
    localobjmap_type::const_iterator i = localobjmap.find(id);
    if (i == localobjmap.end()) {
      DBG(fprintf(stderr, "new local object %lld\n", id));
      localobjmap.insert(std::make_pair(id, value));
    }
    id_r = id;
    return true;
  }
}

ase_variant
remote_handle::deserialize_object(bool remote, ase_long id)
{
  if (remote) {
    /* sender's remote, our local */
    localobjmap_type::const_iterator i = localobjmap.find(id);
    if (i == localobjmap.end()) {
      DBG(fprintf(stderr, "unknown local object %lld\n", id));
      ase_throw_exception("ASERemoteUnknownLocalObject", ase_string());
    }
    return i->second;
  } else {
    /* sender's local, our remote */
    remoteobjmap_type::const_iterator i = remoteobjmap.find(id);
    if (i != remoteobjmap.end()) {
      /* already exist */
      DBG_GC(fprintf(stderr, "already exist: remote proxy %lld\n", id));
      remote_object *const robj = i->second;
      const ase_variant v = ase_variant::create_object(
	&ase_variant_impl<remote_object>::vtable, robj, 0);
      remote_object::add_ref(v);
      return v;
    } else {
      /* new remote proxy */
      DBG_GC(fprintf(stderr, "new remote proxy %lld\n", id));
      const ase_variant v = ase_variant::create_object(
	&ase_variant_impl<remote_object>::vtable,
	new remote_object(id, this),
	remote_object::debugid);
      return v;
    }
  }
}

void
remote_handle::check_connection()
{
  if (stream.get() == 0) {
    ase_throw_exception("RemoteConnClosed", ase_string());
  }
}

void
remote_handle::execute_request(const ase_variant& rpkt, const int op,
  wres_type& wres_r)
{
  /* returns a packet */
  try {
    bool exret = false;
    DBG(std::fprintf(stderr, "exec op=%x\n", op));
    const ase_variant rval = execute_request_internal(rpkt, op, exret);
    DBG(std::fprintf(stderr, "exec op done\n"));
    if (op == op_enumnext) {
      wres_r.reserve(3);
      wres_r.push_back(op_return);
      wres_r.push_back(rval);
      wres_r.push_back(exret);
    } else {
      wres_r.reserve(2);
      wres_r.push_back(op_return);
      wres_r.push_back(rval);
    }
  } catch (...) {
    const ase_variant ex = ase_variant::create_from_active_exception();
    const ase_variant expkt = ase_new_array(2);
    wres_r.clear();
    wres_r.reserve(2);
    wres_r.push_back(op_throw);
    wres_r.push_back(ex);
  }
}

namespace {

template <typename T> const T *
vec_to_ptr(const std::vector<T>& v)
{
  if (v.empty()) {
    return 0;
  }
  return &v[0];
}

};

ase_variant
remote_handle::execute_request_internal(const ase_variant& rpkt, const int op,
  bool& exret_r)
{
  const ase_variant obj = rpkt.get_element(1);
  switch (op) {
  case op_destroy:
    local_object_erase(obj);
    return ase_variant();
  case op_getattr:
    return obj.get_attributes();
  case op_getbool:
    return obj.get_boolean();
  case op_getint:
    return obj.get_int();
  case op_getlong:
    return obj.get_long();
  case op_getdouble:
    return obj.get_double();
  case op_getstring:
    return obj.get_string().to_variant();
  case op_issameobj:
    return obj.is_same_object(rpkt.get_element(2));
  case op_getprop:
    return obj.get_property(rpkt.get_element(2).get_string());
  case op_setprop:
    obj.set_property(rpkt.get_element(2).get_string(),
      rpkt.get_element(3));
    return ase_variant();
  case op_delprop:
    obj.del_property(rpkt.get_element(2).get_string());
    return ase_variant();
  case op_getelem:
    return obj.get_element(rpkt.get_element(2).get_int());
  case op_setelem:
    obj.set_element(rpkt.get_element(2).get_int(),
      rpkt.get_element(3));
    return ase_variant();
  case op_getlen:
    return obj.get_length();
  case op_setlen:
    obj.set_length(rpkt.get_element(2).get_int());
    return ase_variant();
  case op_invname:
    {
      /* TODO: slow */
      std::vector<ase_variant> arr(rpkt.get_length() - 3);
      get_args(rpkt, 3, arr);
      return obj.invoke_by_name(rpkt.get_element(2).get_string(),
	vec_to_ptr(arr), arr.size());
    }
  case op_invid:
    {
      /* TODO: slow */
      std::vector<ase_variant> arr(rpkt.get_length() - 3);
      get_args(rpkt, 3, arr);
      return obj.invoke_by_id(rpkt.get_element(2).get_int(),
	vec_to_ptr(arr), arr.size());
    }
  case op_invself:
    {
      /* TODO: slow */
      std::vector<ase_variant> arr(rpkt.get_length() - 2);
      get_args(rpkt, 2, arr);
      return obj.invoke_self(vec_to_ptr(arr), arr.size());
    }
  case op_getenum:
    return obj.get_enumerator();
  case op_enumnext:
    return obj.enum_next(exret_r).to_variant();
  default:
    DBG(fprintf(stderr, "warning: unknown op: %d\n", op));
    DBG_WRONGOP(abort());
    break;
  }
  return ase_variant();
}

void
remote_handle::get_args(const ase_variant& pkt, ase_size_type offset,
  std::vector<ase_variant>& arr_r)
{
  for (ase_size_type i = 0; i < arr_r.size(); ++i) {
    arr_r[i] = pkt.get_element(offset + i);
  }
}

void
remote_handle::local_object_erase(const ase_variant& obj)
{
  const ase_long id = get_local_object_id(obj);
  DBG_GC(std::fprintf(stderr, "GCLOCAL: erase %lld\n", id));
  localobjmap.erase(id);
}

void
remote_handle::packet_write(const wres_type& wres, bool allow_gc)
{
  if (allow_gc) {
    garbage_collect();
  }
  check_connection();
  const ase_string str = serializer->serialize(vec_to_ptr(wres),
    vec_to_ptr(wres) + wres.size(), this);
  DBG_WR(std::fprintf(stderr, "write: %d\n", static_cast<int>(str.size())));
  DBG_FULL(std::fprintf(stderr, "WRITE [%s]\n",
    ase_asn1xer_serialize(vec_to_ptr(wres), vec_to_ptr(wres) + wres.size(),
      this).data()));
  if (last_op == 2) {
    abort();
  }
  last_op = 2;
  stream->write(str.data(), str.size());
}

ase_variant
remote_handle::packet_read()
{
  if (last_op == 1) {
    abort();
  }
  last_op = 1;
  check_connection();
  const char *buf = 0;
  ase_size_type bufsize = 0;
  ase_size_type derlen = 0;
  while (true) {
    stream->get_readbuf(buf, bufsize);
    bool overflow = false;
    if (serializer->deserialize_get_length(buf, bufsize, derlen, overflow) &&
      derlen <= bufsize) {
      break;
    }
    DBG_WR(std::fprintf(stderr, "tryread: %p\n", this));
    stream->read_more();
    DBG_WR(std::fprintf(stderr, "tryread done: %p\n", this));
  }
  DBG_WR(std::fprintf(stderr, "read: %d\n", tl_len + v_len));
  const ase_variant v = serializer->deserialize(buf, derlen, this);
  DBG_WR(std::fprintf(stderr, "read: arrlen=%d\n", v.get_length()));
  DBG_FULL(std::fprintf(stderr, "READ [%s]\n",
    ase_asn1xer_serialize(v, this).data()));
  stream->readbuf_consume(derlen);
  return v;
}

void
remote_handle::garbage_collect() throw()
{
  DBG_GC(std::fprintf(stderr, "GC: begin mapsz=%d cand=%d\n",
    static_cast<int>(remoteobjmap.size()),
    static_cast<int>(remoteobj_destr_cand.size())));
  while (!remoteobj_destr_cand.empty()) {
    remoteobj_destr_cand_type::iterator i = remoteobj_destr_cand.begin();
    const ase_long remote_obj_id = *i;
    remoteobj_destr_cand.erase(i);
    const remoteobjmap_type::iterator iter = remoteobjmap.find(remote_obj_id);
    remote_object *const robj = iter->second;
    if (robj->refcount != 0) {
      /* addref()ed by remote after robj is inserted into
       * remoteobj_destr_cand */
      continue;
    }
    if (stream.get()) {
      if (last_op == 2) {
	abort();
      }
      try {
	const ase_string str = serializer->serialize_object_op(op_destroy,
	  true, remote_obj_id);
	DBG_WR(std::fprintf(stderr, "write: %d op=%d\n",
	  static_cast<int>(str.size()), op_destroy));
	DBG_FULL(std::fprintf(stderr, "WRITE [DESTR]\n"));
	last_op = 2;
	stream->write(str.data(), str.size());
	remote_event_loop(0); /* remote should send op_return immediately. */
      } catch (...) {
	DBG_DESTR(fprintf(stderr,
	  "thrown in remote_object::garbage_collect"));
	stream.reset();
      }
    }
    delete robj;
  }
  if (!stream.get()) {
    localobjmap.clear();
  }
}

void
remote_handle::close_connection() throw()
{
  DBG_GC(fprintf(stderr, "close connection %p\n", this));
  stream.reset();
  garbage_collect();
}

ase_variant
remote_handle::remote_event_loop(bool *exret)
{
  /* execute requests from remote until we get an op_return or op_throw
   * response from the remote host. */
  ase_variant exc;
  while (true) {
    try {
      const ase_variant rpkt = packet_read();
      const int op = rpkt.get_element(0).get_int();
      DBG(fprintf(stderr, "read op: %x\n", op));
      const ase_int len = rpkt.get_length();
      if (op == op_return) {
	if (len >= 3 && exret != 0) {
	  (*exret) = rpkt.get_element(2).get_boolean();
	}
	return rpkt.get_element(1);
      } else if (op == op_throw) {
	exc = ase_new_exception_from_strmap(rpkt.get_element(1));
	break;
      }
      wres_type wres;
      execute_request(rpkt, op, wres);
      /* don't allow gc if op == op_destroy, in order to avoid recursion */
      packet_write(wres, (op != op_destroy));
    } catch (const std::exception& e) {
      DBG(fprintf(stderr, "e: %s\n", e.what()));
      close_connection();
      throw;
    } catch (const ase_variant& e) {
      DBG(fprintf(stderr, "v: %s\n", e.get_string().data()));
      close_connection();
      throw;
    } catch (...) {
      DBG(fprintf(stderr, "unknownexc\n"));
      close_connection();
      throw;
    }
  }
  throw exc;
}

ase_shared_ptr<remote_handle>
ase_remote_handle_create(ase_auto_ptr<ase_io_buffered_stream>& stream,
  bool is_client, const ase_serializer_table *ser)
{
  ase_shared_ptr<remote_handle> hnd(new remote_handle(is_client, ser));
  hnd->stream = stream; /* move */
  return hnd;
}

};

ase_variant
ase_remote_client_create(ase_auto_ptr<ase_io_buffered_stream>& stream,
  const ase_serializer_table *ser)
{
  ase_shared_ptr<remote_handle> handle = ase_remote_handle_create(stream,
    true, ser);
  try {
    const ase_variant rpkt = handle->packet_read();
    const int op = rpkt.get_element(0).get_int();
    if (op != op_return) {
      ase_throw_exception("RemoteProtocolError", ase_string());
    }
    return rpkt.get_element(1);
  } catch (...) {
    handle->close_connection(); /* or leaks */
  }
  return ase_variant();
}

void
ase_remote_server_execute(ase_auto_ptr<ase_io_buffered_stream>& stream,
  const ase_variant& obj, const ase_serializer_table *ser)
{
  ase_shared_ptr<remote_handle> handle = ase_remote_handle_create(stream,
    false, ser);
  try {
    wres_type wres;
    wres.reserve(2);
    wres.push_back(op_return);
    wres.push_back(obj);
    handle->packet_write(wres, true);
    handle->remote_event_loop(0);
    handle->close_connection();
  } catch (...) {
    handle->close_connection(); /* or leaks */
    throw;
  }
}

const ase_serializer_table *
ase_serializer_table_get(const ase_string& name)
{
  if (name == "asn1der") {
    return ase_serializer_table_asn1der();
  } else if (name == "asn1xer") {
    return ase_serializer_table_asn1xer();
  } else if (name == "compact") {
    return ase_serializer_table_default();
  }
  return ase_serializer_table_asn1der();
}

