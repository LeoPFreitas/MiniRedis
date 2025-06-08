#include "miniredis/miniredis.hpp"
#include <thread>
#include <gtest/gtest.h>

TEST(MiniRedisTest, SetOperation)
{
    miniredis::MiniRedis redis;

    // Test basic set and get
    redis.set("test_key", 42);
    auto result = redis.get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);

    // Test overwriting existing value
    redis.set("test_key", 100);
    result = redis.get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);

    // Test set with a negative value
    redis.set("negative_key", -50);
    result = redis.get("negative_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -50);

    // Test set with zero value
    redis.set("zero_key", 0);
    result = redis.get("zero_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // Test that set resets TTL (set a key with expiration, then set again without expiration)
    redis.set("ttl_key", 123);
    EXPECT_TRUE(redis.expire("ttl_key", 1)); // Set 1 second TTL

    // Immediately set the same key again (should reset TTL)
    redis.set("ttl_key", 456);

    // Wait a bit more than the original TTL
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Key should still exist because set() reset the TTL
    result = redis.get("ttl_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 456);

    // Clean up
    redis.del("test_key");
    redis.del("negative_key");
    redis.del("zero_key");
    redis.del("ttl_key");
}
