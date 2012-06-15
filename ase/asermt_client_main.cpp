
#include <ase/ase.hpp>
#include <ase/asefuncobj.hpp>
#include <ase/aseio_getaddrinfo.hpp>
#include <ase/aseio_listener.hpp>
#include <ase/aseio_bufstream.hpp>
#include <ase/asermt_protocol.hpp>
#include <ase/asermt_client.hpp>

extern "C" {

ASE_COMPAT_DLLEXPORT ase_variant
ASE_DLLMain()
{
  return ase_new_function_object(&ase_new_remote_client);
}

};

