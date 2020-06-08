#ifndef STORAGE_LEVELDB_INCLUDE_DB_ZLIB_COMPRESSOR_H_
#define STORAGE_LEVELDB_INCLUDE_DB_ZLIB_COMPRESSOR_H_

#include "table/compression/compressor.h"

#include <cassert>

namespace leveldb {

	class ZlibCompressorBase : public Compressor 
	{
	public:
		int inflate(const char* input, size_t length, ::std::string &output) const;

		const int compressionLevel;
		const bool raw;
        
        virtual ~ZlibCompressorBase() {
            
        }

		ZlibCompressorBase(int compressionLevel, bool raw) :
			compressionLevel(compressionLevel),
			raw(raw)
		{
			assert(compressionLevel >= -1 && compressionLevel <= 9);
		}

		virtual void compressImpl(const char* input, size_t length, ::std::string& output) const override;

		virtual bool decompress(const char* input, size_t length, ::std::string &output) const override;

	private:

		int _window() const;

	};

	class ZlibCompressor : public ZlibCompressorBase {
	public:
		ZlibCompressor(int compressionLevel = -1) :
			ZlibCompressorBase(compressionLevel, false) {

		}
	};

	class ZlibCompressorRaw : public ZlibCompressorBase {
	public:
		ZlibCompressorRaw(int compressionLevel = -1) :
			ZlibCompressorBase(compressionLevel, true) {

		}
	};
}

#endif