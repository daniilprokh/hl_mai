#include <HLMAI/cache.h>

#include <HLMAI/config.h>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

Cache::Cache() {
  const std::string& cache_server = 
      Config::GetInstance().GetCacheServer();
  size_t pos = cache_server.find(':');

  std::string host = cache_server.substr(0, pos);
  std::string port = cache_server.substr(pos + 1);

  std::cout << "cache host: " << host << " port: " << port << std::endl;
  stream_ = rediscpp::make_stream(host, port);
}

bool Cache::Get(uint64_t id, std::string &value) {
  std::lock_guard<std::mutex> lck(mutex_);
  rediscpp::value response = rediscpp::execute(*stream_, "get",
                                               std::to_string(id));

  if (response.is_error_message() || response.empty()) {
    return false;
  }
      
  value = response.as<std::string>();
  return true;
}

void Cache::Put(uint64_t id, const std::string &value) {
  std::lock_guard<std::mutex> lck(mutex_);
  rediscpp::value response = rediscpp::execute(*stream_, "set",
                                               std::to_string(id),
                                               value, "ex", "60");
}
