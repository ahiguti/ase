
#ifndef GENTYPE_ASEREMOTE_IMPL_HPP
#define GENTYPE_ASEREMOTE_IMPL_HPP

#include <ase/aseasn1.hpp>
#include <string>

struct ASERemoteVariant;

namespace aseremote {

class ase_remote_variant {

 public:

  ase_remote_variant();
  ~ase_remote_variant();
  void set_void();
  void set_bool(bool v);
  void set_int(long long v);
  void set_real(double v);
  void set_string(const char *str, size_t len);
  void set_object(bool remote, long long id);
  void set_array();
  void set_map();
  void push_back(ase_remote_variant& v);
  void dump_xer() const;
  size_t get_der_size() const;
  std::string get_der_stdstring() const;
  size_t get_xer_size() const;
  std::string get_xer_stdstring() const;
  bool set_from_ber(const char *str, size_t slen);
  bool set_from_xer(const char *str, size_t slen);
  const struct ASERemoteVariant *get() const;

 private:

  void reset();
  void reallocate();
  void check_error_internal(const char *func);

 private:

  ase_remote_variant(const ase_remote_variant&);
  ase_remote_variant& operator =(const ase_remote_variant&);

  ASERemoteVariant *ptr;

};

int get_type(const ASERemoteVariant *ptr);
bool get_bool(const ASERemoteVariant *ptr);
long long get_int(const ASERemoteVariant *ptr);
double get_real(const ASERemoteVariant *ptr);
void get_string(const ASERemoteVariant *ptr, char *& buf_r, int& len_r);
void get_object(const ASERemoteVariant *ptr, bool& remote_r, long long& id_r);
void get_array(const ASERemoteVariant *ptr, ASERemoteVariant **& arr_r,
  int& size_r);
void to_remote_variant(const ase_variant& value, ase_object_serializer *om,
  ase_remote_variant& rv_r);
void to_remote_variant(const ase_variant *start, const ase_variant *finish,
  ase_object_serializer *om, ase_remote_variant& rv_r);
void to_remote_variant(const ase_variant *const *start,
  const ase_variant *const *finish, ase_object_serializer *om,
  ase_remote_variant& rv_r);
ase_variant from_remote_variant(const ASERemoteVariant *ptr,
  ase_object_deserializer *ou);

};

#endif

