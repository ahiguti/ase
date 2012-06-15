
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/smtp_proxy.hpp>
#include <gtcpp/file_util.hpp>
#include <gtcpp/auto_dir.hpp>
#include <gtcpp/unistd.hpp>
#include <gtcpp/mutex.hpp>
#include <gtcpp/ascii.hpp>
#include <gtcpp/kr_varlen.hpp>
#include <gtcpp/rfc822_util.hpp>
#include <gtcpp/rfc822.hpp>
#include <vector>
#include <list>
#include <sstream>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

namespace {

using namespace gtcpp;

struct kr_param : public kr_varlen_default_param {
};

typedef kr_varlen<kr_param> kr_type;

struct smtp_filter_factory_content : public smtp_filter_factory {

  smtp_filter_factory_content()
    : kr(8, 1024), content_filter_enabled(false) { }
  virtual void create_filter(std::auto_ptr<smtp_filter>& instance_r);

  kr_type kr;
  bool content_filter_enabled;
  string_buffer stamp_pre;
  std::vector<std::string> dnsbl_list;

};

struct smtp_filter_content : public smtp_filter {

  smtp_filter_content(smtp_filter_factory_content& ref)
    : backref(ref) { }

  virtual int filter(string_buffer& obuf, const string_ref& message);

  smtp_filter_factory_content& backref;

};

struct search_funcobj {
  search_funcobj(smtp_filter_content& f) : filter(f) { }
  smtp_filter_content& filter;
  std::string found_ndl;
  bool operator ()(size_t, const std::string& ndl) {
    found_ndl = ndl;
    return true; /* finish searching */
  }
};

void
smtp_filter_factory_content::create_filter(
  std::auto_ptr<smtp_filter>& instance_r)
{
  instance_r.reset(new smtp_filter_content(*this));
}

struct addrinfo_error_callback_nothrow : public addrinfo_error_callback {
  virtual int on_addrinfo_error(int e, const char *) {
    return e;
  }
};

std::string
find_dom_dnsbl(const std::vector<std::string>& dnsbls,
  const std::set<std::string>& doms)
{
  addrinfo_error_callback_nothrow aec;
  for (size_t i = 0; i < dnsbls.size(); ++i) {
    const std::string& bl = dnsbls[i];
    typedef std::set<std::string> doms_type;
    for (doms_type::const_iterator j = doms.begin(); j != doms.end(); ++j) {
      const std::string& dom = *j;
      const std::string qdom = dom + "." + bl;
      const char *qdom_c = qdom.c_str();
      const char *const qdom_c_end = qdom_c + dom.size();
      while (true) {
	auto_addrinfo ai;
	int e = getaddrinfo(ai, qdom_c, 0, 0, AF_INET, SOCK_STREAM, 0, aec);
	if (e == 0) {
	  return dom;
	}
	const char *const p = std::strchr(qdom_c, '.');
	if (p == 0 || p >= qdom_c_end) {
	  break;
	}
	qdom_c = p + 1;
      }
    }
  }
  return std::string();
}

int
smtp_filter_content::filter(string_buffer& obuf,
  const string_ref& message)
{
  obuf.make_space(message.size() + 32);
  string_buffer plain;
  if (backref.content_filter_enabled || !backref.dnsbl_list.empty()) {
    rfc822_to_plain_options opts;
    rfc822_to_plain(plain, message.begin(), message.end(), opts);
  }
  if (backref.content_filter_enabled) {
    search_funcobj fobj(*this);
    backref.kr.search(fobj, plain.begin(), plain.size());
    if (!fobj.found_ndl.empty()) {
      obuf.append(backref.stamp_pre.begin(), backref.stamp_pre.size());
      obuf.append(fobj.found_ndl.data(), fobj.found_ndl.size());
      obuf.append("\r\n", 2);
    }
  }
  if (!backref.dnsbl_list.empty()) {
    typedef std::set<std::string> doms_type;
    doms_type doms;
    rfc822_extract_urls(doms, plain.begin(), plain.end(), false, true);
    std::string found_dom = find_dom_dnsbl(backref.dnsbl_list, doms);
    if (!found_dom.empty()) {
      string_ref stamp_url("X-GTCPP-URL: ");
      obuf.append(stamp_url.begin(), stamp_url.size());
      obuf.append(found_dom.data(), found_dom.size());
      obuf.append("\r\n", 2);
    }
  }
  obuf.append(message.begin(), message.size());
  return 250;

}

void
load_content_filter(smtp_filter_factory_content& ff,
  const std::string& filename)
{
  if (ff.stamp_pre.empty()) {
    ff.stamp_pre.append_literal("X-GTCPP-Filter: ");
  }
  posix_error_callback ec;
  typedef std::set<std::string> strmap_type;
  strmap_type strmap;
  read_string_set(filename.c_str(), strmap, ec);
  ff.kr.resize_buckets(ff.kr.size() + strmap.size() * 2);
  string_buffer sb;
  for (strmap_type::const_iterator i = strmap.begin(); i != strmap.end();
    ++i) {
    fold_space(sb, i->data(), i->data() + i->size());
    const std::string s(sb.begin(), sb.end());
    ff.kr.register_needle(s);
    sb.clear();
  }
  ff.content_filter_enabled = true;
}

void
register_dnsbl_filter(smtp_filter_factory_content& ff,
  const std::string& dnsbl)
{
  if (ff.stamp_pre.empty()) {
    ff.stamp_pre.append_literal("X-GTCPP-Filter: ");
  }
  ff.dnsbl_list.push_back(dnsbl);
}

};

void
smtputil_smtppr(int argc, char **argv)
{
  smtp_filter_factory_content ff;
  std::string host, backend_host, tproxy_local_host, num_threads;
  for (int i = 0; i < argc; ++i) {
    const std::string a(argv[i]);
    if (a == "-content" && i + 1 < argc) {
      load_content_filter(ff, argv[i + 1]);
      i += 1;
    } else if (a == "-dnsbl" && i + 1 < argc) {
      register_dnsbl_filter(ff, argv[i + 1]);
      i += 1;
    } else if (host.empty()) {
      host = a;
    } else if (backend_host.empty()) {
      backend_host = a;
    } else if (tproxy_local_host.empty()) {
      tproxy_local_host = a;
    } else if (num_threads.empty()) {
      num_threads = a;
    }
  }
  if (backend_host.empty()) {
    throw std::runtime_error(
      "usage: smtppr [-content FILENAME]* [-dnsbl DNSBL]* "
      "HOST BACKEND_ADDR [TPROXY_LOCAL_ADDR] [NUM_THREADS]");
  }
  const size_t thr_stack_size = 1024 * 1024;
  smtp_proxy::args a;
  a.ehlo_domain = gethostname();
  a.backend_address = backend_host;
  a.tproxy_local_address = tproxy_local_host;
  smtp_proxy spr(a, ff);
  smtp_server::options opt;
  smtp_server svr(spr, opt);
  tcp_server::options topt;
  topt.address_to_bind = host;
  topt.num_threads = num_threads.empty() ? 100
    : std::atoi(num_threads.c_str());
  topt.thread_stack_size = thr_stack_size;
  concurrent_tcp_server_main(svr, topt);
}

