
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseruntime.hpp>
#include <ase/aseflags.hpp>
#include <ase/aseio.hpp>
#include <ase/aseutf16.hpp>
#include <ase/asemalloc.hpp>
#include <ase/asecclass.hpp>
#include <ase/aseexcept.hpp>

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>

namespace {

#ifdef _WINDOWS

void
show_message_cout(const std::string& mess)
{
  ase_utf16_string wmess(mess.data(), mess.size());
  ::MessageBox(NULL, wmess.data(), NULL, MB_OK);
}

void
show_message_cerr(const std::string& mess)
{
  show_message_cout(mess);
}

#else

void
show_message_cout(const std::string& mess)
{
  std::cout << mess << std::endl;
}

void
show_message_cerr(const std::string& mess)
{
  std::cerr << mess << std::endl;
}

#endif

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
int
aserun_getconf(const std::string& argv0, const std::string& argv1)
{
  ase_init_verbose_flags();
  ase_string libexec = ase_runtime::get_lib_exec(argv0.c_str());
  if (argv1 == "--getlib") {
    libexec = libexec + ASE_DIR_SEP_STR + "libase" + ASE_STATICLIB_SUFFIX;
  } else if (argv1 == "--getjar") {
    libexec = libexec + ASE_DIR_SEP_STR + "asejni.jar";
  }
  show_message_cout(libexec.data());
  return 0;
}

int
aserun_main(const std::string& argv0, const std::string& filename,
  int argc, char **argv, const std::vector<ase_variant>& opts, int loop)
{
  int retval = 0;
  try {
    ase_init_verbose_flags();
    ase_runtime rt(argv0.c_str(), argc, argv);
    for (int lp = 0; lp < loop; ++lp) {
      ase_unified_engine eng(rt);
      try {
	ase_variant r = eng.load_file(ase_string(filename));
	ase_string v = r.get_string();
	ASE_VERBOSE_RUN(show_message_cerr(
	  std::string("(ASERuntime) return value = [") + v.data() + "]"));
	retval = r.get_int();
	ASE_VERBOSE_RUN(show_message_cerr("wait..."));
	eng.wait_threads();
	ASE_VERBOSE_RUN(show_message_cerr("wait done"));
      } catch (...) {
	ase_variant v = ase_variant::create_from_active_exception();
	show_message_cerr_local_codepage(
	  std::string("(ASERuntime) uncaught exception\n")
	  + ase_nothrow_get_string(v).data());
	retval = -1;
      }
    }
  } catch (...) {
    ase_variant v = ase_variant::create_from_active_exception();
    show_message_cerr(std::string("(ASERuntime) failed to initialize ASE\n")
      + ase_nothrow_get_string(v).data());
    retval = -1;
  }
  return retval;
}

void
convert_opts(char **opts, int nopts, std::vector<ase_variant>& vopts_r)
{
  for (int i = 0; i < nopts; ++i) {
    ase_string s(opts[i]);
    vopts_r.push_back(s.to_variant());
  }
}

int
aserun_usage(int argc, char **argv)
{
  std::string argv0(argv[0]);
  show_message_cerr("Usage: " + argv0 + " SCRIPT-FILENAME\n" +
    "       " + argv0 + " ( --getpath | --getlib | --getjar )");
  return 1;
}

int
aserun_main(int argc, char **argv)
{
  std::string argv0(argv[0]);
  if (argc < 2) {
    return aserun_usage(argc, argv);
  }
  std::string arg1 = std::string(argv[1]);
  if (arg1 == "--getpath" || arg1 == "--getlib" || arg1 == "--getjar") {
    return aserun_getconf(argv0, arg1);
  }
  int loop = 1;
  if (argc == 4 && std::string(argv[2]) == "--loop") {
    loop = std::atoi(argv[3]);
    if (loop == 0) {
      loop = 1;
    }
    argc = 2;
  }
  int retval = 0;
  {
    std::vector<ase_variant> vopts;
    convert_opts(argv + 2, argc - 2, vopts);
    retval = aserun_main(argv0, arg1, argc - 2, argv + 2, vopts, loop);
  }
  ase_cclass_base::terminate();
  ase_string::show_statistics();
  ase_variant::show_statistics();
  ase_runtime::show_statistics();
  return retval;
}

}; // anonymous namespace

#ifdef _WINDOWS

/* win32 windows */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdline,
  int nCmdShow)
{
  std::vector<std::string> v;
  std::vector<char *> rawv;
  int argc;
  LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
  try {
    for (int i = 0; i < argc; ++i) {
      ase_string arg(wargv[i], wcslen(wargv[i]));
      v.push_back(arg.data());
    }
    for (int i = 0; i < argc; ++i) {
      rawv.push_back(&v[i][0]);
    }
    GlobalFree(wargv);
    wargv = 0;
    rawv.push_back(0);
    aserun_main(argc, &rawv[0]);
  } catch (...) {
    GlobalFree(wargv);
    show_message_cerr("(ASERuntime) failed to get argv");
  }
}

#else

/* unix or win32 console */
int
main(int argc, char **argv)
{
  #if defined(_MSC_VER) && defined(_DEBUG)
  {
    char *buf = NULL;
    size_t buflen = 0;
    if (_dupenv_s(&buf, &buflen, "ASE_DEBUG_SLEEP")) {
      ::Sleep(10000);
    }
    free(buf);
  }
  // _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
  _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
  _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
  // _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
  _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
  _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
  int tmpDbgFlag = 0;
  tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
  tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
  // tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
  _CrtSetDbgFlag(tmpDbgFlag);
  _CrtMemState memstat;
  // _CrtSetAllocHook(my_alloc_hook);
  show_message_cerr("debug enabled");
  // _CrtSetBreakAlloc(1299);
  aserun_main(argc, argv);
  show_message_cerr("checking leak");
  _CrtMemDumpAllObjectsSince(&memstat);
  #else
  aserun_main(argc, argv);
  #endif
}

#endif
