#include "port/port_config.h"

#if HAVE_ZLIB

#include <algorithm>
#include <zlib.h>

#include "table/compression/zlib_compressor.h"
#include "util/coding.h"

namespace leveldb {

void ZlibCompressorBase::compress(const char* input, size_t length,
                                      ::std::string& buffer) const {
  // reserve enough memory to not reallocate on the fly
  // TODO: this memsets the whole thing to zero, big waste
  buffer.resize(compressBound(length) + 4);

  EncodeFixed32((char*)buffer.data(), (uint64_t)length);

  z_stream strm;
  strm.zalloc = 0;
  strm.zfree = 0;
  strm.next_in = (unsigned char*)(input);
  strm.avail_in = (uint32_t)length;
  strm.next_out = (unsigned char*)(buffer.data() + 4);
  strm.avail_out = buffer.size() - 4;

  auto res = deflateInit2(&strm, compressionLevel, Z_DEFLATED, _window(), 8,
                          Z_DEFAULT_STRATEGY);
  assert(res == Z_OK);

  res = deflate(&strm, Z_FINISH);
  assert(res == Z_STREAM_END);

  buffer.resize(strm.total_out + 4);

  deflateEnd(&strm);
}

int ZlibCompressorBase::inflate(const char* input, size_t length,
                                ::std::string& output) const {
  int ret;
  z_stream strm;

  uint64_t decoded_length = DecodeFixed32(input);

  output.resize(decoded_length);
  /* allocate inflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = (uint32_t)length - 4;
  strm.next_in = (Bytef*)(input + 4);
  strm.avail_out = (uint32_t)decoded_length;
  strm.next_out = (unsigned char*)output.data();

  ret = inflateInit2(&strm, _window());

  if (ret != Z_OK) {
    return ret;
  }

  ret = ::inflate(&strm, Z_FINISH);

  if (ret == Z_NEED_DICT) {
    ret = Z_DATA_ERROR;
  }
  if (ret < 0) {
    (void)inflateEnd(&strm);
    return ret;
  }

  /* clean up and return */
  (void)inflateEnd(&strm);
  return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

bool ZlibCompressorBase::decompress(const char* input, size_t length,
                                    ::std::string& output) const {
  return inflate(input, length, output) == Z_OK;
}

int ZlibCompressorBase::_window() const { return raw ? -15 : 15; }

}  // namespace leveldb

#endif