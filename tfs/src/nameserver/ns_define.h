/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: ns_define.h 626 2011-08-01 03:52:41Z duanfei@taobao.com $
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *      - initial release
 *   qushan<qushan@taobao.com> 
 *      - modify 2009-03-27
 *   duanfei <duanfei@taobao.com> 
 *      - modify 2010-04-23
 *
 */
#ifndef TFS_NAMESERVER_DEFINE_H_
#define TFS_NAMESERVER_DEFINE_H_ 

#include <Mutex.h>
#include <tbsys.h>
#include "common/internal.h"
#include "common/func.h"
#include "common/lock.h"
#include "common/parameter.h"
#include "common/new_client.h"

//#define TFS_NS_GTEST
//#define TFS_NS_DEBUG

namespace tfs
{
  namespace nameserver
  {
    enum NsRole
    {
      NS_ROLE_NONE = 0x00,
      NS_ROLE_MASTER,
      NS_ROLE_SLAVE
    };

    enum NsStatus
    {
      NS_STATUS_NONE = -1,
      NS_STATUS_UNINITIALIZE = 0x00,
      NS_STATUS_OTHERSIDEDEAD,
      NS_STATUS_ACCEPT_DS_INFO,
      NS_STATUS_INITIALIZED
    };

    enum NsSyncDataFlag
    {
      NS_SYNC_DATA_FLAG_NONE = 0x00,
      NS_SYNC_DATA_FLAG_NO,
      NS_SYNC_DATA_FLAG_READY,
      NS_SYNC_DATA_FLAG_YES
    };

    enum NsDestroyFlag
    {
      NS_DESTROY_FLAGS_NO = 0x00,
      NS_DESTROY_FLAGS_YES
    };

    enum NsSwitchFlag
    {
      NS_SWITCH_FLAG_NO = 0x00,
      NS_SWITCH_FLAG_YES
    };
    class LayoutManager;
    class GCObject
    {
    public:
      explicit GCObject(const time_t now):
        dead_time_(now) {}
      virtual ~GCObject() {}
      virtual void callback(LayoutManager* /**manager*/){}
      inline void free(){ delete this;}
      inline void set_dead_time(const time_t now = time(NULL)) {dead_time_ = now;}
      inline bool can_be_clear(const time_t now = time(NULL)) const
      {
        return now >= (dead_time_ + common::SYSPARAM_NAMESERVER.object_clear_max_time_);
      }
      inline bool is_dead(const time_t now = time(NULL)) const
      {
        return now >= (dead_time_ + common::SYSPARAM_NAMESERVER.object_dead_max_time_);
      }
    private:
      time_t dead_time_;
    };

    struct NsGlobalStatisticsInfo : public common::RWLock
    {
      NsGlobalStatisticsInfo();
      NsGlobalStatisticsInfo(uint64_t use_capacity, uint64_t totoal_capacity, uint64_t total_block_count, int32_t total_load,
          int32_t max_load, int32_t max_block_count, int32_t alive_server_count);
			void update(const common::DataServerStatInfo& info, const bool is_new = true);
      void update(const NsGlobalStatisticsInfo& info);
      inline int64_t get_elect_seq_num()
      {
        common::RWLock::Lock lock(*this, common::READ_LOCKER);
        return elect_seq_num_;
      }
      inline int64_t calc_elect_seq_num_average()
      {
        common::RWLock::Lock lock(*this, common::READ_LOCKER);
        return elect_seq_num_ <= 0 ? 1 : elect_seq_num_ / alive_server_count_ <= 0 ? 1 : alive_server_count_;
      }

      static NsGlobalStatisticsInfo& instance();
      void dump();
      volatile int64_t elect_seq_num_;
      volatile int64_t use_capacity_;
      volatile int64_t total_capacity_;
      volatile int64_t total_block_count_;
      int32_t total_load_;
      int32_t max_load_;
      int32_t max_block_count_;
      volatile int32_t alive_server_count_;
      static const int8_t ELECT_SEQ_NO_INITIALIZE;
      static NsGlobalStatisticsInfo instance_;
    };

    struct NsRuntimeGlobalInformation : public tbutil::Mutex
    {
      uint64_t owner_ip_port_;
      uint64_t other_side_ip_port_;
      int64_t switch_time_;
      int64_t last_owner_check_time_;
      int64_t last_push_owner_check_packet_time_;
      uint32_t vip_;
      NsDestroyFlag destroy_flag_;
      NsRole owner_role_;
      NsRole other_side_role_;
      NsStatus owner_status_;
      NsStatus other_side_status_;
      NsSyncDataFlag sync_oplog_flag_;
      void initialize();
      void dump(int32_t level, const char* file = __FILE__, const int32_t line = __LINE__, const char* function =
          __FUNCTION__) const;
      static NsRuntimeGlobalInformation& instance();
      static NsRuntimeGlobalInformation instance_;
    };

    class BlockCollect;
    class ServerCollect;
    typedef __gnu_cxx ::hash_map<uint64_t, nameserver::ServerCollect*, __gnu_cxx ::hash<uint64_t> > SERVER_MAP;
    typedef SERVER_MAP::iterator SERVER_MAP_ITER;
    typedef __gnu_cxx ::hash_map<uint32_t, nameserver::BlockCollect*, __gnu_cxx ::hash<uint32_t> > BLOCK_MAP;
    typedef BLOCK_MAP::iterator BLOCK_MAP_ITER;

    extern int ns_async_callback(common::NewClient* client);
    extern void print_servers(const std::vector<ServerCollect*>& servers, std::string& result);
    extern void print_servers(const std::vector<uint64_t>& servers, std::string& result);
    extern void print_blocks(const std::vector<uint32_t>& blocks, std::string& result);

  }/** nameserver **/
}/** tfs **/

#endif 
