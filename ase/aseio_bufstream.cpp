
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <vector>
#include <ase/aseio_bufstream.hpp>
#include <ase/aseexcept.hpp>

struct ase_io_buffered_stream_impl
  : public ase_io_buffered_stream, private ase_noncopyable {

  ase_io_buffered_stream_impl(ase_auto_ptr<ase_io_stream>& base);
  virtual void destroy();
  virtual void close();
  virtual void write(const char *buf, ase_size_type size);
  virtual void read_more();
  virtual void get_readbuf(const char *& buf_r, ase_size_type& size_r);
  virtual void readbuf_consume(ase_size_type size);

 private:

  ase_auto_ptr<ase_io_stream> base_buf;
  std::vector<char> read_buffer;

};

ase_io_buffered_stream_impl::ase_io_buffered_stream_impl(
  ase_auto_ptr<ase_io_stream>& base)
  : base_buf(base)
{
}

void
ase_io_buffered_stream_impl::destroy()
{
  delete this;
}

void
ase_io_buffered_stream_impl::close()
{
  base_buf->close();
  read_buffer.clear();
}

void
ase_io_buffered_stream_impl::write(const char *buf, ase_size_type size)
{
  const int x = base_buf->write(buf, size);
  if (x <= 0 || static_cast<ase_size_type>(x) != size) {
    close();
    ase_throw_exception("RemoteWrite", ase_string());
  }
}

void
ase_io_buffered_stream_impl::read_more()
{
  const size_t old_size = read_buffer.size();
  const int read_len = 4096;
  read_buffer.resize(old_size + read_len);
  const int x = base_buf->read(&read_buffer[old_size], read_len);
  if (x <= 0) {
    close();
    ase_throw_exception("RemoteRead", ase_string());
  }
  read_buffer.resize(old_size + x);
}

void
ase_io_buffered_stream_impl::get_readbuf(const char *& buf_r,
  ase_size_type& size_r)
{
  size_r = read_buffer.size();
  if (size_r == 0) {
    buf_r = 0;
  } else {
    buf_r = &read_buffer[0];
  }
}

void
ase_io_buffered_stream_impl::readbuf_consume(ase_size_type size)
{
  read_buffer.erase(read_buffer.begin(), read_buffer.begin() + size);
}

void
ase_io_buffered_stream_create(
  ase_auto_ptr<ase_io_stream>& base_buf /* move */,
  ase_auto_ptr<ase_io_buffered_stream>& bufstr_r)
{
  bufstr_r.reset(new ase_io_buffered_stream_impl(base_buf));
}

