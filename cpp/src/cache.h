#if !defined(CACHE_H)
#define CACHE_H

#include <string>
#include <limits>
#include <functional>

namespace test_ns {

using user_id_t = unsigned;
using user_name_t = std::string;

extern user_name_t getUserByID(user_id_t);

using external_func_t = std::function<user_name_t(user_id_t)>;

class cache {
 public:
    cache(size_t cache_limit = std::numeric_limits<size_t>::max(),
          external_func_t = &test_ns::getUserByID);
    user_name_t getUserName(user_id_t);
 
 private:
    struct cache_impl;
    cache_impl* impl;
};


}  // namespace test_ns

#endif  
