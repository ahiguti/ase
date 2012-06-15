
#include <ase/ase.hpp>
#include <ase/asefuncobj.hpp>
#include <ase/aseio_getaddrinfo.hpp>
#include <ase/aseio_listener.hpp>
#include <ase/aseio_bufstream.hpp>
#include <ase/asermt_protocol.hpp>
#include <ase/asermt_client.hpp>
#include <ase/aseexcept.hpp>

ase_variant
ase_new_remote_client(const ase_variant *args, ase_size_type nargs)
{
  if (nargs < 1) {
    ase_throw_missing_arg("ASERemoteClient");
  }
  const ase_variant a = args[0];
  ase_string host;
  ase_io_socket_options opts;
  opts.timeout_sec = 600;
  ase_string ser_name;
  if (a.get_type() == ase_vtype_object) {
    host = a.get("host").get_string();
    opts.timeout_sec = a.get("timeout").get_int();
    opts.sndbuf = a.get("sndbuf").get_int();
    opts.rcvbuf = a.get("rcvbuf").get_int();
    ser_name = a.get("serializer").get_string();
  } else {
    host = a.get_string();
  }
  const ase_serializer_table *ser = ase_serializer_table_get(ser_name);
  ase_sockaddr_generic addr;
  ase_io_hoststring_to_sockaddr(host.data(), addr);
  ase_auto_ptr<ase_io_stream> s;
  ase_io_socket_create(s, addr, opts);
  ase_auto_ptr<ase_io_buffered_stream> bs;
  ase_io_buffered_stream_create(s, bs);
  return ase_remote_client_create(bs, ser);
}

