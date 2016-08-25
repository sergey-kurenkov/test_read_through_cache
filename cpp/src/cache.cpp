#include "cache.h"

#include <unordered_map>
#include <string>
#include <map>
#include <mutex>
#include <utility>
#include <condition_variable>

namespace test_ns {

user_name_t getUserByID(user_id_t id) {
    return "UserName #" + std::to_string(id);
}

struct cached_name_t {
    user_name_t user_name;
    size_t mru_index;
};

using lock_t = std::unique_lock<std::mutex>;

struct wait_load_t {
    wait_load_t() : waiting_total(0U), loaded(false) {
    }

    wait_load_t(wait_load_t&& rhs) : loaded_name(std::move(rhs.loaded_name)),
        waiting_total(rhs.waiting_total), loaded(rhs.loaded) {
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
    std::unordered_map<user_id_t, wait_load_t> wait_list;

    cache_impl(size_t max_cached_number, external_func_t func) :
        func(std::move(func)),
        max_cached_number(max_cached_number),
        mru_index(0U) {
    }

    void update_entry(cached_name_t& cached_name, user_id_t id) {
        mru_cached_names.erase(cached_name.mru_index);
        cached_name.mru_index = ++mru_index;
        mru_cached_names.insert(std::make_pair(cached_name.mru_index, id));
    }

    void evict_lru_entries() {
        while (cached_names.size() >= max_cached_number) {
            auto id_to_evict = mru_cached_names.begin()->second;
            mru_cached_names.erase(mru_cached_names.begin());
            cached_names.erase(id_to_evict);
        }
    }

    void insert_entry(user_name_t& user_name, user_id_t id) {
        evict_lru_entries();
        auto insert_result = cached_names.insert(
                std::make_pair(id, cached_name_t({user_name, ++mru_index})));
        cached_name_t& cached_name = insert_result.first->second;
        mru_cached_names.insert(std::make_pair(cached_name.mru_index, id));
    }

    user_name_t getUserName(user_id_t id);

};

}


test_ns::user_name_t test_ns::cache::cache_impl::getUserName(user_id_t id) {
    lock_t lk(mt);
    auto itr = cached_names.find(id);
    if (itr != cached_names.end()) {
        // cache hit branch
        update_entry(itr->second, id);
        return itr->second.user_name;
    }

    // cache miss branch
    auto wait_pair = wait_list.emplace(std::make_pair(id, wait_load_t()));
    wait_load_t& wait_load = wait_pair.first->second;
    user_name_t loaded_name;
    bool erase_wait_list = false;
    if (wait_pair.second) {
        lk.unlock();
        loaded_name = func(id);
        lk.lock();
        insert_entry(loaded_name, id);
        lk.unlock();

        lock_t wait_lk(wait_load.mt);
        wait_load.loaded = true;
        wait_load.loaded_name = loaded_name;
        if (wait_load.waiting_total == 0) {
            erase_wait_list = true;
        } else {
            wait_load.cv.notify_all();
        }
    } else {
        lock_t wait_lk(wait_load.mt);
        ++wait_load.waiting_total;
        wait_load.cv.wait(wait_lk, [&wait_load]() {return wait_load.loaded;} );
        --wait_load.waiting_total;
        loaded_name = wait_load.loaded_name;
        if (wait_load.waiting_total == 0) {
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


/*
 *
 */
test_ns::cache::cache(size_t max_cached_number,
        std::function<user_name_t(user_id_t)> func) :
    impl (new cache_impl(max_cached_number, std::move(func))){
}

/*
 *
 */
test_ns::user_name_t test_ns::cache::getUserName(user_id_t id) {
    return impl->getUserName(id);
}
