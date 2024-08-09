#pragma once

#include <emu/fwd.hpp>
#include <emu/config.hpp>
#include <emu/concepts.hpp>
#include <emu/utility.hpp>

#include <gsl/pointers>

#include <atomic>
#include <unistd.h>

namespace emu
{

    template<typename T>
    struct copy
    {
        T& value;
    };

    /**
     * @brief A smart pointer-like class that allows manual release of the held object.
     */
    struct capsule
    {
        /**
         * @brief The interface for the held object.
         */
        struct interface
        {

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
        struct DLL_LOCAL impl : interface
        {
            /**
             * @brief Constructor.
             * @param d The held object.
             */
            impl(auto&& d) : data_holder(EMU_FWD(d)) {}

            ~impl() = default;

            DataHolder data_holder; /**< The held object. */
        };

        gsl::owner<interface*> holder = nullptr; /**< Pointer to the held object. */

        /**
         * @brief Default constructor.
         */
        constexpr capsule() = default;

        /**
         * @brief Constructs a capsule object with the given held object.
         * @tparam DataHolder The type of the held object.
         * @param d The held object.
         */
        template<typename DataHolder>
            requires (not cpts::capsule_owner<DataHolder>)
                 and cpts::not_equivalent<DataHolder, capsule>
                 and (not std::derived_from<rm_ptr<DataHolder>, interface>)
                 and (not is_lref<DataHolder>)
        constexpr explicit capsule(DataHolder&& d)
            : holder( new impl<rm_ref<DataHolder>>( EMU_FWD(d) ) )
        {}

        template <typename T>
        constexpr explicit capsule(copy<T> t)
            : holder( new impl< rm_const<T> >( t.value ) )
        {}

        template <cpts::capsule_owner T>
        constexpr explicit capsule(T&& t) noexcept
            : capsule( t.capsule() )
        {}

        constexpr explicit capsule(gsl::owner<interface*> holder) noexcept
            : holder(holder)
        {
            if (holder) holder->hold();
        }

        /**
         * @brief Copy constructor.
         * @param other The capsule object to copy from.
         */
        constexpr capsule(capsule const& other) noexcept
            : holder(other.holder)
        {
            if (holder) holder->hold();
        }

        /**
         * @brief Move constructor.
         * @param other The capsule object to move from.
         */
        constexpr capsule(capsule&& other) noexcept
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) - lint bug with std::exchange.
            : holder(std::exchange(other.holder, nullptr))
        {}

        /**
         * @brief Copy assignment operator.
         * @param other The capsule object to copy from.
         * @return Reference to the modified capsule object.
         */
        constexpr capsule& operator=(capsule const& other) noexcept {
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
        constexpr capsule& operator=(capsule&& other) noexcept {
            reset();
            // NOLINTNEXTLINE(cppcoreguidelines-owning-memory) - lint bug with std::exchange.
            holder = std::exchange(other.holder, nullptr);
            return *this;
        }

        /**
         * @brief Destructor.
         */
        constexpr ~capsule() noexcept {
            reset();
        }

        /**
         * @brief Resets the capsule object by releasing the held object if necessary.
         */
        constexpr void reset() noexcept {
            // clang analyzer does not like the delete call here.
            manual_release(holder); //NOLINT(clang-analyzer-cplusplus.NewDelete)
            holder = nullptr;
        }

        /**
         * @brief Returns the reference count of the held object.
         * @return The reference count of the held object.
         */
        [[nodiscard]] constexpr long use_count() const noexcept { return holder ? static_cast<long>(holder->use_count) : 0l; }

        /**
         * @brief Conversion operator to bool.
         * @return True if the capsule object holds a valid object, false otherwise.
         */
        constexpr operator bool() const noexcept { return static_cast<bool>(holder); }

        constexpr static void manual_hold(capsule::interface* holder) {
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
        constexpr static void manual_release(gsl::owner<capsule::interface*> holder) {
            if (holder) // pointer not null
                // decrement use count and check if zero
                if (bool can_delete = holder->release(); can_delete)
                    delete holder;

        }
    };

    /**
     * @brief Utility function to create a capsule object from a relocatable range.
     *
     * @tparam Range
     * @param range
     * @return capsule
     */
    template<typename Range>
    constexpr auto capsule_from_range(Range&& range) noexcept -> capsule
    {
        // if constexpr (cpts::relocatable_range<Range>)
        if constexpr (cpts::relocatable_owning_range<Range>)
        {
            if constexpr (is_lref<Range>)
                return capsule( copy{range} );
            else
                return capsule( EMU_FWD(range) );
        }
        else
            return {};
    }

namespace spe
{

    /// std::array are not relocatable regardless if it is a reference or not.
    template <typename T, size_t N>
    inline constexpr bool enable_relocatable_owning_range< std::array<T, N> > = false;

    /// C arrays are not relocatable regardless if it is a reference or not.
    ///TODO: check if it is useful to have this.
    template <typename T>
    inline constexpr bool enable_relocatable_owning_range<T[]> = false;
    /// C arrays are not relocatable regardless if it is a reference or not.
    template <typename T, size_t N>
    inline constexpr bool enable_relocatable_owning_range<T[N]> = false;

} // namespace spe

} // namespace emu
