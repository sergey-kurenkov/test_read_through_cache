#include "gtest/gtest.h"
#include "cache.h"

#include <memory>
#include <string>
#include <functional>
#include <limits>
#include <chrono>
#include <thread>

namespace test_ns {

const user_name_t test_user_name_prefix = "TestUserName #";

static user_name_t get_test_user_name(user_id_t id) {
    return test_user_name_prefix + std::to_string(id);
}


/*
 *
 */
struct injected_func {
    size_t number_calls = 0;
    size_t sleep_time = 0;

    user_name_t getUserByID(user_id_t id) {
        if (sleep_time > 0)
            std::this_thread::sleep_for(std::chrono::seconds(sleep_time));

        ++number_calls;
        return get_test_user_name(id);
    }
};

}



/*
 *
 */
struct TestInjectedCache : public ::testing::Test {
    TestInjectedCache() {
    }

    void create_cache(size_t upper_limit = std::numeric_limits<size_t>::max()) {
        a_cache.reset(new test_ns::cache(upper_limit, std::bind(
                &test_ns::injected_func::getUserByID, &a_test_func,
                        std::placeholders::_1)));
    }

    void set_sleep_time(size_t seconds) {
        a_test_func.sleep_time = seconds;
    }

    test_ns::injected_func a_test_func;
    std::unique_ptr<test_ns::cache> a_cache;
};

/*
 *
 */
struct TestDefaultCache : public ::testing::Test {
    TestDefaultCache() :
        a_cache(new test_ns::cache(std::numeric_limits<size_t>::max())) {
    }

    void create_cache(size_t upper_limit = std::numeric_limits<size_t>::max()) {
        a_cache.reset(new test_ns::cache(upper_limit));
    }

    std::unique_ptr<test_ns::cache> a_cache;
};

/*
 *
 */
TEST(Cache, BuildDefault) {
    try {
      test_ns::cache *a_cache = new test_ns::cache;
      delete a_cache;
      SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

/*
 *
 */
TEST(Cache, BuildWithUpperLimit) {
    try {
        test_ns::cache a_cache(1);
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST(Cache, BuildWithInjectedFunc) {
    try {
        test_ns::injected_func a_test_func;
        auto injected_func = std::bind(&test_ns::injected_func::getUserByID,
                &a_test_func, std::placeholders::_1);
        test_ns::cache a_cache(std::numeric_limits<size_t>::max(),
                std::move(injected_func));
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckGetSameKey) {
    try {
        create_cache();
        ASSERT_EQ(a_test_func.number_calls, 0);
        test_ns::user_name_t user_name;

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 1);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 1);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 1);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckCacheEviction) {
    try {
        create_cache(1);

        ASSERT_EQ(a_test_func.number_calls, 0);
        test_ns::user_name_t user_name;

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 1);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 1);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());

        user_name = a_cache->getUserName(2);
        ASSERT_EQ(a_test_func.number_calls, 2);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(2).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_EQ(a_test_func.number_calls, 3);
        ASSERT_STREQ(user_name.c_str(), test_ns::get_test_user_name(1).c_str());
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestDefaultCache, CheckGetSameKey) {
    try {
        create_cache();
        test_ns::user_name_t user_name;

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestDefaultCache, CheckCacheEviction) {
    try {
        create_cache(1);

        test_ns::user_name_t user_name;

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());

        user_name = a_cache->getUserName(2);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(2).c_str());

        user_name = a_cache->getUserName(1);
        ASSERT_STREQ(user_name.c_str(), test_ns::getUserByID(1).c_str());
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckGetSameKeyInThreads) {
    try {
        create_cache();
        set_sleep_time(1);

        std::thread t1([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
            ASSERT_EQ(a_test_func.number_calls, 1);
        });
        std::thread t2([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
            ASSERT_EQ(a_test_func.number_calls, 1);
        });
        t1.join();
        t2.join();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckGetDifferentKeysInThreads) {
    try {
        create_cache(1);
        set_sleep_time(1);

        std::thread t1([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
        });
        std::thread t2([this]() {
            ASSERT_STREQ(a_cache->getUserName(2).c_str(),
                    test_ns::get_test_user_name(2).c_str());
        });
        t1.join();
        t2.join();
        ASSERT_EQ(a_test_func.number_calls, 2);
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckGetDifferentKeysIn4Threads) {
    try {
        create_cache(2);
        set_sleep_time(1);

        std::thread t1([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
        });
        std::thread t2([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
        });
        std::thread t3([this]() {
            ASSERT_STREQ(a_cache->getUserName(2).c_str(),
                    test_ns::get_test_user_name(2).c_str());
        });
        std::thread t4([this]() {
            ASSERT_STREQ(a_cache->getUserName(2).c_str(),
                    test_ns::get_test_user_name(2).c_str());
        });
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        ASSERT_EQ(a_test_func.number_calls, 2);
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, CheckGetDifferentKeysIn4ThreadsWithUpLimit1) {
    try {
        create_cache(1);
        set_sleep_time(1);

        std::thread t1([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
        });
        std::thread t2([this]() {
            ASSERT_STREQ(a_cache->getUserName(1).c_str(),
                    test_ns::get_test_user_name(1).c_str());
        });
        std::thread t3([this]() {
            ASSERT_STREQ(a_cache->getUserName(2).c_str(),
                    test_ns::get_test_user_name(2).c_str());
        });
        std::thread t4([this]() {
            ASSERT_STREQ(a_cache->getUserName(2).c_str(),
                    test_ns::get_test_user_name(2).c_str());
        });
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        ASSERT_EQ(a_test_func.number_calls, 2);
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}
