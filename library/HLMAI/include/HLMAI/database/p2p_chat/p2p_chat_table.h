#ifndef P2P_CHAT_TABLE_H_HL_MAI
#define P2P_CHAT_TABLE_H_HL_MAI

#include "p2p_chat.h"

#include <HLMAI/singleton.h>
#include <HLMAI/database/table.h>

#include <optional>

namespace database {

class P2pChatTable : public Table<P2pChat>,
                     public Singleton<P2pChatTable> {
 public:
  virtual void Create() override;
  virtual void SaveToMySQL(P2pChat &p2pChat) override;

  std::optional<uint64_t> GetP2pChatId(uint64_t userId1, uint64_t userId2);
 private:
  friend Singleton<P2pChatTable>;

  P2pChatTable();
  ~P2pChatTable() = default;
};

}

#endif