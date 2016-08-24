#if !defined(CACHE_H)
#define CACHE_H

#include <string>
#include <functional>

namespace test_ns {

using user_id_t = unsigned;
using user_name_t = std::string;

extern user_name_t getUserByID(user_id_t);

using std::function<user_name_t(user_id_t)> external_func_t;

class cache {
 public:
    cache(external_func_t = &test_ns::getUserByID, 
          size_t cache_limit = std::numeric_limits<size_t>::max());
    user_name_t getUserName(user_id_t);
 
 private:
    struct cache_impl;
    cache_impl* impl;
};


}  // namespace test_ns

#endif  
