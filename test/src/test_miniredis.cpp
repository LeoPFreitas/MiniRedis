//
// Created by Leonardo Freitas on 31/05/25.
//

#include "miniredis/miniredis.hpp"

#include <thread>
#include <gtest/gtest.h>

// Test for the set operation
TEST(MiniRedisTest, SetOperation)
{
    // Test basic set and get
    miniredis::set("test_key", 42);
    auto result = miniredis::get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);

    // Test overwriting existing value
    miniredis::set("test_key", 100);
    result = miniredis::get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);

    // Test set with a negative value
    miniredis::set("negative_key", -50);
    result = miniredis::get("negative_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -50);

    // Test set with zero value
    miniredis::set("zero_key", 0);
    result = miniredis::get("zero_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    // Test that set resets TTL (set a key with expiration, then set again without expiration)
    miniredis::set("ttl_key", 123);
    EXPECT_TRUE(miniredis::expire("ttl_key", 1)); // Set 1 second TTL

    // Immediately set the same key again (should reset TTL)
    miniredis::set("ttl_key", 456);

    // Wait a bit more than the original TTL
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Key should still exist because set() reset the TTL
    result = miniredis::get("ttl_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 456);

    // Clean up
    miniredis::del("test_key");
    miniredis::del("negative_key");
    miniredis::del("zero_key");
    miniredis::del("ttl_key");
}
