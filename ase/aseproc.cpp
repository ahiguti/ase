
#include <ase/aseproc.hpp>
#include <ase/aseutf16.hpp>
#include <ase/aseexcept.hpp>

#include <cstring>
#include <cstdio>
#include <windows.h>
#include <process.h>

#define DBG(x)

namespace {

class autohandle {

 public:

  autohandle() { h = INVALID_HANDLE_VALUE; }
  ~autohandle() /* DCHK */ { close(); }
  bool close() {
    bool r = (h != INVALID_HANDLE_VALUE) ? (CloseHandle(h) != 0) : false;
    h = INVALID_HANDLE_VALUE;
    return r;
  }
  void reset(HANDLE newhandle) { close(); h = newhandle; }
  bool isValid() const { return h != INVALID_HANDLE_VALUE; }
  handle_t get() const { return h; }
  handle_t release() { HANDLE r = h; h = INVALID_HANDLE_VALUE; return r; }

 private:

  /* noncopyable */
  autohandle(const autohandle&);
  autohandle& operator =(const autohandle&);

  HANDLE h;

};

bool
create_pipe(autohandle& rd, autohandle& wr)
{
  HANDLE hrd = INVALID_HANDLE_VALUE;
  HANDLE hwr = INVALID_HANDLE_VALUE;
  if (!CreatePipe(&hrd, &hwr, NULL, 0)) {
    return false;
  }
  rd.reset(hrd);
  wr.reset(hwr);
  return true;
}

bool
set_inheritable(autohandle& from, autohandle& to)
{
  HANDLE h = INVALID_HANDLE_VALUE;
  if (!DuplicateHandle(GetCurrentProcess(), from.get(),
    GetCurrentProcess(), &h, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
    return false;
  }
  to.reset(h);
  return true;
}

void
read_fill(HANDLE h, std::vector<char>& buf)
{
  while (true) {
    ase_size_type oldlen = buf.size();
    buf.resize(oldlen + 1024);
    char *begin = &buf[oldlen];
    DWORD rlen = 0;
    DBG(fprintf(stderr, "ReadFile...\n"));
    if (ReadFile(h, begin, 1023, &rlen, NULL) == 0 || rlen == 0) {
      break;
    }
    begin[rlen] = 0;
    DBG(fprintf(stderr, "READ: [%s]\n", begin));
    buf.resize(oldlen + rlen);
  }
  DBG(fprintf(stderr, "ReadFile done\n"));
  /* ignore error */
}

}; // anonymous namespace

int
ase_process_read(const ase_string& exec, const ase_string& cmdline,
  ase_string& out_r)
{
  /* create pipes */
  autohandle pipe_parent, pipe_childstdout;
  if (!create_pipe(pipe_parent, pipe_childstdout) ||
      !set_inheritable(pipe_childstdout, pipe_childstdout)) {
    ase_throw_io_error("ASEProc: createpipe failed");
  }

  /* redirect STDERR */
  PROCESS_INFORMATION cpinfo;
  STARTUPINFOW stinfo;
  std::memset(&cpinfo, 0, sizeof(cpinfo));
  std::memset(&stinfo, 0, sizeof(stinfo));
  stinfo.cb = sizeof(stinfo);
  stinfo.dwFlags = STARTF_USESTDHANDLES;
  stinfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  stinfo.hStdOutput = pipe_childstdout.get();
  stinfo.hStdError = pipe_childstdout.get();

  /* create process */
  bool suc = true;
  DBG(fprintf(stderr, "exec=%s\n", exec.data()));
  DBG(fprintf(stderr, "cmd=%s\n", cmdline.data()));
  ase_utf16_string execw(exec);
  ase_utf16_string cmdlinew(cmdline);
  DWORD creflags = 0;
  char *envptr = NULL;
  const WCHAR *curdir = NULL;
  suc = (CreateProcessW(NULL, cmdlinew.DataWr(), NULL, NULL,
    TRUE, creflags, NULL, NULL, &stinfo, &cpinfo) != 0);
  if (!suc) {
    DWORD n = GetLastError();
    ase_variant v(static_cast<ase_int>(n));
    ase_string s = "ASEProc: createprocess failed: err=" + v.get_string()
      + " command= " + cmdline;
    ase_throw_error(s.data());
  }
  pipe_childstdout.close();
  CloseHandle(cpinfo.hThread);
  autohandle prochandle;
  prochandle.reset(cpinfo.hProcess);

  /* read data from pipe */
  std::vector<char> buffer;
  read_fill(pipe_parent.get(), buffer);
  
  /* wait child process to exit */
  if (WaitForSingleObject(prochandle.get(), INFINITE) != WAIT_OBJECT_0) {
    ase_throw_io_error("ASEProc: waitforsingleobject failed");
  }
  DWORD ec = 0;
  if (!GetExitCodeProcess(prochandle.get(), &ec)) {
    ase_throw_io_error("ASEProc: getexitcodeprocess failed");
  }
  int r = static_cast<int>(ec);
  if (!buffer.empty()) {
    out_r = ase_string(&buffer[0], buffer.size());
  }
  return r;
}

