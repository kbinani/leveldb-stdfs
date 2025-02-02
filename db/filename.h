// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// File names used by DB code

#ifndef STORAGE_LEVELDB_DB_FILENAME_H_
#define STORAGE_LEVELDB_DB_FILENAME_H_

#include <cstdint>
#include <filesystem>
#include <string>

#include "leveldb/slice.h"
#include "leveldb/status.h"

#include "port/port.h"

namespace leveldb {

class Env;

enum FileType {
  kLogFile,
  kDBLockFile,
  kTableFile,
  kDescriptorFile,
  kCurrentFile,
  kTempFile,
  kInfoLogFile  // Either the current one, or an old one
};

// Return the name of the log file with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
std::filesystem::path LogFileName(const std::filesystem::path& dbname,
                                  uint64_t number);

// Return the name of the sstable with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
std::filesystem::path TableFileName(const std::filesystem::path& dbname,
                                    uint64_t number);

// Return the legacy file name for an sstable with the specified number
// in the db named by "dbname". The result will be prefixed with
// "dbname".
std::filesystem::path SSTTableFileName(const std::filesystem::path& dbname,
                                       uint64_t number);

// Return the name of the descriptor file for the db named by
// "dbname" and the specified incarnation number.  The result will be
// prefixed with "dbname".
std::filesystem::path DescriptorFileName(const std::filesystem::path& dbname,
                                         uint64_t number);

// Return the name of the current file.  This file contains the name
// of the current manifest file.  The result will be prefixed with
// "dbname".
std::filesystem::path CurrentFileName(const std::filesystem::path& dbname);

// Return the name of the lock file for the db named by
// "dbname".  The result will be prefixed with "dbname".
std::filesystem::path LockFileName(const std::filesystem::path& dbname);

// Return the name of a temporary file owned by the db named "dbname".
// The result will be prefixed with "dbname".
std::filesystem::path TempFileName(const std::filesystem::path& dbname,
                                   uint64_t number);

// Return the name of the info log file for "dbname".
std::filesystem::path InfoLogFileName(const std::filesystem::path& dbname);

// Return the name of the old info log file for "dbname".
std::filesystem::path OldInfoLogFileName(const std::filesystem::path& dbname);

// If filename is a leveldb file, store the type of the file in *type.
// The number encoded in the filename is stored in *number.  If the
// filename was successfully parsed, returns true.  Else return false.
bool ParseFileName(const std::filesystem::path& filename, uint64_t* number,
                   FileType* type);

// Make the CURRENT file point to the descriptor file with the
// specified number.
Status SetCurrentFile(Env* env, const std::filesystem::path& dbname,
                      uint64_t descriptor_number);

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_FILENAME_H_
