#pragma once

#include <emu/utility.hpp>

#include <type_traits>
#include <concepts>
#include <variant>
#include <cstddef>
#include <utility>
#include <memory>
#include <limits>
#include <algorithm>

namespace emu
{

    /**
     * @brief Non owning storage for a reference.
     *
     * Does not own the reference, does not delete it.
     *
     */
    struct ref_storage
    {
        /**
         * @brief Check if the storage can store a reference to T.
         *
         * @tparam T The type to check.
         * @return true if T is a reference type.
         * @return false otherwise.
         */
        template<typename T>
        static constexpr bool can_store = std::is_reference_v<T>;

        /**
         * @brief Construct a new ref storage object
         *
         * @tparam T The type of the reference.
         *
         * @param t The reference to store.
         */
        template<typename T>
            requires (not std::same_as<T, ref_storage>)
        constexpr ref_storage(T& t):
            ptr(reinterpret_cast<std::byte*>(&t))
        {}

        ref_storage(const ref_storage&) = delete;
        ref_storage(ref_storage&&) = default;

        ref_storage& operator=(const ref_storage&) = default;
        ref_storage& operator=(ref_storage&&) = default;

        ~ref_storage() = default;

        /**
         * @brief Get the reference.
         *
         * @tparam T The type of the reference.
         * @return constexpr T& The reference.
         */
        template<typename T>
        constexpr T& get() {
            return *reinterpret_cast<T*>(ptr);
        }

        std::byte* ptr;
    };

    /**
     * @brief Storage for an object of size StorageSize and matching alignment.
     *
     * The storage is allocated on the stack. The object is moved into the storage.
     * The object is destroyed when the storage is destroyed.
     *
     * @tparam StorageSize The size of the storage.
     * @tparam alignment The alignment of the storage. Defaults to alignof(std::max_align_t).
     */
    template<std::size_t StorageSize, std::size_t alignment = alignof(std::max_align_t)>
    struct local_storage
    {
        /**
         * @brief Check if the storage can store an object of type T.
         *
         * @tparam T The type to check.
         * @return true if T can fit in the storage and has the right alignment.
         */
        template<typename T>
        static constexpr bool can_store = sizeof(T) <= StorageSize && alignment % alignof(T) == 0;

        /**
         * @brief Construct a new local storage object
         *
         * @tparam T The type of the object to store.
         *
         * @param t The object to store.
         */
        template<typename T>
            requires (not std::same_as<T, local_storage>)
        constexpr local_storage(T&& t) :
            buffer_{},
            destroy([](std::byte* ptr) {
                // Call destructor on the object.
                std::destroy_at(reinterpret_cast<T*>(ptr));
            })

        {
            static_assert(can_store<T>,
                "emu::local_storage: Trying to construct from an object that won't fit in the local storage.");

            std::construct_at(reinterpret_cast<std::decay_t<T>*>(&buffer_), EMU_FWD(t));
        }

        local_storage(const local_storage&) = delete;
        local_storage(local_storage&& ols) noexcept
            : destroy{std::exchange(ols.destroy, nullptr)}
        {
            std::copy_n(ols.buffer_, StorageSize, buffer_);
        }

        local_storage& operator=(const local_storage&) = delete;
        local_storage& operator=(local_storage&& ols) noexcept {
            std::copy_n(ols.buffer_, StorageSize, buffer_);
            destroy = std::exchange(ols.destroy, nullptr);

            return *this;
        }

        ~local_storage() {
            if (destroy)
                destroy(reinterpret_cast<std::byte*>(&buffer_));
        }

        template <typename T>
        constexpr T& get() {
            return *reinterpret_cast<T*>(&buffer_);
        }

        alignas(alignment) std::byte buffer_[StorageSize];
        void (*destroy)(std::byte*) = nullptr;
    };

    /**
     * @brief Storage for an object of any size.
     *
     * The storage is allocated on the heap. The object is moved into the storage.
     * The object is destroyed when the storage is destroyed.
     *
     * @tparam T The type of the object to store.
     *
     * @todo Add a way to specify the allocator.
     *
     * @todo Add a way to specify the deleter.
     *
     */
    struct dynamic_storage
    {

        template<typename T>
        static constexpr bool can_store = true;

        /**
         * @brief Construct a new dynamic storage object.
         *
         * @tparam T The type of the object to store.
         *
         * @tparam T The type of the object to store.
         *
         * @param t The object to store.
         */
        template<typename T>
            requires (not std::same_as<T, dynamic_storage>)
        dynamic_storage(T&& t):
            buffer_{reinterpret_cast<std::byte*>(new T(std::forward<T>(t)))},
            destroy([](std::byte* ptr) {
                delete reinterpret_cast<T*>(ptr);
            })
        {}

        dynamic_storage(const dynamic_storage&) = delete;
        dynamic_storage(dynamic_storage&& str) noexcept {
            buffer_ = std::exchange(str.buffer_, nullptr);
            destroy = str.destroy;
        }

        dynamic_storage& operator=(const dynamic_storage&) = delete;
        dynamic_storage& operator=(dynamic_storage&& str) noexcept {
            buffer_ = std::exchange(str.buffer_, nullptr);
            destroy = str.destroy;
            return *this;
        }

        ~dynamic_storage() {
            if (buffer_)
                destroy(buffer_);
        }

        template <typename T>
        constexpr T& get() {
            return *reinterpret_cast<T*>(buffer_);
        }

        std::byte* buffer_ = nullptr;
        void (*destroy)(std::byte*) = nullptr;
    };

    template<std::size_t StorageSize, std::size_t Alignment = alignof(std::max_align_t)>
    struct poly_storage
    {

        using local_str = local_storage<StorageSize, Alignment>;

        using buffer_t = std::variant<
            ref_storage,
            local_str,
            dynamic_storage
        >;

        template<typename T>
        static constexpr buffer_t create(T&& t) {

            if constexpr (ref_storage::can_store<T>)
                return ref_storage{t};

            else if constexpr (local_str::template can_store<T>)
                return local_str{std::forward<T>(t)};

            else
                return dynamic_storage{std::forward<T>(t)};
        }

        template<typename T>
        requires (not std::same_as<T, poly_storage>)
        poly_storage(T&& t):
            buffer_(create(std::forward<T>(t)))
        {}

        poly_storage(const poly_storage&) = delete;
        poly_storage(poly_storage&&) = default;

        poly_storage& operator=(const poly_storage&) = delete;
        poly_storage& operator=(poly_storage&&) = default;

        ~poly_storage() = default;

        template<typename T>
        T& get() {
            return std::visit([&](auto& str) -> T& {
                return str.template get<T>();
            }, buffer_);
        }

        buffer_t buffer_;
    };

    template<std::size_t StorageSize, std::size_t Alignment = alignof(std::max_align_t)>
    using storage = poly_storage<StorageSize, Alignment>;

} // namespace emu
