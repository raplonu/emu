#pragma once

#include <emu/utility.hpp>

#include <gsl/pointers>

#include <concepts>
#include <atomic>
#include <utility>

namespace emu
{
    /**
     * @brief A smart pointer-like class that allows manual release of the held object.
     */
    struct capsule
    {
        /**
         * @brief The interface for the held object.
         */
        struct interface {

            interface() = default;
            /**
             * @brief Destructor.
             */
            virtual ~interface() = default;

            std::atomic<long> use_count = 1; /**< The reference count of the held object. */

            /**
             * @brief Increases the reference count of the held object.
             */
            void hold() noexcept {
                ++use_count;
            }

            /**
             * @brief Decreases the reference count of the held object and returns true if the count reaches zero.
             * @return True if the reference count reaches zero, false otherwise.
             */
            bool release() noexcept {
                return (--use_count == 0);
            }

            interface(const interface&) = delete;
            interface(interface&&) = delete;
            interface& operator=(const interface&) = delete;
            interface& operator=(interface&&) = delete;
        };

        /**
         * @brief The implementation of the held object.
         * @tparam DataHolder The type of the held object.
         */
        template<typename DataHolder>
        struct impl : interface {
            /**
             * @brief Constructor.
             * @param d The held object.
             */
            impl(auto&& d) : data_holder(EMU_FWD(d)) {}

            DataHolder data_holder; /**< The held object. */
        };

        gsl::owner<interface*> holder = nullptr; /**< Pointer to the held object. */

        /**
         * @brief Default constructor.
         */
        capsule() = default;

        /**
         * @brief Constructs a capsule object with the given held object.
         * @tparam DataHolder The type of the held object.
         * @param d The held object.
         */
        template<typename DataHolder>
        requires(
                (not is_lref<DataHolder>)
            and (not std::same_as<decay<DataHolder>, capsule   >)
            and (not std::same_as<decay<DataHolder>, interface*>)
        )
        capsule(DataHolder&& d)
            : holder( new impl<DataHolder>( EMU_FWD(d) ) )
        {}

        capsule(gsl::owner<interface*> holder)
            : holder(holder)
        {
            if (holder) holder->hold();
        }

        /**
         * @brief Copy constructor.
         * @param other The capsule object to copy from.
         */
        capsule(capsule const& other)
            : holder(other.holder)
        {
            if (holder) holder->hold();
        }

        /**
         * @brief Move constructor.
         * @param other The capsule object to move from.
         */
        capsule(capsule&& other)
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) - lint bug with std::exchange.
            : holder(std::exchange(other.holder, nullptr))
        {}

        /**
         * @brief Copy assignment operator.
         * @param other The capsule object to copy from.
         * @return Reference to the modified capsule object.
         */
        capsule& operator=(capsule const& other) noexcept {
            reset();
            holder = other.holder;
            if (holder) holder->hold();
            return *this;
        }

        /**
         * @brief Move assignment operator.
         * @param other The capsule object to move from.
         * @return Reference to the modified capsule object.
         */
        capsule& operator=(capsule&& other) noexcept {
            reset();
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) - lint bug with std::exchange.
            holder = std::exchange(other.holder, nullptr);
            return *this;
        }

        /**
         * @brief Destructor.
         */
        ~capsule() {
            reset();
        }

        /**
         * @brief Resets the capsule object by releasing the held object if necessary.
         */
        void reset() noexcept {
            // clang analyzer does not like the delete call here.
            manual_release(holder); //NOLINT(clang-analyzer-cplusplus.NewDelete)
            holder = nullptr;
        }

        /**
         * @brief Returns the reference count of the held object.
         * @return The reference count of the held object.
         */
        [[nodiscard]] long use_count() const noexcept { return holder ? static_cast<long>(holder->use_count) : 0l; }

        /**
         * @brief Conversion operator to bool.
         * @return True if the capsule object holds a valid object, false otherwise.
         */
        operator bool() const noexcept { return static_cast<bool>(holder); }

        static void manual_hold(capsule::interface* holder) {
            if (holder) holder->hold();
        }

        /**
         * Releases the given capsule holder.
         * If the holder is not null and its `release` function returns true, the holder is deleted.
         *
         *
         *
         * @param holder The capsule holder to release.
         */
        static void manual_release(gsl::owner<capsule::interface*> holder) {
            if (holder) // pointer not null
                if (bool can_delete = holder->release(); can_delete) // decrement use count and check if zero
                    delete holder;

        }
    };


} // namespace emu
