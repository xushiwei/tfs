/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: message_factory.h 553 2011-06-24 08:47:47Z duanfei@taobao.com $
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *      - initial release
 *
 */
#ifndef TFS_MESSAGE_MESSAGEFACTORY_H_
#define TFS_MESSAGE_MESSAGEFACTORY_H_

#include "common/base_packet.h"
#include "common/base_packet_factory.h"
#include "dataserver_message.h"
#include "block_info_message.h"
#include "close_file_message.h"
#include "read_data_message.h"
#include "write_data_message.h"
#include "unlink_file_message.h"
#include "replicate_block_message.h"
#include "compact_block_message.h"
#include "server_status_message.h"
#include "file_info_message.h"
#include "rename_file_message.h"
#include "client_cmd_message.h"
#include "create_filename_message.h"
#include "rollback_message.h"
#include "heart_message.h"
#include "reload_message.h"
#include "oplog_sync_message.h"
#include "crc_error_message.h"
#include "admin_cmd_message.h"
#include "dump_plan_message.h"
#include "rc_session_message.h"
#include "get_dataserver_information_message.h"

namespace tfs
{
  namespace message
  {
    class MessageFactory: public common::BasePacketFactory
    {
      public:
        MessageFactory(){}
        virtual ~MessageFactory(){}
        virtual tbnet::Packet* createPacket(int pcode);
    };
  }
}
#endif //TFS_MESSAGE_MESSAGEFACTORY_H_
