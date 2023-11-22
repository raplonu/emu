#include <gtest/gtest.h>
#include <emu/expected.hpp>

// Tests from https://github.com/TartanLlama/expected

TEST(SimpleAssignment, assignmentSimple) {
  emu::expected<int, int> e1 = 42;
  emu::expected<int, int> e2 = 17;
  emu::expected<int, int> e3 = 21;
  emu::expected<int, int> e4 = emu::make_unexpected(42);
  emu::expected<int, int> e5 = emu::make_unexpected(17);
  emu::expected<int, int> e6 = emu::make_unexpected(21);

  e1 = e2;
  EXPECT_TRUE(e1);
  EXPECT_TRUE(*e1 == 17);
  EXPECT_TRUE(e2);
  EXPECT_TRUE(*e2 == 17);

  e1 = std::move(e2);
  EXPECT_TRUE(e1);
  EXPECT_TRUE(*e1 == 17);
  EXPECT_TRUE(e2);
  EXPECT_TRUE(*e2 == 17);

  e1 = 42;
  EXPECT_TRUE(e1);
  EXPECT_TRUE(*e1 == 42);

  auto unex = emu::make_unexpected(12);
  e1 = unex;
  EXPECT_TRUE(!e1);
  EXPECT_TRUE(e1.error() == 12);

  e1 = emu::make_unexpected(42);
  EXPECT_TRUE(!e1);
  EXPECT_TRUE(e1.error() == 42);

  e1 = e3;
  EXPECT_TRUE(e1);
  EXPECT_TRUE(*e1 == 21);

  e4 = e5;
  EXPECT_TRUE(!e4);
  EXPECT_TRUE(e4.error() == 17);

  e4 = std::move(e6);
  EXPECT_TRUE(!e4);
  EXPECT_TRUE(e4.error() == 21);

  e4 = e1;
  EXPECT_TRUE(e4);
  EXPECT_TRUE(*e4 == 21);
}

TEST(AssignmentDeletion, assignmentDeletion) {
  struct has_all {
    has_all() = default;
    has_all(const has_all &) = default;
    has_all(has_all &&) noexcept = default;
    has_all &operator=(const has_all &) = default;
  };

  emu::expected<has_all, has_all> e1 = {};
  emu::expected<has_all, has_all> e2 = {};
  e1 = e2;

  struct except_move {
    except_move() = default;
    except_move(const except_move &) = default;
    except_move(except_move &&) noexcept(false){};
    except_move &operator=(const except_move &) = default;
  };
  emu::expected<except_move, except_move> e3 = {};
  emu::expected<except_move, except_move> e4 = {};
  // e3 = e4; should not compile
}


#include <string>

