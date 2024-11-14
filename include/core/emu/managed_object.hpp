#pragma once

#include <emu/associative_container.hpp>
#include <memory>

namespace emu
{

namespace detail
{

    template<typename T, typename ...Args>
    constexpr bool is_constructible_from_tuple_args_v = std::is_constructible_v<T, std::tuple<Args...>>;

} // namespace detail

    /**
     * @brief Base class for managed object.
     *
     * This class is a base class for managed object. It relies on a singleton map to store the weak pointer to the object.
     *
     *
     * @tparam Key The key type to store the object.
     * @tparam Value The value type to store.
     */
    template <typename Key, typename Value>
    class managed_object : public std::enable_shared_from_this<Value> {
    public:
        using s_value_type = std::shared_ptr<Value>;
        using w_value_type = std::weak_ptr<Value>;

        using map_t = emu::unordered_map<Key, w_value_type>;

        using pair_t = std::pair<s_value_type, bool>;

    protected:
        static std::pair<map_t, std::mutex>& ressource() {
            static std::pair<map_t, std::mutex> instance;
            return instance;
        }

        // When the object's count reach 0, the object is deleted.
        // In addition, the object is also removed from the map.
        struct deleter {

            Key name;

            void operator()(Value* ptr) const {
                auto& [map, mutex] = ressource();
                std::lock_guard lg(mutex);

                // First we remove the object from the map.
                map.erase(name);

                std::default_delete<Value>{}(ptr);
            }
        };

        template <typename KKey, typename... Args>
        static pair_t try_emplace(KKey&& key, Args&&... args) {

            auto& [map, mutex] = ressource();
            std::lock_guard lg(mutex);

            auto it = map.find(key);
            // check if value exist
            if (it != map.end()) {
                // check if weak ptr is not expired, should never happen
                // but retryving the shared ptr force you to perform the check.
                if (auto s_ptr = it->second.lock(); s_ptr) return {s_ptr, false};
            }

            auto s_ptr = s_value_type(
                new Value(EMU_FWD(args)...), deleter{key});

            map.emplace(EMU_FWD(key), s_ptr);

            return {std::move(s_ptr), true};
        }

        template<typename KKey>
        static auto get(KKey&& key) {
            auto& [map, mutex] = ressource();

            auto it = map.find(key);
            if (it == map.end()) return s_value_type{};
            return it->second.lock();
        }

        template<typename KKey, typename Fn>
        static auto auto_emplace(map_t& map, Key && key, Fn && fn)
        {
            return map.emplace(
                key, s_value_type(fn(), deleter{Key(key)}));

        }

        template<typename KKey, typename Fn>
            requires std::invocable<Fn> && std::is_convertible_v<std::invoke_result_t<Fn>, Value*>
        static pair_t find_or_emplace(KKey&& key, Fn && fn)
        {
            auto& [map, mutex] = ressource();
            std::lock_guard lg(mutex);

            auto it = map.find(key);
            if (it != map.end())
                return {it->second.lock(), false};

            return pair_t{map.emplace(
                key, s_value_type(fn(), deleter{Key(key)})).first->second, true};
        }

        template<typename KKey, typename Fn>
            requires std::invocable<Fn> && std::is_convertible_v<std::invoke_result_t<Fn>, Value*>
        static pair_t emplace_or_throw(KKey&& key, Fn && fn)
        {
            auto& [map, mutex] = ressource();
            std::lock_guard lg(mutex);

            auto it = map.find(key);
            if (it != map.end())
                throw std::runtime_error("Key already exists");

            return pair_t{map.emplace(
                key, s_value_type(fn(), deleter{Key(key)})).first->second, true};
        }

    public:
        s_value_type getptr() { return this->shared_from_this(); }

    };


} // namespace emu
