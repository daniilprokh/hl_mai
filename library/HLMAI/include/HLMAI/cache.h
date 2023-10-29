#ifndef CACHE_H_HL_MAI
#define CACHE_H_HL_MAI

#include "config.h"
#include "singleton.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <string>

class Cache : public Singleton<Cache> {
 public:
  bool Get(uint64_t id, std::string &value);
  void Put(uint64_t id, const std::string &value);
 private:
  friend Singleton<Cache>;

  Cache();
  ~Cache() = default;

  std::mutex mutex_;
  std::shared_ptr<std::iostream> stream_;
};

#endif