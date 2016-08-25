#include "gtest/gtest.h"
#include "cache.h"

#include <memory>
#include <string>
#include <functional>
#include <limits>

namespace test_ns {

const user_name_t test_user_name_prefix = "TestUserName #";

/*
 *
 */
struct injected_func {
    size_t number_calls = 0;

    user_name_t getUserByID(user_id_t id) {
        ++number_calls;
        return test_user_name_prefix + std::to_string(id);
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
        test_ns::cache a_cache();
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

TEST_F(TestInjectedCache, Create) {
    try {
        create_cache();
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestInjectedCache, Create1) {
    try {
        create_cache(1);
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestDefaultCache, Create) {
    try {
        create_cache();
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}

TEST_F(TestDefaultCache, Create1) {
    try {
        create_cache(1);
        SUCCEED();
    } catch(std::exception& e) {
        FAIL() << e.what();
    }
}
