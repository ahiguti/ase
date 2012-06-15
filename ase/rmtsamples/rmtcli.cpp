
#include <ase/asecclass.hpp>
#include <ase/asearray.hpp>
#include <ase/asestrmap.hpp>
#include <ase/aseexcept.hpp>
#include <ase/asermt_client.hpp>

#include <stdio.h>

#define DBG(x)

struct rmtcli {

  static void usage() {
    ase_throw_exception(
      "Usage: rmtcli.cpp { -h HOST| -f FILENAME} [LOOPCOUNT] [SERIALIZER]",
      ase_string());
  }

  static void main(const ase_variant& gl) {
    const ase_variant args = gl.get("Host").get("Args");
    if (args.get_length() < 2) {
      usage();
    }
    const ase_string a0 = args.get(0).get_string();
    ase_variant hnd;
    if (a0 == "-h") {
      const ase_variant m = ase_new_string_map();
      m.set("host", args.get(1));
      m.set("timeout", 30);
      if (!args.get(3).is_void()) {
	m.set("serializer", args.get(3));
      }
      #if 1
      const ase_string host = args.get(0).get_string();
      const ase_string serializer = args.get(2).get_string();
      hnd = ase_new_remote_client(&m, 1);
      #endif
      #if 0
      const ase_variant cpp = gl.get("CPP");
      printf("t %s\n", cpp.get_string().data());
      hnd = gl.get("CPP").minvoke("Remote", m);
      #endif
    } else if (a0 == "-f") {
      hnd = gl.get("Host").minvoke("Load", args.get(1));
    } else {
      usage();
    }
    const ase_variant rmtobj = hnd.minvoke("Create");
    ase_long loop = args.get(2).get_long();
    if (loop < 1) {
      loop = 1;
    }
    const ase_variant arr = ase_new_array(5);
    for (ase_int i = 0; i < 5; ++i) {
      arr.set(i, i + 1);
    }
    ase_long rval = 0;
    for (ase_long i = 0; i < loop; ++i) {
      const ase_variant x = rmtobj.minvoke("Sum", arr);
      rval = x.get_long();
      if (rval != 15) {
	printf("unexpected value %lld\n", rval);
      }
    }
    printf("done value=%lld\n", rval);
  }

};

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain(const ase_variant& gl)
{
  ase_cclass<rmtcli>::initializer()
    .def("Main", &rmtcli::main)
    ;
  const ase_variant v = ase_cclass<rmtcli>::get_class();
  return v.minvoke("Main", gl);
}

};
