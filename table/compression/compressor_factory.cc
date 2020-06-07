#include "table/compression/compressor_factory.h"

#include "port/port_config.h"

#if HAVE_SNAPPY
#include "table/compression/snappy_compressor.h"
#endif

#if HAVE_ZLIB
#include "table/compression/zlib_compressor.h"
#endif

#if HAVE_ZSTD
#include "table/compression/zstd_compressor.h"
#endif

namespace leveldb {

Compressor* CompressorFactory::GetCompressor(CompressionType type) {
  switch (type) {
#if HAVE_SNAPPY
    case kSnappyCompression: {
      static SnappyCompressor snappyCompressor;
      return &snappyCompressor;
    }
#endif
#if HAVE_ZLIB
    case kZlibCompression: {
      static ZlibCompressor zlibCompressor;
      return &zlibCompressor;
    }
    case kZlibRawCompression: {
      static ZlibCompressorRaw zlibCompressorRaw;
      return &zlibCompressorRaw;
    }
#endif
#if HAVE_ZSTD
    case kZstdCompression: {
      static ZstdCompressor zstdCompressor;
      return &zstdCompressor;
    }
#endif
    default:
      return nullptr;
  }
}
}  // namespace leveldb
