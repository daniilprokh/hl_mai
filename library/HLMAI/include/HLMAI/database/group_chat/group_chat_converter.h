#ifndef GROUP_CHAT_CONVERTER_H_HL_MAI
#define GROUP_CHAT_CONVERTER_H_HL_MAI

#include "group_chat.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/row_converter.h>

namespace database {

class GroupChatConverter : public RowConverter<GroupChatConverter, GroupChat>,
                           public Singleton<GroupChatConverter> 
{
  friend RowConverter<GroupChatConverter, GroupChat>;
  friend Singleton<GroupChatConverter>;

  void FromJSON(GroupChat &groupChat,
                const Poco::JSON::Object::Ptr &jsonObject);
  void ToJSON(const GroupChat &groupChat,
              Poco::JSON::Object::Ptr &jsonObject);
};

}


#endif