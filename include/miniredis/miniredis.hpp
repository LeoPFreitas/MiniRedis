//
// Created by Leonardo Freitas on 30/05/25.
//

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>

namespace miniredis
{

    class MiniRedis
    {
    public:
        MiniRedis() = default;
        ~MiniRedis() = default;

        MiniRedis(const MiniRedis&) = delete;
        MiniRedis& operator=(const MiniRedis&) = delete;

        /**
        * @brief Store or overwrite the integer value for a key.
        * This resets any existing TTL on that key (i.e., after set, the key has no expiration unless expire is called)
        * @param key The unique identifier to store the value
        * @param value The integer value to be stored
        */
        void set(const std::string& key, int value);

        /**
        * @brief Retrieve the value for a key. If the key does not exist or has expired, return std::nullopt.
        * (Using std::optional makes it easy to represent “not found or expired.”)
        * Before returning a value, get checks the stored expiration timestamp and deletes expired keys lazily.
        * @param key The unique identifier to retrieve the value
        * @return std::optional<int> The integer value if it exists, otherwise std::nullopt
        */
        std::optional<int> get(const std::string& key);


        /**
        * @brief Delete a key if it exists, returning true if the key was removed.
        * This also clears any pending expiration for that key.
        * @param key The unique identifier to retrieve the value
        */
        bool del(const std::string& key);

        /**
        * @brief This computes a new expireTime = now + ttlSeconds. If ttlSeconds <= 0, we immediately delete the key.
        * Returns true if the key existed (and was updated or deleted), or false if the key was not found.
        * @param key The unique identifier to set the TTL
        * @param ttlSeconds The time-to-live in seconds
        * @return bool True if the TTL was set, false if the key does not exist
        */
        bool expire(const std::string& key, int ttlSeconds);

        /**
        * @brief Remove all expired keys from storage.
        * This is called internally to perform lazy cleanup of expired entries.
        */
        void cleanUpExpired();

    private:
        struct Entry
        {
            int value{};
            bool hasExpire{};
            std::chrono::steady_clock::time_point expireTime;
        };

        std::unordered_map<std::string, Entry> store;
        mutable std::mutex storeMutex;
    };

} // namespace miniredis
