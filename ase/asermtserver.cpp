
#include <ase/asecclass.hpp>
#include <ase/asestrutil.hpp>
#include <ase/aseflags.hpp>
#include <ase/aseio_listener.hpp>
#include <ase/aseio_getaddrinfo.hpp>
#include <ase/asermt_concurrent.hpp>

#define DBG(x)
#define DBG_EXIT(x)

namespace asermtsvr {

namespace {

int ase_server_eng_main(const std::string& prog_name,
  const std::string& bind_addr, int num_threads, size_t thr_stack_size,
  char **engname, int nengname)
{
  ase_init_verbose_flags();
  ase_runtime rt(prog_name.c_str(), 0, 0);
  ase_unified_engine eng(rt);
  const ase_variant topobj = eng.get_ase_global_object();
    /* all the threads share the same topobj */
  for (int i = 0; i < nengname; ++i) {
    try {
      topobj.get_property(ase_string(engname[i])).get_string();
    } catch (const ase_variant& e) {
      DBG(std::fprintf(stderr, "caught %s\n", e.get_string().data()));
    } catch (...) {
      DBG(std::fprintf(stderr, "caught an unknown exception\n"));
    }
  }
  ase_remote_concurrent_server_options sopt;
  ase_io_hoststring_to_sockaddr(bind_addr.c_str(), sopt.address_to_bind);
  ase_auto_ptr<ase_io_listener> lsn;
  ase_io_socket_options sockopts; /* TODO: options */
  ase_io_listener_socket_create(lsn, sockopts);
  ase_remote_concurrent_server_main(*lsn, eng, topobj, sopt);
  return 0;
}

/* TODO: aserun.cpp */
void
show_message_cerr(const std::string& mess)
{
  std::cerr << mess << std::endl;
}

/* TODO: aserun.cpp */
void
show_message_cerr_local_codepage(const std::string& mess)
{
  #ifdef _MSC_VER
  ase_utf16_string wstr(mess.data(), mess.size(),
    ase_utf16_string::permissive());
  std::vector<char> buf(wstr.size() * 2 + 1);
  WideCharToMultiByte(CP_OEMCP, 0, wstr.data(),
    static_cast<int>(wstr.size()), &buf[0],
    static_cast<int>(buf.size()), NULL, NULL);
  buf[buf.size() - 1] = 0;
  show_message_cerr(&buf[0]);
  #else
  show_message_cerr(mess);
  #endif
}

void parse_server_args(int argc, char **argv, ase_string& bind_addr_r,
  ase_io_socket_options& sockopts_r,
  ase_remote_concurrent_server_options& sopt_r)
{
  const ase_variant m = ase_parse_argv(ase_string("asermtserver"), argc, argv);
  bind_addr_r = m.get("host").get_string();
  sopt_r.serializer = m.get("serializer").get_string();
  if (!m.get("thr").is_void()) {
    sopt_r.num_threads = std::atoi(m.get("thr").get_string().data());
  }
  if (!m.get("fork").is_void()) {
    sopt_r.num_procs = std::atoi(m.get("fork").get_string().data());
  }
  if (!m.get("stack").is_void()) {
    sopt_r.thread_stack_size = std::atoi(m.get("stack").get_string().data());
  }
  if (!m.get("timeout").is_void()) {
    sockopts_r.timeout_sec = std::atoi(m.get("timeout").get_string().data());
  }
  if (!m.get("sndbuf").is_void()) {
    sockopts_r.sndbuf = std::atoi(m.get("sndbuf").get_string().data());
  }
  if (!m.get("rcvbuf").is_void()) {
    sockopts_r.rcvbuf = std::atoi(m.get("rcvbuf").get_string().data());
  }
}

};

int ase_server_file_main(const std::string& prog_name,
  const std::string& main_filename, int argc, char **argv)
{
  ase_string bind_addr;
  ase_remote_concurrent_server_options svropt;
  ase_io_socket_options sockopts;
  parse_server_args(argc, argv, bind_addr, sockopts, svropt);
  if (bind_addr.empty()) {
    std::fprintf(stderr,
      "Usage: %s MAINFILENAME asermtserver:host=ADDR [ARGS ...] \n",
      argv[0]);
    return -1;
  }
  int retval = 0;
  try {
    ase_init_verbose_flags();
    ase_runtime rt(prog_name.c_str(), argc, argv);
    ase_unified_engine eng(rt);
    try {
      ase_variant topobj = eng.load_file(ase_string(main_filename));
      ASE_VERBOSE_RUN(show_message_cerr("wait..."));
      eng.wait_threads();
      ASE_VERBOSE_RUN(show_message_cerr("wait done"));
      ase_io_hoststring_to_sockaddr(bind_addr.data(), svropt.address_to_bind);
      ase_auto_ptr<ase_io_listener> lsn;
      ase_io_listener_socket_create(lsn, sockopts);
      ase_remote_concurrent_server_main(*lsn, eng, topobj, svropt);
      retval = 0;
    } catch (...) {
      ase_variant v = ase_variant::create_from_active_exception();
      show_message_cerr_local_codepage(
	std::string("(ASERuntime) uncaught exception\n")
	+ ase_nothrow_get_string(v).data());
      retval = -1;
    }
  } catch (...) {
    ase_variant v = ase_variant::create_from_active_exception();
    show_message_cerr(std::string("(ASERuntime) failed to initialize ASESVR\n")
      + ase_nothrow_get_string(v).data());
    retval = -1;
  }
  return 0;
}

};

int main(int argc, char **argv)
{
  using namespace asermtsvr;
  if (argc < 3) {
    std::fprintf(stderr,
      "Usage: %s MAINFILENAME asermtserver:host=ADDR [ARGS ...] \n",
      argv[0]);
    std::fprintf(stderr, "       %s BINDHOST -t [ENGINE ...] \n", argv[0]);
    return -1;
  }
  int num_threads = 10;
  const size_t thr_stack_size = 1024 * 1024;
  int r = 0;
  if (std::string(argv[2]) == "-t") {
    const std::string bind_host = argv[1];
    const std::string filename = argv[2];
    r = ase_server_eng_main(argv[0], bind_host, num_threads, thr_stack_size,
      argv + 3, argc - 3);
  } else {
    r = ase_server_file_main(argv[0], argv[1], argc - 2, argv + 2);
  }
  DBG_EXIT(std::fprintf(stderr, "asermtsvr: shutdown\n"));
  ase_cclass_base::terminate();
  ase_string::show_statistics();
  ase_variant::show_statistics();
  ase_runtime::show_statistics();
  return r;
}

