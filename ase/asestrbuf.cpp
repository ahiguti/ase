
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asestrbuf.hpp>
#include <memory>
#include <assert.h>
#include <string.h>

int xm_constr_count = 0;
int xm_destr_count = 0;

#ifdef NDEBUG
#define X_MALLOC(s)     ::malloc(s)
#define X_REALLOC(p, s) ::realloc(p, s)
#define X_FREE(p)       ::free(p)
#else
#define X_MALLOC(s)     x_malloc(s)
#define X_REALLOC(p, s) x_realloc(p, s)
#define X_FREE(p)       x_free(p)
#endif

#define DBG_MEM(x)

namespace {

const size_t alloc_size_min = 32;

void *
x_malloc(size_t size)
{
  void *r = ::malloc(size);
  DBG_MEM(fprintf(stderr, "m %p\n", r));
  return r;
}

void
x_free(void *ptr)
{
  DBG_MEM(fprintf(stderr, "f %p\n", ptr));
  ::free(ptr);
}

void *
x_realloc(void *ptr, size_t size)
{
  DBG_MEM(fprintf(stderr, "rf %p %zu\n", ptr, size));
  void *r = ::realloc(ptr, size);
  DBG_MEM(fprintf(stderr, "rm %p\n", r));
  return r;
}

};

ase_string_buffer::ase_string_buffer(size_type initial_alloc_size)
  : bufalloc(0), buf_end(0), bufalloc_end(0)
{
  /* we use malloc/free instead of new/delete in order to make debugging
    easier */
  const size_t bufalloc_len = (initial_alloc_size > alloc_size_min)
    ? initial_alloc_size : alloc_size_min;
  bufalloc = static_cast<value_type *>(
    X_MALLOC(bufalloc_len * sizeof(value_type)));
  if (!bufalloc) {
    throw std::bad_alloc();
  }
  buf_end = bufalloc;
  bufalloc_end = bufalloc + bufalloc_len;
  assert_valid();
}

ase_string_buffer::~ase_string_buffer()
{
  assert_valid();
  X_FREE(bufalloc);
}

void
ase_string_buffer::assert_valid() const
{
  assert(bufalloc != 0);
  assert(bufalloc <= buf_end);
  assert(buf_end <= bufalloc_end);
}

void
ase_string_buffer::clear()
{
  buf_end = bufalloc;
  assert_valid();
}

void
ase_string_buffer::reset(size_type initial_alloc_size)
{
  const size_t len = (initial_alloc_size > alloc_size_min)
    ? initial_alloc_size : alloc_size_min;
  if (len != alloc_size()) {
    value_type *p = static_cast<value_type *>(
      X_MALLOC(len * sizeof(value_type)));
    if (!p) {
      throw std::bad_alloc();
    }
    X_FREE(bufalloc);
    bufalloc = p;
    bufalloc_end = bufalloc + len;
  }
  buf_end = bufalloc;
  assert_valid();
}

void
ase_string_buffer::remove_front(size_type len)
{
  assert(len <= size());
  const size_type savesize = size() - len;
  if (savesize != 0) {
    ::memmove(bufalloc, bufalloc + len, savesize);
  }
  buf_end = bufalloc + savesize;
  assert_valid();
}

void
ase_string_buffer::truncate(size_type len)
{
  if (len >= size()) {
    return;
  }
  buf_end = bufalloc + len;
  assert_valid();
}

ase_string_buffer::value_type *
ase_string_buffer::make_space(size_type len)
{
  if (buf_end + len <= bufalloc_end) {
    /* no need to extend */
    return buf_end;
  }
  const size_type buf_end_offset = buf_end - bufalloc;
  const size_type nalloclen = buf_end_offset + len;
  assert(nalloclen > 0);
  value_type *const nptr = static_cast<value_type *>(
    X_REALLOC(bufalloc, nalloclen * sizeof(value_type)));
  if (!nptr) {
    throw std::bad_alloc();
  }
  bufalloc = nptr;
  bufalloc_end = nptr + nalloclen;
  buf_end = nptr + buf_end_offset;
  assert_valid();
  return buf_end;
}

void
ase_string_buffer::append_done(size_type len_written)
{
  buf_end += len_written;
  assert_valid();
}

void
ase_string_buffer::append(const value_type *str, size_type len)
{
  value_type *const p = make_space(len);
  ::memcpy(p, str, len * sizeof(value_type));
  append_done(len);
}

