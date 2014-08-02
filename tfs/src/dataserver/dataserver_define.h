/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: dataserver_define.h 552 2011-06-24 08:44:50Z duanfei@taobao.com $
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *      - initial release
 *
 */
#ifndef TFS_DATASERVER_DATASERVERDEFINE_H_
#define TFS_DATASERVER_DATASERVERDEFINE_H_

#include <string>
#include <assert.h>
#include "common/internal.h"
#include "common/new_client.h"

namespace tfs
{
  namespace dataserver
  {
    static const std::string SUPERBLOCK_NAME = "/fs_super";
    static const std::string MAINBLOCK_DIR_PREFIX = "/";
    static const std::string EXTENDBLOCK_DIR_PREFIX = "/extend/";
    static const std::string INDEX_DIR_PREFIX = "/index/";
    static const mode_t DIR_MODE = 0755;
    static const int32_t MAX_COMPACT_READ_SIZE = 8388608;
    static const char DEV_TAG[common::MAX_DEV_TAG_LEN] = "TAOBAO";
    static const int32_t COPY_BETWEEN_CLUSTER = -1;
    static const int32_t BLOCK_VERSION_MAGIC_NUM = 2;

    // fileinfo flag
    enum FileinfoFlag
    {
      FI_DELETED = 1,
      FI_INVALID = 2,
      FI_CONCEAL = 4
    };

    enum DirtyFlag
    {
      C_DATA_CLEAN = 0,
      C_DATA_DIRTY,
      C_DATA_COMPACT
    };

    enum BlockType
    {
      C_MAIN_BLOCK,
      C_EXT_BLOCK,
      C_COMPACT_BLOCK,
      C_CONFUSE_BLOCK,
    };

    enum BitMapType
    {
      C_ALLOCATE_BLOCK,
      C_ERROR_BLOCK
    };

    enum OperType
    {
      C_OPER_INSERT = 1,
      C_OPER_DELETE,
      C_OPER_UNDELETE,
      C_OPER_UPDATE
    };

    #pragma pack(4) 
    struct BlockPrefix
    {
      uint32_t logic_blockid_;
      uint32_t prev_physic_blockid_;
      uint32_t next_physic_blockid_;
    };
    struct MetaInfo
    {
      public:
        MetaInfo()
        {
          init();
        }

        MetaInfo(const uint64_t file_id, const int32_t in_offset, const int32_t file_size, const int32_t next_meta_offset)
        {
          raw_meta_.set_file_id(file_id);
          raw_meta_.set_offset(in_offset);
          raw_meta_.set_size(file_size);
          next_meta_offset_ = next_meta_offset;
        }

        MetaInfo(const MetaInfo& meta_info)
        {
          memcpy(this, &meta_info, sizeof(MetaInfo));
        }

        explicit MetaInfo(const common::RawMeta& raw_meta)
        {
          raw_meta_ = raw_meta;
          next_meta_offset_ = 0;
        }

        uint64_t get_key() const
        {
          return raw_meta_.get_key();
        }

        void set_key(const uint64_t key)
        {
          raw_meta_.set_key(key);
        }

        uint64_t get_file_id() const
        {
          return raw_meta_.get_file_id();
        }

        void set_file_id(const uint64_t file_id)
        {
          raw_meta_.set_file_id(file_id);
        }

        int32_t get_offset() const
        {
          return raw_meta_.get_offset();
        }

        void set_offset(const int32_t offset)
        {
          raw_meta_.set_offset(offset);
        }

        int32_t get_size() const
        {
          return raw_meta_.get_size();
        }

        void set_size(const int32_t file_size)
        {
          raw_meta_.set_size(file_size);
        }

        common::RawMeta& get_raw_meta()
        {
          return raw_meta_;
        }

        void set_raw_meta(const common::RawMeta& raw_meta)
        {
          raw_meta_ = raw_meta;
        }

        int32_t get_next_meta_offset() const
        {
          return next_meta_offset_;
        }

        void set_next_meta_offset(const int32_t offset)
        {
          next_meta_offset_ = offset;
        }

        MetaInfo& operator=(const MetaInfo& meta_info)
        {
          if (this == &meta_info)
          {
            return *this;
          }
          raw_meta_ = meta_info.raw_meta_;
          next_meta_offset_ = meta_info.next_meta_offset_;
          return *this;
        }

        MetaInfo& clone(const MetaInfo& meta_info)
        {
          assert(this != &meta_info);
          raw_meta_ = meta_info.raw_meta_;
          next_meta_offset_ = meta_info.next_meta_offset_;
          return *this;
        }

        bool operator ==(const MetaInfo& rhs) const
        {
          return raw_meta_ == rhs.raw_meta_ && next_meta_offset_ == rhs.next_meta_offset_;
        }

        common::RawMeta raw_meta_;
        int32_t next_meta_offset_;

      private:
        void init()
        {
          raw_meta_.init();
          next_meta_offset_ = 0;
        }
    };

    struct ClonedBlock
    {
      uint32_t blockid_;
      int32_t start_time_;
      int32_t status_;
    };
    #pragma pack()

    static const int32_t META_INFO_SIZE = sizeof(MetaInfo);

    typedef std::vector<MetaInfo> MetaInfoVec;
    typedef std::vector<MetaInfo>::iterator MetaInfoVecIter;
    typedef std::vector<MetaInfo>::const_iterator MetaInfoVecConstIter;

    typedef __gnu_cxx::hash_map<uint32_t, common::ReplBlock*> ReplBlockMap; // blockid => replblock
    typedef ReplBlockMap::iterator ReplBlockMapIter;
    typedef __gnu_cxx::hash_map<uint32_t, ClonedBlock*> ClonedBlockMap; // blockid => ClonedBlock
    typedef ClonedBlockMap::iterator ClonedBlockMapIter;

    int ds_async_callback(common::NewClient* client);
  }
}

#endif //TFS_DATASERVER_DATASERVERDEFINE_H_
