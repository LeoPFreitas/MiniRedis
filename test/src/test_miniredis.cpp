#include "miniredis/miniredis.hpp"
#include <thread>
#include <gtest/gtest.h>

TEST(MiniRedisTest, WhenSettingKeyThenValueCanBeRetrieved)
{
    // Given
    miniredis::MiniRedis redis;

    // When
    redis.set("test_key", 42);

    // Then
    auto result = redis.get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
}

TEST(MiniRedisTest, WhenOverwritingExistingKeyThenValueIsUpdated)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("test_key", 42);

    // When
    redis.set("test_key", 100);

    // Then
    auto result = redis.get("test_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);
}

TEST(MiniRedisTest, WhenSettingNegativeAndZeroValuesThenTheyAreStoredCorrectly)
{
    // Given
    miniredis::MiniRedis redis;

    // When & Then - Negative value
    redis.set("negative_key", -50);
    auto result = redis.get("negative_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -50);

    // When & Then - Zero value
    redis.set("zero_key", 0);
    result = redis.get("zero_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);
}

TEST(MiniRedisTest, WhenSettingKeyAfterExpirationThenTTLIsReset)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("ttl_key", 123);
    EXPECT_TRUE(redis.expire("ttl_key", 1));

    // When
    redis.set("ttl_key", 456);
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Then
    auto result = redis.get("ttl_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 456);
}

TEST(MiniRedisTest, WhenDeletingNonExistentKeyThenReturnsFalse)
{
    // Given
    miniredis::MiniRedis redis;

    // When & Then
    EXPECT_FALSE(redis.del("no_such_key"));
}

TEST(MiniRedisTest, WhenSettingExpirationOnNonExistentKeyThenReturnsFalse)
{
    // Given
    miniredis::MiniRedis redis;

    // When & Then
    EXPECT_FALSE(redis.expire("no_such_key", 10));
}

TEST(MiniRedisTest, WhenSettingZeroOrNegativeTTLThenKeyIsDeleted)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("key1", 1);
    redis.set("key2", 2);

    // When & Then - Zero TTL
    EXPECT_TRUE(redis.expire("key1", 0));
    EXPECT_FALSE(redis.get("key1").has_value());

    // When & Then - Negative TTL
    EXPECT_TRUE(redis.expire("key2", -5));
    EXPECT_FALSE(redis.get("key2").has_value());
}

TEST(MiniRedisTest, WhenKeyExpiresThenGetReturnsNoValue)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("expiring_key", 99);
    redis.expire("expiring_key", 1);

    // When
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    // Then
    EXPECT_FALSE(redis.get("expiring_key").has_value());
}

TEST(MiniRedisTest, WhenCleanUpExpiredIsCalledThenExpiredKeysAreRemoved)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("short_ttl", 1);
    redis.set("long_ttl", 2);
    redis.expire("short_ttl", 1);
    redis.expire("long_ttl", 2);

    // When - After first TTL
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    redis.cleanUpExpired();

    // Then
    EXPECT_FALSE(redis.get("short_ttl").has_value());
    EXPECT_TRUE(redis.get("long_ttl").has_value());

    // When - After second TTL
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    redis.cleanUpExpired();

    // Then
    EXPECT_FALSE(redis.get("long_ttl").has_value());
}

TEST(MiniRedisTest, WhenSettingExtremeIntegerValuesThenTheyAreStoredCorrectly)
{
    // Given
    miniredis::MiniRedis redis;

    // When & Then - Maximum integer value
    redis.set("max_int", std::numeric_limits<int>::max());
    auto result = redis.get("max_int");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), std::numeric_limits<int>::max());

    // When & Then - Minimum integer value
    redis.set("min_int", std::numeric_limits<int>::min());
    result = redis.get("min_int");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), std::numeric_limits<int>::min());
}

TEST(MiniRedisTest, WhenSettingKeysWithSpecialStringsThenTheyAreHandledCorrectly)
{
    // Given
    miniredis::MiniRedis redis;

    // When & Then - Empty string key (edge case)
    redis.set("", 42);
    auto result = redis.get("");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);

    // When & Then - Very long key string
    std::string longKey(1000, 'x');
    redis.set(longKey, 100);
    result = redis.get(longKey);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);
}

TEST(MiniRedisTest, WhenSettingManyKeysRapidlyThenAllAreStoredCorrectly)
{
    // Given
    miniredis::MiniRedis redis;

    // When - Setting many keys to potentially trigger map rehashing
    for (int i = 0; i < 1000; ++i)
    {
        redis.set("key_" + std::to_string(i), i);
    }

    // Then - Verify random keys are correctly stored
    auto result = redis.get("key_0");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0);

    result = redis.get("key_999");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 999);
}

TEST(MiniRedisTest, WhenOverwritingKeyWithExpiredEntryThenTTLIsCleared)
{
    // Given
    miniredis::MiniRedis redis;
    redis.set("overwrite_key", 111);
    redis.expire("overwrite_key", 1);

    // When - Overwrite the key before it expires
    redis.set("overwrite_key", 222);

    // Then - Wait longer than original TTL and verify key still exists
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    auto result = redis.get("overwrite_key");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 222);
}

TEST(MiniRedisTest, WhenSettingWithPotentialHashCollisionsThenAllKeysAreStored)
{
    // Given
    miniredis::MiniRedis redis;

    // When - Create keys that might cause hash collisions
    std::vector<std::string> collisionKeys;
    for (int i = 0; i < 100; ++i)
    {
        // Create keys that might hash to similar values
        std::string key = std::string(i % 10, 'a') + std::to_string(i);
        collisionKeys.push_back(key);
        redis.set(key, i);
    }

    // Then - Verify all keys exist
    for (size_t i = 0; i < collisionKeys.size(); ++i)
    {
        auto result = redis.get(collisionKeys[i]);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), static_cast<int>(i));
    }
}

TEST(MiniRedisTest, WhenSettingVeryLargeNumberOfKeysThenMapHandlesGrowth)
{
    // Given
    miniredis::MiniRedis redis;

    // When - Force multiple map rehashes with large values
    constexpr int numKeys = 1000000; // 1 million keys
    for (int i = 0; i < numKeys; ++i)
    {
        // Use larger strings to consume more memory
        std::string key = "stress_key_" + std::string(100, 'x') + std::to_string(i);
        redis.set(key, i);
    }

    // Then - Verify random keys exist and have correct values
    std::string test_key0 = "stress_key_" + std::string(100, 'x') + "0";
    std::string test_key_mid = "stress_key_" + std::string(100, 'x') + "500000";
    std::string test_key_last = "stress_key_" + std::string(100, 'x') + "999999";

    auto result0 = redis.get(test_key0);
    auto result_mid = redis.get(test_key_mid);
    auto result_last = redis.get(test_key_last);

    ASSERT_TRUE(result0.has_value());
    ASSERT_TRUE(result_mid.has_value());
    ASSERT_TRUE(result_last.has_value());
    EXPECT_EQ(result0.value(), 0);
    EXPECT_EQ(result_mid.value(), 500000);
    EXPECT_EQ(result_last.value(), 999999);
}
