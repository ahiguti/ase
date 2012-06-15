
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/resolver.hpp>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>
#include <vector>
#include <cstring>

namespace gtcpp {

struct resolver::impl_type {

  impl_type() : buffer(4096) {
    std::memset(&state, 0, sizeof(state));
    res_ninit(&state);
  }
  ~impl_type() {
    res_nclose(&state); /* glibc doesn't have res_ndestroy() */
  }

  struct __res_state state;
  std::vector<unsigned char> buffer;

};

resolver::resolver()
  : impl(new impl_type())
{
}

resolver::~resolver()
{
}

namespace {

template <typename T> void
read_uint(const unsigned char *& pos, size_t len, T& val_r)
{
  val_r = 0;
  for (size_t i = 0; i < len; ++i) {
    val_r <<= 8;
    val_r |= pos[0];
    ++pos;
  }
}

};

int /* returns h_errno */
resolver::lookup(const char *name, int typ, std::vector<record>& rec_r)
{
  rec_r.clear();
  char dnbuf[1024]; /* STACK BUFFER */
  const int r = res_nquery(&impl->state, name, C_IN, typ, &impl->buffer[0],
    impl->buffer.size());
  if (r < 0) {
    return impl->state.res_h_errno;
  }
  const size_t repl_len = r;
  if (repl_len > impl->buffer.size()) {
    return NO_RECOVERY;
  }
  const unsigned char *const repl_begin = &impl->buffer[0];
  const unsigned char *const repl_end = repl_begin + repl_len;
  const unsigned char *const repl_query = repl_begin + sizeof(HEADER);
  if (repl_query > repl_end) {
    return NO_RECOVERY;
  }
  const HEADER& hdr = *reinterpret_cast<HEADER *>(&impl->buffer[0]);
  const unsigned int num_query = ntohs(hdr.qdcount);
  const unsigned int num_answer = ntohs(hdr.ancount);
  /* query data in dns reply */
  const unsigned char *cur_pos = repl_query;
  for (unsigned int i = 0; i < num_query; ++i) {
    if (cur_pos >= repl_end) {
      return NO_RECOVERY;
    }
    const int len = dn_expand(repl_begin, repl_end, cur_pos, dnbuf,
      sizeof(dnbuf));
    if (len < 0) {
      return NO_RECOVERY;
    }
    cur_pos += len;      /* QNAME */
    cur_pos += QFIXEDSZ; /* QTYPE(2), QCLASS(2) */
  }
  /* answer data in dns reply */
  for (unsigned int i = 0; i < num_answer; ++i) {
    if (cur_pos >= repl_end) {
      return NO_RECOVERY;
    }
    const int len = dn_expand(repl_begin, repl_end, cur_pos, dnbuf,
      sizeof(dnbuf));
    if (len < 0) {
      return NO_RECOVERY;
    }
    cur_pos += len;        /* NAME */
    if (cur_pos + RRFIXEDSZ > repl_end) {
      return NO_RECOVERY;
    }
    record rec;
    uint16_t rr_rdlength = 0;
    read_uint(cur_pos, 2, rec.rr_type);
    read_uint(cur_pos, 2, rec.rr_class);
    read_uint(cur_pos, 4, rec.rr_ttl);
    read_uint(cur_pos, 2, rr_rdlength);
    const unsigned char *rr_rdata = cur_pos;
    cur_pos += rr_rdlength; /* RDATA */
    if (cur_pos > repl_end) {
      return NO_RECOVERY;
    }
    int dlen = 0;
    switch (rec.rr_type) {
    case T_CNAME:
    case T_MB:
    case T_MG:
    case T_MR:
    case T_NS:
    case T_PTR:
      if ((dlen = dn_expand(repl_begin, repl_end, rr_rdata, dnbuf,
        sizeof(dnbuf))) < 0) {
        return NO_RECOVERY;
      }
      rec.data = std::string(dnbuf);
      break;
    case T_TXT:
      {
        const unsigned char *const rr_rdata_end = rr_rdata + rr_rdlength;
	while (rr_rdata < rr_rdata_end) {
          uint32_t tlen = 0;
          read_uint(rr_rdata, 1, tlen);
	  const uint32_t buf_left = rr_rdata_end - rr_rdata;
	  tlen = std::min(tlen, buf_left);
	  rec.data += std::string(reinterpret_cast<const char *>(rr_rdata),
	    tlen);
	  rr_rdata += tlen;
	}
      }
      break;
    case T_MX:
      read_uint(rr_rdata, 2, rec.mx_pref); /* rr_rdata is modified */
      if ((dlen = dn_expand(repl_begin, repl_end, rr_rdata, dnbuf,
        sizeof(dnbuf))) < 0) {
        return NO_RECOVERY;
      }
      rec.data = std::string(dnbuf);
      break;
    case T_A:
      if (rr_rdlength != 4) {
	return NO_RECOVERY;
      }
      rec.data = std::string(reinterpret_cast<const char *>(rr_rdata),
        rr_rdlength);
      break;
    case T_AAAA:
      if (rr_rdlength != 16) {
        return NO_RECOVERY;
      }
      rec.data = std::string(reinterpret_cast<const char *>(rr_rdata),
        rr_rdlength);
      break;
    default:
      rec.data = std::string(reinterpret_cast<const char *>(rr_rdata),
        rr_rdlength);
      break;
    }
    rec_r.push_back(rec);
  }
  return NETDB_SUCCESS;
}

};

