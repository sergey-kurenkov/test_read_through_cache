#include "cache.h"

#include <unordered_map>
#include <mutex>

namespace test_ns {

struct cached_name_t {
    user_name_t user_name;
    size_t mru_index;
};

using lock_t = std::unique_lock<std::mutex>;

struct wait_load_t {
    wait_load_t() : waiting_number (0), loaded(false){
    }
    
    std::mutex mt;
    std::condition_variable cv;
    user_name_t loaded_name;
    unsigned waiting_total;
    bool loaded;
};

struct cache::cache_impl {
    external_func_t func;
    size_t max_cached_number;
    
    std::mutex mt;
    
    size_t mru_index;
    std::unordered_map<user_id_t, cached_name_t> cached_names;
    std::map<size_t, user_id_t> mru_cached_names;
    std::unordered_map<user_id_t, wait_cv> wait_list;

    cache_impl(external_func_t func, size_t max_cached_number) : 
        func(std::move(func)), 
        max_cached_number(max_cached_number),
        mru_index(0U) {
    }
    
    void update_entry(cached_name_t& cached_name, user_id_t id) {
        mru_cached_names.erase(cached_name.mru_index);
        cached_name.mru_index = ++mru_index;
        mru_cached_names.insert(std::make_pair(cached_name.mru_index, id));
    }

    void insert_entry(user_name_t& user_name, user_id_t id) {
        auto insert_pair = cached_names.insert(std::make_pair(id, 
            {user_name, ++mru_index}));
        cached_name_t& cached_name = insert_pair.first->second;
        while (cached_names.size() >= max_cached_number) {
            auto id_to_evict = mru_cached_names.begin()->second;
            mru_cached_names.erase(mru_cached_names.begin());
            cached_names.erase(id_to_evict);
        }
        mru_cached_names.insert(std::make_pair(cached_name.mru_index, id));
    }

    user_name_t getUserName(user_id_t id) {
        lock_t lk(mt);
        auto itr = cached_names.find(id);
        if (itr != cached_names.find()) {
            update_entry(*itr->second, id);
            return itr->second.user_name;
        }
        auto wait_pair = wait_list.insert(std::make_pair(id, wait_load_t()));
        wait_cv& a_wait_cv = *wait_pair.first;
        user_name_t loaded_name;
        bool erase_wait_list = false;
        if (wait_pair.second) {
            lk.unlock();
            loaded_name = func(id);
            lk.lock();
            insert_entry(loaded_name, id);
            lk.unlock();
            
            lock_t wait_lk(a_wait_cv.mt);
            a_wait_cv.loaded = true;
            a_wait_cv.loaded_name = loaded_name;
            if (a_wait_cv.waiting_total == 0) {
                erase_wait_list = true;
            } else {
                a_wait_cv.cv.notify_all();
            }
        } else {
            lock_t wait_lk(a_wait_cv.mt);
            ++a_wait_cv.waiting_total;
            a_wait_cv.wait_for(wait_lk, []() {return loaded;} );
            --a_wait_cv.waiting_total;
            loaded_name = a_wait_cv.loaded_name;
            if (a_wait_cv.waiting_total == 0) {
                erase_wait_list = true;
            }
        }
        if (erase_wait_list) {
            lk.lock();
            wait_list.erase(wait_pair.first);
            lk.unlock();
        }
        return std::move(loaded_name);
    }
    
};

}

/*
 *
 */
test_ns::cache::cache(
    std::function<user_name_t(user_id_t)> func, size_t max_cached_number) :
    impl (new cache_impl{std::move(func), cache_limit}){
}

/*
 *
 */
user_name_t test_ns::cache::getUserName(user_id_t id) {
    return impl->getUserName(id);
}