// Old versions of GCC don't have the correct trait names. Could fix them up if needs be.
#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
// nothing for now
#else
TEST(Triviality, basesTriviality) {
    static_assert(std::is_trivially_copy_constructible<emu::expected<int,int>>::value, "");
    static_assert(std::is_trivially_copy_assignable<emu::expected<int,int>>::value, "");
    static_assert(std::is_trivially_move_constructible<emu::expected<int,int>>::value, "");
    static_assert(std::is_trivially_move_assignable<emu::expected<int,int>>::value, "");
    static_assert(std::is_trivially_destructible<emu::expected<int,int>>::value, "");

    static_assert(std::is_trivially_copy_constructible<emu::expected<void,int>>::value, "");
    static_assert(std::is_trivially_move_constructible<emu::expected<void,int>>::value, "");
    static_assert(std::is_trivially_destructible<emu::expected<void,int>>::value, "");


    {
        struct T {
            T(const T&) = default;
            T(T&&) = default;
            T& operator=(const T&) = default;
            T& operator=(T&&) = default;
            ~T() = default;
        };
        static_assert(std::is_trivially_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_trivially_copy_assignable<emu::expected<T,int>>::value, "");
        static_assert(std::is_trivially_move_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_trivially_move_assignable<emu::expected<T,int>>::value, "");
        static_assert(std::is_trivially_destructible<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(const T&){}
            T(T&&) {};
            T& operator=(const T&) { return *this; }
            T& operator=(T&&) { return *this; };
            ~T(){}
        };
        static_assert(!std::is_trivially_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(!std::is_trivially_copy_assignable<emu::expected<T,int>>::value, "");
        static_assert(!std::is_trivially_move_constructible<emu::expected<T,int>>::value, "");
        static_assert(!std::is_trivially_move_assignable<emu::expected<T,int>>::value, "");
        static_assert(!std::is_trivially_destructible<emu::expected<T,int>>::value, "");
    }

}

TEST(Deletion, basesDeletion) {
    static_assert(std::is_copy_constructible<emu::expected<int,int>>::value, "");
    static_assert(std::is_copy_assignable<emu::expected<int,int>>::value, "");
    static_assert(std::is_move_constructible<emu::expected<int,int>>::value, "");
    static_assert(std::is_move_assignable<emu::expected<int,int>>::value, "");
    static_assert(std::is_destructible<emu::expected<int,int>>::value, "");

    {
        struct T {
            T()=default;
        };
        static_assert(std::is_default_constructible<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(int){}
        };
        static_assert(!std::is_default_constructible<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(const T&) = default;
            T(T&&) = default;
            T& operator=(const T&) = default;
            T& operator=(T&&) = default;
            ~T() = default;
        };
        static_assert(std::is_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_copy_assignable<emu::expected<T,int>>::value, "");
        static_assert(std::is_move_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_move_assignable<emu::expected<T,int>>::value, "");
        static_assert(std::is_destructible<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(const T&)=delete;
            T(T&&)=delete;
            T& operator=(const T&)=delete;
            T& operator=(T&&)=delete;
        };
        static_assert(!std::is_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(!std::is_copy_assignable<emu::expected<T,int>>::value, "");
        static_assert(!std::is_move_constructible<emu::expected<T,int>>::value, "");
        static_assert(!std::is_move_assignable<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(const T&)=delete;
            T(T&&)=default;
            T& operator=(const T&)=delete;
            T& operator=(T&&)=default;
        };
        static_assert(!std::is_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(!std::is_copy_assignable<emu::expected<T,int>>::value, "");
        static_assert(std::is_move_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_move_assignable<emu::expected<T,int>>::value, "");
    }

    {
        struct T {
            T(const T&)=default;
            T(T&&)=delete;
            T& operator=(const T&)=default;
            T& operator=(T&&)=delete;
        };
        static_assert(std::is_copy_constructible<emu::expected<T,int>>::value, "");
        static_assert(std::is_copy_assignable<emu::expected<T,int>>::value, "");
    }

	{
		emu::expected<int, int> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<int, std::string> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<std::string, int> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<std::string, std::string> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

}


#endif

namespace {
struct takes_init_and_variadic {
  std::vector<int> v;
  std::tuple<int, int> t;
  template <class... Args>
  takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
      : v(l), t(std::forward<Args>(args)...) {}
};
}

#include <type_traits>
#include <vector>
#include <string>

TEST(Constructors, constructors) {
    {
        emu::expected<int,int> e;
        EXPECT_TRUE(e);
        EXPECT_TRUE(e == 0);
    }

    {
        emu::expected<int,int> e = emu::make_unexpected(0);
        EXPECT_TRUE(!e);
        EXPECT_TRUE(e.error() == 0);
    }

    {
        emu::expected<int,int> e (emu::unexpect, 0);
        EXPECT_TRUE(!e);
        EXPECT_TRUE(e.error() == 0);
    }

    {
        emu::expected<int,int> e (emu::in_place, 42);
        EXPECT_TRUE(e);
        EXPECT_TRUE(e == 42);
    }

    {
        emu::expected<std::vector<int>,int> e (emu::in_place, {0,1});
        EXPECT_TRUE(e);
        EXPECT_TRUE((*e)[0] == 0);
        EXPECT_TRUE((*e)[1] == 1);
    }

    {
        emu::expected<std::tuple<int,int>,int> e (emu::in_place, 0, 1);
        EXPECT_TRUE(e);
        EXPECT_TRUE(std::get<0>(*e) == 0);
        EXPECT_TRUE(std::get<1>(*e) == 1);
    }

    {
        emu::expected<takes_init_and_variadic,int> e (emu::in_place, {0,1}, 2, 3);
        EXPECT_TRUE(e);
        EXPECT_TRUE(e->v[0] == 0);
        EXPECT_TRUE(e->v[1] == 1);
        EXPECT_TRUE(std::get<0>(e->t) == 2);
        EXPECT_TRUE(std::get<1>(e->t) == 3);
    }

	{
		emu::expected<int, int> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<int, std::string> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<std::string, int> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

	{
		emu::expected<std::string, std::string> e;
		static_assert(std::is_default_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_constructible<decltype(e)>::value, "");
		static_assert(std::is_move_constructible<decltype(e)>::value, "");
		static_assert(std::is_copy_assignable<decltype(e)>::value, "");
		static_assert(std::is_move_assignable<decltype(e)>::value, "");
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(decltype(e))::value);
		EXPECT_TRUE(!TL_EXPECTED_IS_TRIVIALLY_COPY_ASSIGNABLE(decltype(e))::value);
#	if !defined(TL_EXPECTED_GCC49)
		static_assert(!std::is_trivially_move_constructible<decltype(e)>::value, "");
		static_assert(!std::is_trivially_move_assignable<decltype(e)>::value, "");
#	endif
	}

    {
        emu::expected<void,int> e;
        EXPECT_TRUE(e);
    }

    {
        emu::expected<void,int> e (emu::unexpect, 42);
        EXPECT_TRUE(!e);
        EXPECT_TRUE(e.error() == 42);
    }
}

#include <memory>
#include <vector>
#include <tuple>

TEST(Emplace, emplace) {
    {
        emu::expected<std::unique_ptr<int>,int> e;
        e.emplace(new int{42});
        EXPECT_TRUE(e);
        EXPECT_TRUE(**e == 42);
    }

    {
        emu::expected<std::vector<int>,int> e;
        e.emplace({0,1});
        EXPECT_TRUE(e);
        EXPECT_TRUE((*e)[0] == 0);
        EXPECT_TRUE((*e)[1] == 1);
    }

    {
        emu::expected<std::tuple<int,int>,int> e;
        e.emplace(2,3);
        EXPECT_TRUE(e);
        EXPECT_TRUE(std::get<0>(*e) == 2);
        EXPECT_TRUE(std::get<1>(*e) == 3);
    }

    {
        emu::expected<takes_init_and_variadic,int> e = emu::make_unexpected(0);
        e.emplace({0,1}, 2, 3);
        EXPECT_TRUE(e);
        EXPECT_TRUE(e->v[0] == 0);
        EXPECT_TRUE(e->v[1] == 1);
        EXPECT_TRUE(std::get<0>(e->t) == 2);
        EXPECT_TRUE(std::get<1>(e->t) == 3);
    }
}


#define TOKENPASTE(x, y) x##y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define STATIC_EXPECT_TRUE(e)                                                      \
  constexpr bool TOKENPASTE2(rqure, __LINE__) = e;                             \
  EXPECT_TRUE(e);

TEST(MapExtensions, extensionsMap) {
  auto mul2 = [](int a) { return a * 2; };
  auto ret_void = [](int a) {};

  {
    emu::expected<int, int> e = 21;
    auto ret = e.map(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.map(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).map(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).map(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = e.map(ret_void);
    EXPECT_TRUE(ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.map(ret_void);
    EXPECT_TRUE(ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).map(ret_void);
    EXPECT_TRUE(ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).map(ret_void);
    EXPECT_TRUE(ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map(ret_void);
    EXPECT_TRUE(!ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map(ret_void);
    EXPECT_TRUE(!ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map(ret_void);
    EXPECT_TRUE(!ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map(ret_void);
    EXPECT_TRUE(!ret);
    STATIC_EXPECT_TRUE(
        (std::is_same<decltype(ret), emu::expected<void, int>>::value));
  }


  // mapping functions which return references
  {
    emu::expected<int, int> e(42);
    auto ret = e.map([](int& i) -> int& { return i; });
    EXPECT_TRUE(ret);
    EXPECT_TRUE(ret == 42);
  }
}

TEST(MapErrorExtensions, extensionsMapError) {
  auto mul2 = [](int a) { return a * 2; };
  auto ret_void = [](int a) {};

  {
    emu::expected<int, int> e = 21;
    auto ret = e.map_error(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.map_error(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).map_error(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).map_error(mul2);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map_error(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 42);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map_error(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 42);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map_error(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 42);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map_error(mul2);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 42);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = e.map_error(ret_void);
    EXPECT_TRUE(ret);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.map_error(ret_void);
    EXPECT_TRUE(ret);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).map_error(ret_void);
    EXPECT_TRUE(ret);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).map_error(ret_void);
    EXPECT_TRUE(ret);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map_error(ret_void);
    EXPECT_TRUE(!ret);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.map_error(ret_void);
    EXPECT_TRUE(!ret);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map_error(ret_void);
    EXPECT_TRUE(!ret);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).map_error(ret_void);
    EXPECT_TRUE(!ret);
  }

}

TEST(AndThenExtensions, extensionsAndThen) {
  auto succeed = [](int a) { return emu::expected<int, int>(21 * 2); };
  auto fail = [](int a) { return emu::expected<int, int>(emu::unexpect, 17); };

  {
    emu::expected<int, int> e = 21;
    auto ret = e.and_then(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.and_then(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).and_then(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).and_then(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = e.and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.and_then(succeed);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.and_then(succeed);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).and_then(succeed);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).and_then(succeed);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).and_then(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }
}

TEST(or_else, extensionsOr_else) {
  using eptr = std::unique_ptr<int>;
  auto succeed = [](int a) { return emu::expected<int, int>(21 * 2); };
  auto succeedptr = [](eptr e) { return emu::expected<int,eptr>(21*2);};
  auto fail =    [](int a) { return emu::expected<int,int>(emu::unexpect, 17);};
  auto efail =   [](eptr e) { *e = 17;return emu::expected<int,eptr>(emu::unexpect, std::move(e));};
  auto failptr = [](eptr e) { return emu::expected<int,eptr>(emu::unexpect, std::move(e));};
  auto failvoid = [](int) {};
  auto failvoidptr = [](const eptr&) { /* don't consume */};
  auto consumeptr = [](eptr) {};
  auto make_u_int = [](int n) { return std::unique_ptr<int>(new int(n));};

  {
    emu::expected<int, int> e = 21;
    auto ret = e.or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, eptr> e = 21;
    auto ret = std::move(e).or_else(succeedptr);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = e.or_else(fail);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = e.or_else(fail);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e = 21;
    auto ret = std::move(e).or_else(fail);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(ret == 21);
  }


  {
    emu::expected<int, eptr> e = 21;
    auto ret = std::move(e).or_else(efail);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(ret == 21);
  }

  {
    const emu::expected<int, int> e = 21;
    auto ret = std::move(e).or_else(fail);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, eptr> e(emu::unexpect, make_u_int(21));
    auto ret = std::move(e).or_else(succeedptr);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(succeed);
    EXPECT_TRUE(ret);
    EXPECT_TRUE(*ret == 42);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(failvoid);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = e.or_else(failvoid);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(failvoid);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

  {
    emu::expected<int, eptr> e(emu::unexpect, make_u_int(21));
    auto ret = std::move(e).or_else(failvoidptr);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(*ret.error() == 21);
  }

  {
    emu::expected<int, eptr> e(emu::unexpect, make_u_int(21));
    auto ret = std::move(e).or_else(consumeptr);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == nullptr);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(fail);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 17);
  }

  {
    const emu::expected<int, int> e(emu::unexpect, 21);
    auto ret = std::move(e).or_else(failvoid);
    EXPECT_TRUE(!ret);
    EXPECT_TRUE(ret.error() == 21);
  }

}
struct S {
    int x;
};

struct F {
    int x;
};

TEST(Test14, issue14) {
    auto res = emu::expected<S,F>{emu::unexpect, F{}};

    res.map_error([](F f) {

    });
}

TEST(Test32, issue32) {
    int i = 0;
    emu::expected<void, int> a;
    a.map([&i]{i = 42;});
    EXPECT_TRUE(i == 42);

    auto x = a.map([]{return 42;});
    EXPECT_TRUE(*x == 42);
}


#include <string>

using std::string;

emu::expected<int, string> getInt3(int val) { return val; }

emu::expected<int, string> getInt2(int val) { return val; }

emu::expected<int, string> getInt1() { return getInt2(5).and_then(getInt3); }

TEST(Issue1, issues1) { getInt1(); }

emu::expected<int, int> operation1() { return 42; }

emu::expected<std::string, int> operation2(int const val) { return "Bananas"; }

TEST(Issue17, issues17) {
  auto const intermediate_result = operation1();

  intermediate_result.and_then(operation2);
}

struct a {};
struct b : a {};

auto doit() -> emu::expected<std::unique_ptr<b>, int> {
    return emu::make_unexpected(0);
}

TEST(Issue23, issues23) {
    emu::expected<std::unique_ptr<a>, int> msg = doit();
    EXPECT_TRUE(!msg.has_value());
}

TEST(Issue26, issues26) {
  emu::expected<a, int> exp = emu::expected<b, int>(emu::unexpect, 0);
  EXPECT_TRUE(!exp.has_value());
}

struct foo {
  foo() = default;
  foo(foo &) = delete;
  foo(foo &&){};
};

TEST(Issue29, issues29) {
  std::vector<foo> v;
  v.emplace_back();
  emu::expected<std::vector<foo>, int> ov = std::move(v);
  EXPECT_TRUE(ov->size() == 1);
}

emu::expected<int, std::string> error() {
  return emu::make_unexpected(std::string("error1" ));
}
std::string maperror(std::string s) { return s + "maperror" ; }

TEST(Issue30, issues30) {
  error().map_error(maperror);
}

struct i31{
  int i;
};
TEST(Issue31, issues31) {
    const emu::expected<i31, int> a = i31{42};
    a->i;

    emu::expected< void, std::string > result;
    emu::expected< void, std::string > result2 = result;
    result2 = result;
}

TEST(Issue33, issues33) {
    emu::expected<void, int> res {emu::unexpect, 0};
    EXPECT_TRUE(!res);
    res = res.map_error([](int i) { return 42; });
    EXPECT_TRUE(res.error() == 42);
}


emu::expected<void, std::string> voidWork() { return {}; }
emu::expected<int, std::string> work2() { return 42; }
void errorhandling(std::string){}

TEST(Issue34, issues34) {
  emu::expected <int, std::string> result = voidWork ()
      .and_then (work2);
  result.map_error ([&] (std::string result) {errorhandling (result);});
}

struct non_copyable {
	non_copyable(non_copyable&&) = default;
	non_copyable(non_copyable const&) = delete;
	non_copyable() = default;
};

TEST(Issue42, issues42) {
	emu::expected<non_copyable,int>{}.map([](non_copyable) {});
}

TEST(Issue43, issues43) {
	auto result = emu::expected<void, std::string>{};
	result = emu::make_unexpected(std::string{ "foo" });
}

#if !(__GNUC__ <= 5)
#include <memory>

using MaybeDataPtr = emu::expected<int, std::unique_ptr<int>>;

MaybeDataPtr test(int i) noexcept
{
  return std::move(i);
}

MaybeDataPtr test2(int i) noexcept
{
  return std::move(i);
}

TEST(Issue49, issues49) {
  auto m = test(10)
    .and_then(test2);
}
#endif

//TODO uncomment when fixed.
// emu::expected<int, std::unique_ptr<std::string>> func()
// {
//   return 1;
// }

// TEST(Issue61, issues61) {
//   EXPECT_TRUE(func().value() == 1);
// }


struct move_detector {
  move_detector() = default;
  move_detector(move_detector &&rhs) { rhs.been_moved = true; }
  bool been_moved = false;
};

TEST(Observers, observers) {
    emu::expected<int,int> o1 = 42;
    emu::expected<int,int> o2 {emu::unexpect, 0};
    const emu::expected<int,int> o3 = 42;

  EXPECT_TRUE(*o1 == 42);
  EXPECT_TRUE(*o1 == o1.value());
  EXPECT_TRUE(o2.value_or(42) == 42);
  EXPECT_TRUE(o2.error() == 0);
  EXPECT_TRUE(o3.value() == 42);
  auto success = std::is_same<decltype(o1.value()), int &>::value;
  EXPECT_TRUE(success);
  success = std::is_same<decltype(o3.value()), const int &>::value;
  EXPECT_TRUE(success);
  success = std::is_same<decltype(std::move(o1).value()), int &&>::value;
  EXPECT_TRUE(success);

  #ifndef TL_EXPECTED_NO_CONSTRR
  success = std::is_same<decltype(std::move(o3).value()), const int &&>::value;
  EXPECT_TRUE(success);
  #endif

  emu::expected<move_detector,int> o4{emu::in_place};
  move_detector o5 = std::move(o4).value();
  EXPECT_TRUE(o4->been_moved);
  EXPECT_TRUE(!o5.been_moved);
}


struct no_throw {
  no_throw(std::string i) : i(i) {}
  std::string i;
};
struct canthrow_move {
  canthrow_move(std::string i) : i(i) {}
  canthrow_move(canthrow_move const &) = default;
  canthrow_move(canthrow_move &&other) noexcept(false) : i(other.i) {}
  canthrow_move &operator=(canthrow_move &&) = default;
  std::string i;
};

bool should_throw = false;
struct willthrow_move {
  willthrow_move(std::string i) : i(i) {}
  willthrow_move(willthrow_move const &) = default;
  willthrow_move(willthrow_move &&other) : i(other.i) {
    if (should_throw)
      throw 0;
  }
  willthrow_move &operator=(willthrow_move &&) = default;
  std::string i;
};
static_assert(tl::detail::is_swappable<no_throw>::value, "");

template <class T1, class T2> void swap_test() {
  std::string s1 = "abcdefghijklmnopqrstuvwxyz";
  std::string s2 = "zyxwvutsrqponmlkjihgfedcba";

  emu::expected<T1, T2> a{s1};
  emu::expected<T1, T2> b{s2};
  swap(a, b);
  EXPECT_TRUE(a->i == s2);
  EXPECT_TRUE(b->i == s1);

  a = s1;
  b = emu::unexpected<T2>(s2);
  swap(a, b);
  EXPECT_TRUE(a.error().i == s2);
  EXPECT_TRUE(b->i == s1);

  a = emu::unexpected<T2>(s1);
  b = s2;
  swap(a, b);
  EXPECT_TRUE(a->i == s2);
  EXPECT_TRUE(b.error().i == s1);

  a = emu::unexpected<T2>(s1);
  b = emu::unexpected<T2>(s2);
  swap(a, b);
  EXPECT_TRUE(a.error().i == s2);
  EXPECT_TRUE(b.error().i == s1);

  a = s1;
  b = s2;
  a.swap(b);
  EXPECT_TRUE(a->i == s2);
  EXPECT_TRUE(b->i == s1);

  a = s1;
  b = emu::unexpected<T2>(s2);
  a.swap(b);
  EXPECT_TRUE(a.error().i == s2);
  EXPECT_TRUE(b->i == s1);

  a = emu::unexpected<T2>(s1);
  b = s2;
  a.swap(b);
  EXPECT_TRUE(a->i == s2);
  EXPECT_TRUE(b.error().i == s1);

  a = emu::unexpected<T2>(s1);
  b = emu::unexpected<T2>(s2);
  a.swap(b);
  EXPECT_TRUE(a.error().i == s2);
  EXPECT_TRUE(b.error().i == s1);
}

TEST(Swap, swap) {

  swap_test<no_throw, no_throw>();
  swap_test<no_throw, canthrow_move>();
  swap_test<canthrow_move, no_throw>();

  std::string s1 = "abcdefghijklmnopqrstuvwxyz";
  std::string s2 = "zyxwvutsrqponmlkjihgfedcbaxxx";
  emu::expected<no_throw, willthrow_move> a{s1};
  emu::expected<no_throw, willthrow_move> b{emu::unexpect, s2};
  should_throw = 1;

  #ifdef _MSC_VER
  //this seems to break catch on GCC and Clang
  EXPECT_TRUE_THROWS(swap(a, b));
  #endif

  EXPECT_TRUE(a->i == s1);
  EXPECT_TRUE(b.error().i == s2);
}
