/* THIS FILE IS GENERATED. DON'T EDIT. */
#ifdef ASE_CCLASS_GEN_INC
template <typename T> struct ase_cclass<T>::initializer : private ase_noncopyable {
  static ase_variant callkfg(const ase_variant& selfv, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    ase_variant (*const tkf)(const ase_variant *, const ase_size_type) = reinterpret_cast<ase_variant (*)(const ase_variant *, const ase_size_type)>(kf);
    return (*tkf)(a, na);
  }
  initializer& def(const char *n, ase_variant (*kfp)(const ase_variant *, ase_size_type)) {
    ase_cclass_base::add(me, n, callkfg, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  static ase_variant callkfgs(const ase_variant& selfv, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    ase_variant (*const tkf)(const ase_variant&, const ase_variant *, const ase_size_type) = reinterpret_cast<ase_variant (*)(const ase_variant&, const ase_variant *, const ase_size_type)>(kf);
    return (*tkf)(selfv, a, na);
  }
  initializer& def(const char *n, ase_variant (*kfp)(const ase_variant&, const ase_variant *, ase_size_type)) {
    ase_cclass_base::add(me, n, callkfgs, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  static ase_variant callmfg(const ase_variant& selfv, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    ase_variant (T::*const tmf)(const ase_variant *, ase_size_type) = reinterpret_cast<ase_variant (T::*)(const ase_variant *, ase_size_type)>(mf);
    return (tself->value.*tmf)(a, na);
  }
  initializer& def(const char *n, ase_variant (T::*mfp)(const ase_variant *, ase_size_type)) {
    ase_cclass_base::add(me, n, callmfg, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  static ase_variant callmfgs(const ase_variant& selfv, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    ase_variant (T::*const tmf)(const ase_variant&, const ase_variant *, ase_size_type) = reinterpret_cast<ase_variant (T::*)(const ase_variant&, const ase_variant *, ase_size_type)>(mf);
    return (tself->value.*tmf)(selfv, a, na);
  }
  initializer& def(const char *n, ase_variant (T::*mfp)(const ase_variant&, const ase_variant *, ase_size_type)) {
    ase_cclass_base::add(me, n, callmfgs, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)() = reinterpret_cast<void (*)()>(kf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    (*tkf)();
    return ase_variant();
  }
  initializer& def(const char *n, void (*kfp)()) {
    ase_cclass_base::add(me, n, callkfv, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)() = reinterpret_cast<R (*)()>(kf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    R tr = (*tkf)();
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R> initializer& def(const char *n, R (*kfp)()) {
    ase_cclass_base::add(me, n, callkfr<R>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)() = reinterpret_cast<void (T::*)()>(mf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    (tself->value.*tmf)();
    return ase_variant();
  }
  initializer& def(const char *n, void (T::*mfp)()) {
    ase_cclass_base::add(me, n, callmfv, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)() = reinterpret_cast<R (T::*)()>(mf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    R tr = (tself->value.*tmf)();
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R> initializer& def(const char *n, R (T::*mfp)()) {
    ase_cclass_base::add(me, n, callmfr<R>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)() const = reinterpret_cast<void (T::*)() const>(mf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    (tself->value.*tmf)();
    return ase_variant();
  }
  initializer& def(const char *n, void (T::*mfp)() const) {
    ase_cclass_base::add(me, n, callmfvc, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)() const = reinterpret_cast<R (T::*)() const>(mf);
    if (na != 0) { ase_cclass_base::wrong_num_args(na, 0); }
    R tr = (tself->value.*tmf)();
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R> initializer& def(const char *n, R (T::*mfp)() const) {
    ase_cclass_base::add(me, n, callmfrc<R>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0) = reinterpret_cast<void (*)(A0)>(kf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    (*tkf)(ta0);
    return ase_variant();
  }
  template <typename A0> initializer& def(const char *n, void (*kfp)(A0)) {
    ase_cclass_base::add(me, n, callkfv<A0>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0) = reinterpret_cast<R (*)(A0)>(kf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    R tr = (*tkf)(ta0);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0> initializer& def(const char *n, R (*kfp)(A0)) {
    ase_cclass_base::add(me, n, callkfr<R, A0>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0) = reinterpret_cast<void (T::*)(A0)>(mf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    (tself->value.*tmf)(ta0);
    return ase_variant();
  }
  template <typename A0> initializer& def(const char *n, void (T::*mfp)(A0)) {
    ase_cclass_base::add(me, n, callmfv<A0>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0) = reinterpret_cast<R (T::*)(A0)>(mf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    R tr = (tself->value.*tmf)(ta0);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0> initializer& def(const char *n, R (T::*mfp)(A0)) {
    ase_cclass_base::add(me, n, callmfr<R, A0>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0) const = reinterpret_cast<void (T::*)(A0) const>(mf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    (tself->value.*tmf)(ta0);
    return ase_variant();
  }
  template <typename A0> initializer& def(const char *n, void (T::*mfp)(A0) const) {
    ase_cclass_base::add(me, n, callmfvc<A0>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0) const = reinterpret_cast<R (T::*)(A0) const>(mf);
    if (na != 1) { ase_cclass_base::wrong_num_args(na, 1); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    R tr = (tself->value.*tmf)(ta0);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0> initializer& def(const char *n, R (T::*mfp)(A0) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1) = reinterpret_cast<void (*)(A0, A1)>(kf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    (*tkf)(ta0, ta1);
    return ase_variant();
  }
  template <typename A0, typename A1> initializer& def(const char *n, void (*kfp)(A0, A1)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1) = reinterpret_cast<R (*)(A0, A1)>(kf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    R tr = (*tkf)(ta0, ta1);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1> initializer& def(const char *n, R (*kfp)(A0, A1)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1) = reinterpret_cast<void (T::*)(A0, A1)>(mf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    (tself->value.*tmf)(ta0, ta1);
    return ase_variant();
  }
  template <typename A0, typename A1> initializer& def(const char *n, void (T::*mfp)(A0, A1)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1) = reinterpret_cast<R (T::*)(A0, A1)>(mf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    R tr = (tself->value.*tmf)(ta0, ta1);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1> initializer& def(const char *n, R (T::*mfp)(A0, A1)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1) const = reinterpret_cast<void (T::*)(A0, A1) const>(mf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    (tself->value.*tmf)(ta0, ta1);
    return ase_variant();
  }
  template <typename A0, typename A1> initializer& def(const char *n, void (T::*mfp)(A0, A1) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1) const = reinterpret_cast<R (T::*)(A0, A1) const>(mf);
    if (na != 2) { ase_cclass_base::wrong_num_args(na, 2); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    R tr = (tself->value.*tmf)(ta0, ta1);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1> initializer& def(const char *n, R (T::*mfp)(A0, A1) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1, A2) = reinterpret_cast<void (*)(A0, A1, A2)>(kf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    (*tkf)(ta0, ta1, ta2);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2> initializer& def(const char *n, void (*kfp)(A0, A1, A2)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1, A2) = reinterpret_cast<R (*)(A0, A1, A2)>(kf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    R tr = (*tkf)(ta0, ta1, ta2);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2> initializer& def(const char *n, R (*kfp)(A0, A1, A2)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2) = reinterpret_cast<void (T::*)(A0, A1, A2)>(mf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    (tself->value.*tmf)(ta0, ta1, ta2);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2) = reinterpret_cast<R (T::*)(A0, A1, A2)>(mf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2) const = reinterpret_cast<void (T::*)(A0, A1, A2) const>(mf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    (tself->value.*tmf)(ta0, ta1, ta2);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2) const = reinterpret_cast<R (T::*)(A0, A1, A2) const>(mf);
    if (na != 3) { ase_cclass_base::wrong_num_args(na, 3); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1, A2>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1, A2, A3) = reinterpret_cast<void (*)(A0, A1, A2, A3)>(kf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    (*tkf)(ta0, ta1, ta2, ta3);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, void (*kfp)(A0, A1, A2, A3)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1, A2, A3) = reinterpret_cast<R (*)(A0, A1, A2, A3)>(kf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    R tr = (*tkf)(ta0, ta1, ta2, ta3);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, R (*kfp)(A0, A1, A2, A3)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3) = reinterpret_cast<void (T::*)(A0, A1, A2, A3)>(mf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3) = reinterpret_cast<R (T::*)(A0, A1, A2, A3)>(mf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3) const = reinterpret_cast<void (T::*)(A0, A1, A2, A3) const>(mf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3) const = reinterpret_cast<R (T::*)(A0, A1, A2, A3) const>(mf);
    if (na != 4) { ase_cclass_base::wrong_num_args(na, 4); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1, A2, A3>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1, A2, A3, A4) = reinterpret_cast<void (*)(A0, A1, A2, A3, A4)>(kf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    (*tkf)(ta0, ta1, ta2, ta3, ta4);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, void (*kfp)(A0, A1, A2, A3, A4)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1, A2, A3, A4) = reinterpret_cast<R (*)(A0, A1, A2, A3, A4)>(kf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    R tr = (*tkf)(ta0, ta1, ta2, ta3, ta4);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, R (*kfp)(A0, A1, A2, A3, A4)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4) = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4)>(mf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4) = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4)>(mf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4) const = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4) const>(mf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4) const = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4) const>(mf);
    if (na != 5) { ase_cclass_base::wrong_num_args(na, 5); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1, A2, A3, A4>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1, A2, A3, A4, A5) = reinterpret_cast<void (*)(A0, A1, A2, A3, A4, A5)>(kf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    (*tkf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, void (*kfp)(A0, A1, A2, A3, A4, A5)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1, A2, A3, A4, A5) = reinterpret_cast<R (*)(A0, A1, A2, A3, A4, A5)>(kf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    R tr = (*tkf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, R (*kfp)(A0, A1, A2, A3, A4, A5)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4, A5) = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4, A5)>(mf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4, A5)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4, A5) = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4, A5)>(mf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4, A5)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4, A5) const = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4, A5) const>(mf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4, A5) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4, A5) const = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4, A5) const>(mf);
    if (na != 6) { ase_cclass_base::wrong_num_args(na, 6); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4, A5) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1, A2, A3, A4, A5>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callkfv(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    void (*const tkf)(A0, A1, A2, A3, A4, A5, A6) = reinterpret_cast<void (*)(A0, A1, A2, A3, A4, A5, A6)>(kf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    (*tkf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, void (*kfp)(A0, A1, A2, A3, A4, A5, A6)) {
    ase_cclass_base::add(me, n, callkfv<A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callkfr(const ase_variant&, ase_cclass_base::cklass_klsfun_type kf, const ase_variant *a, ase_size_type na) {
    R (*const tkf)(A0, A1, A2, A3, A4, A5, A6) = reinterpret_cast<R (*)(A0, A1, A2, A3, A4, A5, A6)>(kf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    R tr = (*tkf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, R (*kfp)(A0, A1, A2, A3, A4, A5, A6)) {
    ase_cclass_base::add(me, n, callkfr<R, A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callmfv(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4, A5, A6) = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4, A5, A6)>(mf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4, A5, A6)) {
    ase_cclass_base::add(me, n, callmfv<A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callmfr(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4, A5, A6) = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4, A5, A6)>(mf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4, A5, A6)) {
    ase_cclass_base::add(me, n, callmfr<R, A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callmfvc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    void (T::*const tmf)(A0, A1, A2, A3, A4, A5, A6) const = reinterpret_cast<void (T::*)(A0, A1, A2, A3, A4, A5, A6) const>(mf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_variant();
  }
  template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, void (T::*mfp)(A0, A1, A2, A3, A4, A5, A6) const) {
    ase_cclass_base::add(me, n, callmfvc<A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> static ase_variant callmfrc(const ase_variant&, void *self, ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ase_size_type na) {
    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);
    R (T::*const tmf)(A0, A1, A2, A3, A4, A5, A6) const = reinterpret_cast<R (T::*)(A0, A1, A2, A3, A4, A5, A6) const>(mf);
    if (na != 7) { ase_cclass_base::wrong_num_args(na, 7); }
    typename ase_marshal::arg_decl<A0>::type ta0 = ase_marshal::from_variant<A0>::type::func(a[0]);
    typename ase_marshal::arg_decl<A1>::type ta1 = ase_marshal::from_variant<A1>::type::func(a[1]);
    typename ase_marshal::arg_decl<A2>::type ta2 = ase_marshal::from_variant<A2>::type::func(a[2]);
    typename ase_marshal::arg_decl<A3>::type ta3 = ase_marshal::from_variant<A3>::type::func(a[3]);
    typename ase_marshal::arg_decl<A4>::type ta4 = ase_marshal::from_variant<A4>::type::func(a[4]);
    typename ase_marshal::arg_decl<A5>::type ta5 = ase_marshal::from_variant<A5>::type::func(a[5]);
    typename ase_marshal::arg_decl<A6>::type ta6 = ase_marshal::from_variant<A6>::type::func(a[6]);
    R tr = (tself->value.*tmf)(ta0, ta1, ta2, ta3, ta4, ta5, ta6);
    return ase_marshal::to_variant<R>::type::func(tr);
  }
  template <typename R, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6> initializer& def(const char *n, R (T::*mfp)(A0, A1, A2, A3, A4, A5, A6) const) {
    ase_cclass_base::add(me, n, callmfrc<R, A0, A1, A2, A3, A4, A5, A6>, reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));
    return *this;
  }
  initializer(ase_int opt = ase_vtoption_none) : option(opt) { }
  ~initializer() { initialize_internal(*this); /* throws */ }
  const ase_int option;
  ase_cclass_base::cklass_method_entries_type me;
};
#endif
