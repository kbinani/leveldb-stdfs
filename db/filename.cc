// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/filename.h"

#include <cassert>
#include <cstdio>

#include "db/dbformat.h"
#include "leveldb/env.h"
#include "util/logging.h"

namespace leveldb {

// A utility routine: write "data" to the named file and Sync() it.
Status WriteStringToFileSync(Env* env, const Slice& data,
                             const std::filesystem::path& fname);

static std::filesystem::path MakeFileName(const std::filesystem::path& dbname,
                                          uint64_t number, const char* suffix) {
  char buf[100];
  std::snprintf(buf, sizeof(buf), "%06llu.%s",
                static_cast<unsigned long long>(number), suffix);
  return dbname / buf;
}

std::filesystem::path LogFileName(const std::filesystem::path& dbname,
                                  uint64_t number) {
  assert(number > 0);
  return MakeFileName(dbname, number, "log");
}

std::filesystem::path TableFileName(const std::filesystem::path& dbname,
                                    uint64_t number) {
  assert(number > 0);
  return MakeFileName(dbname, number, "ldb");
}

std::filesystem::path SSTTableFileName(const std::filesystem::path& dbname,
                                       uint64_t number) {
  assert(number > 0);
  return MakeFileName(dbname, number, "sst");
}

std::string DescriptorFileBaseName(uint64_t number) {
  char buf[100];
  std::snprintf(buf, sizeof(buf), "MANIFEST-%06llu",
                static_cast<unsigned long long>(number));
  return buf;
}

std::filesystem::path DescriptorFileName(const std::filesystem::path& dbname,
                                         uint64_t number) {
  assert(number > 0);
  std::string baseName = DescriptorFileBaseName(number);
  return dbname / baseName;
}

std::filesystem::path CurrentFileName(const std::filesystem::path& dbname) {
  return dbname / "CURRENT";
}

std::filesystem::path LockFileName(const std::filesystem::path& dbname) {
  return dbname / "LOCK";
}

std::filesystem::path TempFileName(const std::filesystem::path& dbname,
                                   uint64_t number) {
  assert(number > 0);
  return MakeFileName(dbname, number, "dbtmp");
}

std::filesystem::path InfoLogFileName(const std::filesystem::path& dbname) {
  return dbname / "LOG";
}

// Return the name of the old info log file for "dbname".
std::filesystem::path OldInfoLogFileName(const std::filesystem::path& dbname) {
  return dbname / "LOG.old";
}

// Owned filenames have the form:
//    dbname/CURRENT
//    dbname/LOCK
//    dbname/LOG
//    dbname/LOG.old
//    dbname/MANIFEST-[0-9]+
//    dbname/[0-9]+.(log|sst|ldb)
bool ParseFileName(const std::filesystem::path& filename, uint64_t* number,
                   FileType* type) {
  namespace fs = std::filesystem;
  fs::path::string_type rest(filename);
  if (rest == _T("CURRENT")) {
    *number = 0;
    *type = kCurrentFile;
  } else if (rest == _T("LOCK")) {
    *number = 0;
    *type = kDBLockFile;
  } else if (rest == _T("LOG") || rest == _T("LOG.old")) {
    *number = 0;
    *type = kInfoLogFile;
  } else if (rest.find(_T("MANIFEST-")) == 0) {
    rest = rest.substr(9);
    uint64_t num;
    if (!ConsumeDecimalNumber(&rest, &num)) {
      return false;
    }
    if (!rest.empty()) {
      return false;
    }
    *type = kDescriptorFile;
    *number = num;
  } else {
    // Avoid strtoull() to keep filename format independent of the
    // current locale
    uint64_t num;
    if (!ConsumeDecimalNumber(&rest, &num)) {
      return false;
    }
    fs::path::string_type suffix = rest;
    if (suffix == _T(".log")) {
      *type = kLogFile;
    } else if (suffix == _T(".sst") || suffix == _T(".ldb")) {
      *type = kTableFile;
    } else if (suffix == _T(".dbtmp")) {
      *type = kTempFile;
    } else {
      return false;
    }
    *number = num;
  }
  return true;
}

Status SetCurrentFile(Env* env, const std::filesystem::path& dbname,
                      uint64_t descriptor_number) {
  // Remove leading "dbname/" and add newline to manifest file name
  std::string manifest = DescriptorFileBaseName(descriptor_number);
  Slice contents = manifest;
  std::filesystem::path tmp = TempFileName(dbname, descriptor_number);
  Status s = WriteStringToFileSync(env, contents.ToString() + "\n", tmp);
  if (s.ok()) {
    s = env->RenameFile(tmp, CurrentFileName(dbname));
  }
  if (!s.ok()) {
    env->RemoveFile(tmp);
  }
  return s;
}

}  // namespace leveldb
