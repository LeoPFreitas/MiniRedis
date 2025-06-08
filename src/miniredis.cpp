//
// Created by Leonardo Freitas on 30/05/25.
//

#include <miniredis/miniredis.hpp>

namespace miniredis
{


    void MiniRedis::set(const std::string& key, const int value)
    {
        std::lock_guard lock(storeMutex);
        store[key] = Entry{value, false, std::chrono::steady_clock::time_point{}};
    }

    std::optional<int> MiniRedis::get(const std::string& key)
    {
        std::lock_guard lock(storeMutex);
        const auto it = store.find(key);
        if (it == store.end())
            return std::nullopt;

        auto& [value, hasExpire, expireTime] = it->second;
        if (hasExpire && std::chrono::steady_clock::now() > expireTime)
        {
            store.erase(it);
            return std::nullopt;
        }

        return value;
    }

    bool MiniRedis::del(const std::string& key)
    {
        std::lock_guard lock(storeMutex);
        return store.erase(key) > 0;
    }

    bool MiniRedis::expire(const std::string& key, int ttlSeconds)
    {
        std::lock_guard lock(storeMutex);
        const auto it = store.find(key);
        if (it == store.end())
            return false;

        if (ttlSeconds <= 0)
        {
            store.erase(it);
            return true;
        }

        it->second.hasExpire = true;
        it->second.expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(ttlSeconds);
        return true;
    }

    void MiniRedis::cleanUpExpired()
    {
        std::lock_guard lock(storeMutex);
        const auto now = std::chrono::steady_clock::now();

        for (auto it = store.begin(); it != store.end();)
        {
            if (it->second.hasExpire && now > it->second.expireTime)
            {
                it = store.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

} // namespace miniredis
