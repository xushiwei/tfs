/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: tfs_client_impl.h 579 2011-07-18 08:47:20Z nayan@taobao.com $
 *
 * Authors:
 *   zhuhui <zhuhui_a.pt@taobao.com>
 *      - initial release
 *
 */
#ifndef TFS_CLIENT_TFSCLIENTIMPL_H_
#define TFS_CLIENT_TFSCLIENTIMPL_H_

#include <Mutex.h>
#include <stdio.h>
#include <pthread.h>
#include "common/internal.h"
#include "tfs_session_pool.h"

namespace tfs
{
  namespace common
  {
    class BasePacketFactory;
    class BasePacketStreamer;
  }
  namespace client
  {
    class tbutil::Mutex;
    class TfsFile;
    class TfsSession;
    class GcWorker;
    typedef std::map<int, TfsFile*> FILE_MAP;

    class TfsClientImpl
    {
    public:
      static TfsClientImpl* Instance()
      {
        static TfsClientImpl tfs_client_impl;
        return &tfs_client_impl;
      }

      int initialize(const char* ns_addr, const int32_t cache_time, const int32_t cache_items, const bool start_bg);
      int set_default_server(const char* ns_addr, const int32_t cache_time, const int32_t cache_items);
      int destroy();

      int open(const char* file_name, const char* suffix, const char* ns_addr, const int flags, ...);
      int64_t read(const int fd, void* buf, const int64_t count);
      int64_t readv2(const int fd, void* buf, const int64_t count, common::TfsFileStat* file_info);
      int64_t write(const int fd, const void* buf, const int64_t count);
      int64_t lseek(const int fd, const int64_t offset, const int whence);
      int64_t pread(const int fd, void* buf, const int64_t count, const int64_t offset);
      int64_t pwrite(const int fd, const void* buf, const int64_t count, const int64_t offset);
      int fstat(const int fd, common::TfsFileStat* buf, const common::TfsStatType mode = common::NORMAL_STAT);
      int close(const int fd, char* tfs_name = NULL, const int32_t len = 0);
      int64_t get_file_length(const int fd);

      int set_option_flag(const int fd, const common::OptionFlag option_flag);

      int unlink(const char* file_name, const char* suffix , int64_t& file_size,
                 const common::TfsUnlinkType action = common::DELETE,
                 const common::OptionFlag option_flag = common::TFS_FILE_DEFAULT_OPTION);

      int unlink(const char* file_name, const char* suffix, const char* ns_addr, int64_t& file_size,
                 const common::TfsUnlinkType action = common::DELETE,
                 const common::OptionFlag option_flag = common::TFS_FILE_DEFAULT_OPTION);

      void set_cache_items(const int64_t cache_items);
      int64_t get_cache_items() const;

      void set_cache_time(const int64_t cache_time);
      int64_t get_cache_time() const;

      void set_segment_size(const int64_t segment_size);
      int64_t get_segment_size() const;

      void set_batch_count(const int64_t batch_count);
      int64_t get_batch_count() const;

      void set_stat_interval(const int64_t stat_interval_ms);
      int64_t get_stat_interval() const;

      void set_gc_interval(const int64_t gc_interval_ms);
      int64_t get_gc_interval() const;

      void set_gc_expired_time(const int64_t gc_expired_time_ms);
      int64_t get_gc_expired_time() const;

      void set_batch_timeout(const int64_t time_out_ms);
      int64_t get_batch_timeout() const;

      void set_wait_timeout(const int64_t time_out_ms);
      int64_t get_wait_timeout() const;

      void set_client_retry_count(const int64_t count);
      int64_t get_client_retry_count() const;

      void set_log_level(const char* level);
      void set_log_file(const char* file);

      int32_t get_block_cache_time() const;
      int32_t get_block_cache_items() const;
      int32_t get_cache_hit_ratio() const;


#ifdef WITH_UNIQUE_STORE
#include "tfs_unique_store.h"
      // unique stuff
      TfsUniqueStore* get_unique_store(const char* ns_addr);
      int init_unique_store(const char* master_addr, const char* slave_addr,
                            const char* group_name, const int32_t area, const char* ns_addr);
      int64_t save_unique(const char* buf, const int64_t count,
                          const char* file_name, const char* suffix,
                          char* ret_tfs_name, const int32_t ret_tfs_name_len, const char* ns_addr);
      int64_t save_unique(const char* local_file,
                          const char* file_name, const char* suffix,
                          char* ret_tfs_name, const int32_t ret_tfs_name_len, const char* ns_addr);
      int32_t unlink_unique(const char* file_name, const char* suffix, int64_t& file_size,
                            const int32_t count, const char* ns_addr);
#endif

      // sort of utility
      uint64_t get_server_id();
      int32_t get_cluster_id();
      int64_t save_file(const char* local_file, const char* tfs_name, const char* suffix,
                        char* ret_tfs_name, const int32_t ret_tfs_name_len,
                        const char* ns_addr, const int32_t flag);
      int64_t save_file(const char* buf, const int64_t count, const char* tfs_name, const char* suffix,
                        char* ret_tfs_name, const int32_t ret_tfs_name_len, const char* ns_addr,
                        const int32_t flag, const char* key);
      int fetch_file(const char* local_file, const char* tfs_name, const char* suffix, const char* ns_addr);
      int fetch_file(const char* tfs_name, const char* suffix, char*& buf, int64_t& count, const char* ns_addr);
      int stat_file(const char* tfs_name, const char* suffix,
                    common::TfsFileStat* file_stat, const common::TfsStatType stat_type, const char* ns_addr);

#ifdef TFS_TEST
      TfsSession* get_tfs_session(const char* ns_addr)
      {
        return SESSION_POOL.get(ns_addr);
      }
#endif

    private:
      bool check_init();
      TfsSession* get_session(const char* ns_addr);
      int get_fd();
      TfsFile* get_file(const int fd);
      int insert_file(const int fd, TfsFile* tfs_file);
      int erase_file(const int fd);

    private:
      TfsClientImpl();
      DISALLOW_COPY_AND_ASSIGN(TfsClientImpl);
      ~TfsClientImpl();

      bool is_init_;
      TfsSession* default_tfs_session_;
      int fd_;
      FILE_MAP tfs_file_map_;
      tbutil::Mutex mutex_;
      common::BasePacketFactory* packet_factory_;
      common::BasePacketStreamer* packet_streamer_;
    };
  }
}

#endif  // TFS_CLIENT_TFSCLIENTAPI_H_
