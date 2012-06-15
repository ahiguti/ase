
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASERMT_PROTOCOL_HPP
#define GENTYPE_ASERMT_PROTOCOL_HPP

#include <ase/ase.hpp>
#include <ase/aseserialize.hpp>
#include <ase/aseio_bufstream.hpp>

ASELIB_DLLEXT ase_variant ase_remote_client_create(
  ase_auto_ptr<ase_io_buffered_stream>& stream,
  const ase_serializer_table *ser);
ASELIB_DLLEXT void ase_remote_server_execute(
  ase_auto_ptr<ase_io_buffered_stream>& stream, const ase_variant& obj,
  const ase_serializer_table *ser);
ASELIB_DLLEXT const ase_serializer_table *ase_serializer_table_get(
  const ase_string& name);

#endif

