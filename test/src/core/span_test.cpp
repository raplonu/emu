#include <gtest/gtest.h>

#include <gsl/gsl_byte> // for byte
#include <gsl/gsl_util> // for narrow_cast, at
#include <emu/span.h>     // for span, span_iterator, operator==, operator!=

#include <array>       // for array
#include <iostream>    // for ptrdiff_t
#include <iterator>    // for reverse_iterator, operator-, operator==
#include <memory>      // for unique_ptr, shared_ptr, make_unique, allo...
#include <regex>       // for match_results, sub_match, match_results<>...
#include <cstddef>     // for ptrdiff_t
#include <string>      // for string
#include <type_traits> // for integral_constant<>::value, is_default_co...
#include <vector>      // for vector
#include <utility>

// the string_view include and macro are used in the deduction guide verification
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
#ifdef __has_include
#if __has_include(<string_view>)
#include <string_view>
#define HAS_STRING_VIEW
#endif // __has_include(<string_view>)
#endif // __has_include
#endif // (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

using namespace std;
using namespace gsl;

namespace
{
static constexpr char deathstring[] = "Expected Death";

struct BaseClass
{
};
struct DerivedClass : BaseClass
{
};
struct AddressOverloaded
{
#if (__cplusplus > 201402L)
    [[maybe_unused]]
#endif
    AddressOverloaded
    operator&() const
    {
        return {};
    }
};
} // namespace

TEST(span_test, constructors)
{
    emu::span_t<int> s;
    EXPECT_TRUE(s.size() == 0);
    EXPECT_TRUE(s.data() == nullptr);

    emu::span_t<const int> cs;
    EXPECT_TRUE(cs.size() == 0);
    EXPECT_TRUE(cs.data() == nullptr);
}

TEST(span_test, constructors_with_extent)
{
    emu::span_t<int, 0> s;
    EXPECT_TRUE(s.size() == 0);
    EXPECT_TRUE(s.data() == nullptr);

    emu::span_t<const int, 0> cs;
    EXPECT_TRUE(cs.size() == 0);
    EXPECT_TRUE(cs.data() == nullptr);
}

TEST(span_test, constructors_with_bracket_init)
{
    emu::span_t<int> s{};
    EXPECT_TRUE(s.size() == 0);
    EXPECT_TRUE(s.data() == nullptr);

    emu::span_t<const int> cs{};
    EXPECT_TRUE(cs.size() == 0);
    EXPECT_TRUE(cs.data() == nullptr);
}

TEST(span_test, from_nullptr_size_constructor)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. from_nullptr_size_constructor";
        std::abort();
    });
    {
        emu::span_t<int> s{nullptr, narrow_cast<emu::span_t<int>::size_type>(0)};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == nullptr);

        emu::span_t<int> cs{nullptr, narrow_cast<emu::span_t<int>::size_type>(0)};
        EXPECT_TRUE(cs.size() == 0);
        EXPECT_TRUE(cs.data() == nullptr);
    }
    {
        auto workaround_macro = []() {
            const emu::span_t<int, 1> s{nullptr, narrow_cast<emu::span_t<int>::size_type>(0)};
        };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }
    {
        auto workaround_macro = []() { const emu::span_t<int> s{nullptr, 1}; };
        EXPECT_DEATH(workaround_macro(), deathstring);

        auto const_workaround_macro = []() { const emu::span_t<const int> s{nullptr, 1}; };
        EXPECT_DEATH(const_workaround_macro(), deathstring);
    }
    {
        auto workaround_macro = []() { const emu::span_t<int, 0> s{nullptr, 1}; };
        EXPECT_DEATH(workaround_macro(), deathstring);

        auto const_workaround_macro = []() { const emu::span_t<const int, 0> s{nullptr, 1}; };
        EXPECT_DEATH(const_workaround_macro(), deathstring);
    }
    {
        emu::span_t<int*> s{nullptr, narrow_cast<emu::span_t<int>::size_type>(0)};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == nullptr);

        emu::span_t<const int*> cs{nullptr, narrow_cast<emu::span_t<int>::size_type>(0)};
        EXPECT_TRUE(cs.size() == 0);
        EXPECT_TRUE(cs.data() == nullptr);
    }
}

TEST(span_test, from_pointer_length_constructor)
{
    std::set_terminate([] {
        std::cerr << "Expected Death. from_pointer_length_constructor";
        std::abort();
    });
    int arr[4] = {1, 2, 3, 4};

    {
        for (int i = 0; i < 4; ++i)
        {
            {
                emu::span_t<int> s = {&arr[0], narrow_cast<std::size_t>(i)};
                EXPECT_TRUE(s.size() == narrow_cast<std::size_t>(i));
                EXPECT_TRUE(s.data() == &arr[0]);
                EXPECT_TRUE(s.empty() == (i == 0));
                for (int j = 0; j < i; ++j)
                    EXPECT_TRUE(arr[j] == s[narrow_cast<std::size_t>(j)]);
            }
            {
                emu::span_t<int> s = {&arr[i], 4 - narrow_cast<std::size_t>(i)};
                EXPECT_TRUE(s.size() == 4 - narrow_cast<std::size_t>(i));
                EXPECT_TRUE(s.data() == &arr[i]);
                EXPECT_TRUE(s.empty() == ((4 - i) == 0));

                for (int j = 0; j < 4 - i; ++j)
                    EXPECT_TRUE(arr[j + i] == s[narrow_cast<std::size_t>(j)]);
            }
        }
    }

    {
        emu::span_t<int, 2> s{&arr[0], 2};
        EXPECT_TRUE(s.size() == 2);
        EXPECT_TRUE(s.data() == &arr[0]);
        EXPECT_TRUE(s[0] == 1);
        EXPECT_TRUE(s[1] == 2);
    }

    {
        int* p = nullptr;
        emu::span_t<int> s{p, narrow_cast<emu::span_t<int>::size_type>(0)};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == nullptr);
    }

    {
        int* p = nullptr;
        auto workaround_macro = [=]() { const emu::span_t<int> s{p, 2}; };
        EXPECT_DEATH(workaround_macro(), deathstring);
    }
}

TEST(span_test, from_pointer_pointer_construction)
{
    int arr[4] = {1, 2, 3, 4};

    {
        emu::span_t<int> s{&arr[0], &arr[2]};
        EXPECT_TRUE(s.size() == 2);
        EXPECT_TRUE(s.data() == &arr[0]);
        EXPECT_TRUE(s[0] == 1);
        EXPECT_TRUE(s[1] == 2);
    }
    {
        emu::span_t<int, 2> s{&arr[0], &arr[2]};
        EXPECT_TRUE(s.size() == 2);
        EXPECT_TRUE(s.data() == &arr[0]);
        EXPECT_TRUE(s[0] == 1);
        EXPECT_TRUE(s[1] == 2);
    }

    {
        emu::span_t<int> s{&arr[0], &arr[0]};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == &arr[0]);
    }

    {
        emu::span_t<int, 0> s{&arr[0], &arr[0]};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == &arr[0]);
    }

    // this will fail the std::distance() precondition, which asserts on MSVC debug builds
    //{
    //    auto workaround_macro = [&]() { emu::span_t<int> s{&arr[1], &arr[0]}; };
    //    EXPECT_DEATH(workaround_macro(), deathstring);
    //}

    // this will fail the std::distance() precondition, which asserts on MSVC debug builds
    //{
    //    int* p = nullptr;
    //    auto workaround_macro = [&]() { emu::span_t<int> s{&arr[0], p}; };
    //    EXPECT_DEATH(workaround_macro(), deathstring);
    //}

    {
        int* p = nullptr;
        emu::span_t<int> s{p, p};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == nullptr);
    }

    {
        int* p = nullptr;
        emu::span_t<int, 0> s{p, p};
        EXPECT_TRUE(s.size() == 0);
        EXPECT_TRUE(s.data() == nullptr);
    }

    // this will fail the std::distance() precondition, which asserts on MSVC debug builds
    //{
    //    int* p = nullptr;
    //    auto workaround_macro = [&]() { emu::span_t<int> s{&arr[0], p}; };
    //    EXPECT_DEATH(workaround_macro(), deathstring);
    //}
}

TEST(span_test, from_array_constructor)
 {
     int arr[5] = {1, 2, 3, 4, 5};

     {
         const emu::span_t<int> s{arr};
         EXPECT_TRUE(s.size() == 5);
         EXPECT_TRUE(s.data() == &arr[0]);
     }

     {
         const emu::span_t<int, 5> s{arr};
         EXPECT_TRUE(s.size() == 5);
         EXPECT_TRUE(s.data() == &arr[0]);
     }

     int arr2d[2][3] = {1, 2, 3, 4, 5, 6};

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<int, 6> s{arr};
     }

     {
         emu::span_t<int, 0> s{arr};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == &arr[0]);
     }

     {
         emu::span_t<int> s{arr2d};
         EXPECT_TRUE(s.size() == 6);
         EXPECT_TRUE(s.data() == &arr2d[0][0]);
         EXPECT_TRUE(s[0] == 1);
         EXPECT_TRUE(s[5] == 6);
     }

     {
         emu::span_t<int, 0> s{arr2d};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == &arr2d[0][0]);
     }

     {
         emu::span_t<int, 6> s{arr2d};
     }
 #endif
     {
         const emu::span_t<int[3]> s{std::addressof(arr2d[0]), 1};
         EXPECT_TRUE(s.size() == 1);
         EXPECT_TRUE(s.data() == std::addressof(arr2d[0]));
     }

     int arr3d[2][3][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<int> s{arr3d};
         EXPECT_TRUE(s.size() == 12);
         EXPECT_TRUE(s.data() == &arr3d[0][0][0]);
         EXPECT_TRUE(s[0] == 1);
         EXPECT_TRUE(s[11] == 12);
     }

     {
         emu::span_t<int, 0> s{arr3d};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == &arr3d[0][0][0]);
     }

     {
         emu::span_t<int, 11> s{arr3d};
     }

     {
         emu::span_t<int, 12> s{arr3d};
         EXPECT_TRUE(s.size() == 12);
         EXPECT_TRUE(s.data() == &arr3d[0][0][0]);
         EXPECT_TRUE(s[0] == 1);
         EXPECT_TRUE(s[5] == 6);
     }
 #endif
     {
         const emu::span_t<int[3][2]> s{std::addressof(arr3d[0]), 1};
         EXPECT_TRUE(s.size() ==  1);
     }

     AddressOverloaded ao_arr[5] = {};

     {
         const emu::span_t<AddressOverloaded, 5> s{ao_arr};
         EXPECT_TRUE(s.size() == 5);
         EXPECT_TRUE(s.data() == std::addressof(ao_arr[0]));
     }
 }

 TEST(span_test, from_dynamic_array_constructor)
 {
     double(*arr)[3][4] = new double[100][3][4];

     {
         emu::span_t<double> s(&arr[0][0][0], 10);
         EXPECT_TRUE(s.size() == 10);
         EXPECT_TRUE(s.data() == &arr[0][0][0]);
     }

     delete[] arr;
 }

 TEST(span_test, from_std_array_constructor)
 {
     std::array<int, 4> arr = {1, 2, 3, 4};

     {
         emu::span_t<int> s{arr};
         EXPECT_TRUE(s.size() == arr.size());
         EXPECT_TRUE(s.data() == arr.data());

         emu::span_t<const int> cs{arr};
         EXPECT_TRUE(cs.size() == arr.size());
         EXPECT_TRUE(cs.data() == arr.data());
     }

     {
         emu::span_t<int, 4> s{arr};
         EXPECT_TRUE(s.size() == arr.size());
         EXPECT_TRUE(s.data() == arr.data());

         emu::span_t<const int, 4> cs{arr};
         EXPECT_TRUE(cs.size() == arr.size());
         EXPECT_TRUE(cs.data() == arr.data());
     }

     {
         std::array<int, 0> empty_arr{};
         emu::span_t<int> s{empty_arr};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.empty());
     }

     std::array<AddressOverloaded, 4> ao_arr{};

     {
         emu::span_t<AddressOverloaded, 4> fs{ao_arr};
         EXPECT_TRUE(fs.size() == ao_arr.size());
         EXPECT_TRUE(ao_arr.data() == fs.data());
     }

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<int, 2> s{arr};
         EXPECT_TRUE(s.size() == 2);
         EXPECT_TRUE(s.data() == arr.data());

         emu::span_t<const int, 2> cs{arr};
         EXPECT_TRUE(cs.size() == 2);
         EXPECT_TRUE(cs.data() == arr.data());
     }

     {
         emu::span_t<int, 0> s{arr};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == arr.data());

         emu::span_t<const int, 0> cs{arr};
         EXPECT_TRUE(cs.size() == 0);
         EXPECT_TRUE(cs.data() == arr.data());
     }

     {
         emu::span_t<int, 5> s{arr};
     }

     {
         auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
         auto take_a_span = [](emu::span_t<int> s) { static_cast<void>(s); };
         // try to take a temporary std::array
         take_a_span(get_an_array());
     }
 #endif

     {
         auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
         auto take_a_span = [](emu::span_t<const int> s) { static_cast<void>(s); };
         // try to take a temporary std::array
         take_a_span(get_an_array());
     }
 }

 TEST(span_test, from_const_std_array_constructor)
 {
     const std::array<int, 4> arr = {1, 2, 3, 4};

     {
         emu::span_t<const int> s{arr};
         EXPECT_TRUE(s.size() == arr.size());
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 4> s{arr};
         EXPECT_TRUE(s.size() == arr.size());
         EXPECT_TRUE(s.data() == arr.data());
     }

     const std::array<AddressOverloaded, 4> ao_arr{};

     {
         emu::span_t<const AddressOverloaded, 4> s{ao_arr};
         EXPECT_TRUE(s.size() == ao_arr.size());
         EXPECT_TRUE(s.data() == ao_arr.data());
     }

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<const int, 2> s{arr};
         EXPECT_TRUE(s.size() == 2);
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 0> s{arr};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 5> s{arr};
     }
 #endif

     {
         auto get_an_array = []() -> const std::array<int, 4> { return {1, 2, 3, 4}; };
         auto take_a_span = [](emu::span_t<const int> s) { static_cast<void>(s); };
         // try to take a temporary std::array
         take_a_span(get_an_array());
     }
 }

 TEST(span_test, from_std_array_const_constructor)
 {
     std::array<const int, 4> arr = {1, 2, 3, 4};

     {
         emu::span_t<const int> s{arr};
         EXPECT_TRUE(s.size() == arr.size());
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 4> s{arr};
         EXPECT_TRUE(s.size() ==  arr.size());
         EXPECT_TRUE(s.data() == arr.data());
     }

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<const int, 2> s{arr};
         EXPECT_TRUE(s.size() == 2);
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 0> s{arr};
         EXPECT_TRUE(s.size() == 0);
         EXPECT_TRUE(s.data() == arr.data());
     }

     {
         emu::span_t<const int, 5> s{arr};
     }

     {
         emu::span_t<int, 4> s{arr};
     }
 #endif
 }

 TEST(span_test, from_container_constructor)
 {
     std::vector<int> v = {1, 2, 3};
     const std::vector<int> cv = v;

     {
         emu::span_t<int> s{v};
         EXPECT_TRUE(s.size() == v.size());
         EXPECT_TRUE(s.data() == v.data());

         emu::span_t<const int> cs{v};
         EXPECT_TRUE(cs.size() == v.size());
         EXPECT_TRUE(cs.data() == v.data());
     }

     std::string str = "hello";
     const std::string cstr = "hello";

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         emu::span_t<char> s{str};
         EXPECT_TRUE(s.size() == str.size());
         EXPECT_TRUE(s.data() == str.data()));
 #endif
         emu::span_t<const char> cs{str};
         EXPECT_TRUE(cs.size() == str.size());
         EXPECT_TRUE(cs.data() == str.data());
     }

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         emu::span_t<char> s{cstr};
 #endif
         emu::span_t<const char> cs{cstr};
         EXPECT_TRUE(cs.size() == cstr.size());
         EXPECT_TRUE(cs.data() == cstr.data());
     }

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         auto get_temp_vector = []() -> std::vector<int> { return {}; };
         auto use_span = [](emu::span_t<int> s) { static_cast<void>(s); };
         use_span(get_temp_vector());
 #endif
     }

     {
         auto get_temp_vector = []() -> std::vector<int> { return {}; };
         auto use_span = [](emu::span_t<const int> s) { static_cast<void>(s); };
         use_span(get_temp_vector());
     }

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         auto get_temp_string = []() -> std::string { return {}; };
         auto use_span = [](emu::span_t<char> s) { static_cast<void>(s); };
         use_span(get_temp_string());
 #endif
     }

     {
         auto get_temp_string = []() -> std::string { return {}; };
         auto use_span = [](emu::span_t<const char> s) { static_cast<void>(s); };
         use_span(get_temp_string());
     }

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         auto get_temp_vector = []() -> const std::vector<int> { return {}; };
         auto use_span = [](emu::span_t<const char> s) { static_cast<void>(s); };
         use_span(get_temp_vector());
 #endif
     }

     {
         auto get_temp_string = []() -> const std::string { return {}; };
         auto use_span = [](emu::span_t<const char> s) { static_cast<void>(s); };
         use_span(get_temp_string());
     }

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         std::map<int, int> m;
         emu::span_t<int> s{m};
 #endif
     }
 }

 TEST(span_test, from_convertible_span_constructor){{emu::span_t<DerivedClass> avd;
 emu::span_t<const DerivedClass> avcd = avd;
 static_cast<void>(avcd);
 }

 {
 #ifdef CONFIRM_COMPILATION_ERRORS
     emu::span_t<DerivedClass> avd;
     emu::span_t<BaseClass> avb = avd;
     static_cast<void>(avb);
 #endif
 }

 #ifdef CONFIRM_COMPILATION_ERRORS
 {
     emu::span_t<int> s;
     emu::span_t<unsigned int> s2 = s;
     static_cast<void>(s2);
 }

 {
     emu::span_t<int> s;
     emu::span_t<const unsigned int> s2 = s;
     static_cast<void>(s2);
 }

 {
     emu::span_t<int> s;
     emu::span_t<short> s2 = s;
     static_cast<void>(s2);
 }
 #endif
 }

 TEST(span_test, copy_move_and_assignment)
 {
     emu::span_t<int> s1;
     EXPECT_TRUE(s1.empty());

     int arr[] = {3, 4, 5};

     emu::span_t<const int> s2 = arr;
     EXPECT_TRUE(s2.size() ==  3);
     EXPECT_TRUE(s2.data() == &arr[0]);

     s2 = s1;
     EXPECT_TRUE(s2.empty());

     auto get_temp_span = [&]() -> emu::span_t<int> { return {&arr[1], 2}; };
     auto use_span = [&](emu::span_t<const int> s) {
         EXPECT_TRUE(s.size() ==  2);
         EXPECT_TRUE(s.data() == &arr[1]);
     }; use_span(get_temp_span());

     s1 = get_temp_span();
     EXPECT_TRUE(s1.size() ==  2);
     EXPECT_TRUE(s1.data() == &arr[1]);
 }

 TEST(span_test, first)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. first";
        std::abort();
    });
     int arr[5] = {1, 2, 3, 4, 5};

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.first<2>().size() == 2);
         EXPECT_TRUE(av.first(2).size() == 2);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.first<0>().size() == 0);
         EXPECT_TRUE(av.first(0).size() == 0);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.first<5>().size() == 5);
         EXPECT_TRUE(av.first(5).size() == 5);
     }

     {
         emu::span_t<int, 5> av = arr;
 #ifdef CONFIRM_COMPILATION_ERRORS
         EXPECT_TRUE(av.first<6>().size() == 6);
         EXPECT_TRUE(av.first<-1>().size() == -1);
 #endif
         EXPECT_DEATH(av.first(6).size(), deathstring);
     }

     {
         emu::span_t<int> av;
         EXPECT_TRUE(av.first<0>().size() == 0);
         EXPECT_TRUE(av.first(0).size() == 0);
     }
 }

 TEST(span_test, last)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. last";
        std::abort();
    });
     int arr[5] = {1, 2, 3, 4, 5};

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.last<2>().size() == 2);
         EXPECT_TRUE(av.last(2).size() == 2);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.last<0>().size() == 0);
         EXPECT_TRUE(av.last(0).size() == 0);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.last<5>().size() == 5);
         EXPECT_TRUE(av.last(5).size() == 5);
     }

     {
         emu::span_t<int, 5> av = arr;
 #ifdef CONFIRM_COMPILATION_ERRORS
         EXPECT_TRUE(av.last<6>().size() == 6);
 #endif
         EXPECT_DEATH(av.last(6).size(), deathstring);
     }

     {
         emu::span_t<int> av;
         EXPECT_TRUE(av.last<0>().size() == 0);
         EXPECT_TRUE(av.last(0).size() == 0);
     }
 }

 TEST(span_test, subspan)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. subspan";
        std::abort();
    });
     int arr[5] = {1, 2, 3, 4, 5};

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE((av.subspan<2, 2>().size()) == 2);
         EXPECT_TRUE(decltype(av.subspan<2, 2>())::extent == 2);
         EXPECT_TRUE(av.subspan(2, 2).size() == 2);
         EXPECT_TRUE(av.subspan(2, 3).size() == 3);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE((av.subspan<0, 0>().size()) == 0);
         EXPECT_TRUE(decltype(av.subspan<0, 0>())::extent == 0);
         EXPECT_TRUE(av.subspan(0, 0).size() == 0);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE((av.subspan<0, 5>().size()) == 5);
         EXPECT_TRUE(decltype(av.subspan<0, 5>())::extent == 5);
         EXPECT_TRUE(av.subspan(0, 5).size() == 5);

         EXPECT_DEATH(av.subspan(0, 6).size(), deathstring);
         EXPECT_DEATH(av.subspan(1, 5).size(), deathstring);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE((av.subspan<4, 0>().size()) == 0);
         EXPECT_TRUE(decltype(av.subspan<4, 0>())::extent == 0);
         EXPECT_TRUE(av.subspan(4, 0).size() == 0);
         EXPECT_TRUE(av.subspan(5, 0).size() == 0);
         EXPECT_DEATH(av.subspan(6, 0).size(), deathstring);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.subspan<1>().size() == 4);
         EXPECT_TRUE(decltype(av.subspan<1>())::extent == 4);
     }

     {
         emu::span_t<int> av;
         EXPECT_TRUE((av.subspan<0, 0>().size()) == 0);
         EXPECT_TRUE(decltype(av.subspan<0, 0>())::extent == 0);
         EXPECT_TRUE(av.subspan(0, 0).size() == 0);
         EXPECT_DEATH((av.subspan<1, 0>().size()), deathstring);
     }

     {
         emu::span_t<int> av;
         EXPECT_TRUE(av.subspan(0).size() == 0);
         EXPECT_DEATH(av.subspan(1).size(), deathstring);
     }

     {
         emu::span_t<int> av = arr;
         EXPECT_TRUE(av.subspan(0).size() == 5);
         EXPECT_TRUE(av.subspan(1).size() == 4);
         EXPECT_TRUE(av.subspan(4).size() == 1);
         EXPECT_TRUE(av.subspan(5).size() == 0);
         EXPECT_DEATH(av.subspan(6).size(), deathstring);
         const auto av2 = av.subspan(1);
         for (std::size_t i = 0; i < 4; ++i) EXPECT_TRUE(av2[i] == static_cast<int>(i) + 2);
     }

     {
         emu::span_t<int, 5> av = arr;
         EXPECT_TRUE(av.subspan(0).size() == 5);
         EXPECT_TRUE(av.subspan(1).size() == 4);
         EXPECT_TRUE(av.subspan(4).size() == 1);
         EXPECT_TRUE(av.subspan(5).size() == 0);
         EXPECT_DEATH(av.subspan(6).size(), deathstring);
         const auto av2 = av.subspan(1);
         for (std::size_t i = 0; i < 4; ++i) EXPECT_TRUE(av2[i] == static_cast<int>(i) + 2);
     }
 }

 TEST(span_test, iterator_default_init)
 {
     emu::span_t<int>::iterator it1;
     emu::span_t<int>::iterator it2;
     EXPECT_TRUE(it1 == it2);
 }

 TEST(span_test, iterator_comparisons)
 {
     int a[] = {1, 2, 3, 4};
     {
         emu::span_t<int> s = a;
         emu::span_t<int>::iterator it = s.begin();
         auto it2 = it + 1;

         EXPECT_TRUE(it == it);
         EXPECT_TRUE(it == s.begin());
         EXPECT_TRUE(s.begin() == it);

         EXPECT_TRUE(it != it2);
         EXPECT_TRUE(it2 != it);
         EXPECT_TRUE(it != s.end());
         EXPECT_TRUE(it2 != s.end());
         EXPECT_TRUE(s.end() != it);

         EXPECT_TRUE(it < it2);
         EXPECT_TRUE(it <= it2);
         EXPECT_TRUE(it2 <= s.end());
         EXPECT_TRUE(it < s.end());

         EXPECT_TRUE(it2 > it);
         EXPECT_TRUE(it2 >= it);
         EXPECT_TRUE(s.end() > it2);
         EXPECT_TRUE(s.end() >= it2);
     }
 }

 TEST(span_test, incomparable_iterators)
 {
     std::set_terminate([] {
         std::cerr << "Expected Death. incomparable_iterators";
         std::abort();
     });

     int a[] = {1, 2, 3, 4};
     int b[] = {1, 2, 3, 4};
     {
         emu::span_t<int> s = a;
         emu::span_t<int> s2 = b;
#if (__cplusplus > 201402L)
         EXPECT_DEATH([[maybe_unused]] bool _ = (s.begin() == s2.begin()), deathstring);
         EXPECT_DEATH([[maybe_unused]] bool _ = (s.begin() <= s2.begin()), deathstring);
#else
         EXPECT_DEATH(bool _ = (s.begin() == s2.begin()), deathstring);
         EXPECT_DEATH(bool _ = (s.begin() <= s2.begin()), deathstring);
#endif
     }
 }

 TEST(span_test, begin_end)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. begin_end";
        std::abort();
    });
     {
         int a[] = {1, 2, 3, 4};
         emu::span_t<int> s = a;

         emu::span_t<int>::iterator it = s.begin();
         emu::span_t<int>::iterator it2 = std::begin(s);
         EXPECT_TRUE(it == it2);

         it = s.end();
         it2 = std::end(s);
         EXPECT_TRUE(it == it2);
     }

     {
         int a[] = {1, 2, 3, 4};
         emu::span_t<int> s = a;

         auto it = s.begin();
         auto first = it;
         EXPECT_TRUE(it == first);
         EXPECT_TRUE(*it == 1);

         auto beyond = s.end();
         EXPECT_TRUE(it != beyond);
         EXPECT_DEATH(*beyond, deathstring);

         EXPECT_TRUE(beyond - first == 4);
         EXPECT_TRUE(first - first == 0);
         EXPECT_TRUE(beyond - beyond == 0);

         ++it;
         EXPECT_TRUE(it - first == 1);
         EXPECT_TRUE(*it == 2);
         *it = 22;
         EXPECT_TRUE(*it == 22);
         EXPECT_TRUE(beyond - it == 3);

         it = first;
         EXPECT_TRUE(it == first);
         while (it != s.end())
         {
             *it = 5;
             ++it;
         }

         EXPECT_TRUE(it == beyond);
         EXPECT_TRUE(it - beyond == 0);

         for (const auto& n : s) { EXPECT_TRUE(n == 5); }
     }
 }

 TEST(span_test, rbegin_rend)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. rbegin_rend";
        std::abort();
    });
     {
         int a[] = {1, 2, 3, 4};
         emu::span_t<int> s = a;

         auto it = s.rbegin();
         auto first = it;
         EXPECT_TRUE(it == first);
         EXPECT_TRUE(*it == 4);

         auto beyond = s.rend();
         EXPECT_TRUE(it != beyond);
#if (__cplusplus > 201402L)
        EXPECT_DEATH([[maybe_unused]] auto _ = *beyond , deathstring);
#else
        EXPECT_DEATH(auto _ = *beyond , deathstring);
#endif

         EXPECT_TRUE(beyond - first == 4);
         EXPECT_TRUE(first - first == 0);
         EXPECT_TRUE(beyond - beyond == 0);

         ++it;
         EXPECT_TRUE(it - s.rbegin() == 1);
         EXPECT_TRUE(*it == 3);
         *it = 22;
         EXPECT_TRUE(*it == 22);
         EXPECT_TRUE(beyond - it == 3);

         it = first;
         EXPECT_TRUE(it == first);
         while (it != s.rend())
         {
             *it = 5;
             ++it;
         }

         EXPECT_TRUE(it == beyond);
         EXPECT_TRUE(it - beyond == 0);

         for (const auto& n : s) { EXPECT_TRUE(n == 5); }
     }
 }

 TEST(span_test, as_bytes)
 {
     std::set_terminate([] {
         std::cerr << "Expected Death. as_bytes";
         std::abort();
     });

     int a[] = {1, 2, 3, 4};
     {
         const emu::span_t<const int> s = a;
         EXPECT_TRUE(s.size() == 4);
         const emu::span_t<const byte> bs = as_bytes(s);
         EXPECT_TRUE(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
         EXPECT_TRUE(bs.size() == s.size_bytes());
     }

     {
         emu::span_t<int> s;
         const auto bs = as_bytes(s);
         EXPECT_TRUE(bs.size() == s.size());
         EXPECT_TRUE(bs.size() == 0);
         EXPECT_TRUE(bs.size_bytes() == 0);
         EXPECT_TRUE(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
         EXPECT_TRUE(bs.data() == nullptr);
     }

     {
         emu::span_t<int> s = a;
         const auto bs = as_bytes(s);
         EXPECT_TRUE(static_cast<const void*>(bs.data()) == static_cast<const void*>(s.data()));
         EXPECT_TRUE(bs.size() == s.size_bytes());
     }

     int b[5] = {1, 2, 3, 4, 5};
     {
         emu::span_t<int> sp(begin(b), static_cast<size_t>(-2));
         EXPECT_DEATH((void) sp.size_bytes(), deathstring);
     }
 }

 TEST(span_test, as_writable_bytes)
 {
     int a[] = {1, 2, 3, 4};

     {
 #ifdef CONFIRM_COMPILATION_ERRORS
         // you should not be able to get writeable bytes for const objects
         emu::span_t<const int> s = a;
         EXPECT_TRUE(s.size() == 4);
         emu::span_t<const byte> bs = as_writable_bytes(s);
         EXPECT_TRUE(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
         EXPECT_TRUE(bs.size() == s.size_bytes());
 #endif
     }

     {
         emu::span_t<int> s;
         const auto bs = as_writable_bytes(s);
         EXPECT_TRUE(bs.size() == s.size());
         EXPECT_TRUE(bs.size() == 0);
         EXPECT_TRUE(bs.size_bytes() == 0);
         EXPECT_TRUE(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
         EXPECT_TRUE(bs.data() == nullptr);
     }

     {
         emu::span_t<int> s = a;
         const auto bs = as_writable_bytes(s);
         EXPECT_TRUE(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
         EXPECT_TRUE(bs.size() == s.size_bytes());
     }
 }

 TEST(span_test, fixed_size_conversions)
 {
    std::set_terminate([] {
        std::cerr << "Expected Death. fixed_size_conversions";
        std::abort();
    });
     int arr[] = {1, 2, 3, 4};

     // converting to an span from an equal size array is ok
     emu::span_t<int, 4> s4 = arr;
     EXPECT_TRUE(s4.size() == 4);

     // converting to dynamic_range is always ok
     {
         emu::span_t<int> s = s4;
         EXPECT_TRUE(s.size() == s4.size());
         static_cast<void>(s);
     }

 // initialization or assignment to static span that REDUCES size is NOT ok
 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<int, 2> s = arr;
     }
     {
         emu::span_t<int, 2> s2 = s4;
         static_cast<void>(s2);
     }
 #endif

     // even when done dynamically
     {
         /*
         // this now results in a compile-time error, rather than runtime.
         // There is no suitable conversion from dynamic span to fixed span.
         emu::span_t<int> s = arr;
         auto f = [&]() {
             const emu::span_t<int, 2> s2 = s;
             static_cast<void>(s2);
         };
         EXPECT_DEATH(f(), deathstring);
         */
     }

     // but doing so explicitly is ok

     // you can convert statically
     {
         const emu::span_t<int, 2> s2{&arr[0], 2};
         static_cast<void>(s2);
     }
     {
         const emu::span_t<int, 1> s1 = s4.first<1>();
         static_cast<void>(s1);
     }

    /*
     // this is not a legal operation in std::span, so we are no longer supporting it
     // conversion from emu::span_t<int, 4> to emu::span_t<int, dynamic_extent> via call to `first`
     // then convert from emu::span_t<int, dynamic_extent> to emu::span_t<int, 1>
     // The dynamic to fixed extents are not supported in the standard
     // to make this work, emu::span_t<int, 1> would need to be emu::span_t<int>.
     {

         // NB: implicit conversion to emu::span_t<int,1> from emu::span_t<int>
         emu::span_t<int, 1> s1 = s4.first(1);
         static_cast<void>(s1);
     }
     */

     // initialization or assignment to static span that requires size INCREASE is not ok.
     int arr2[2] = {1, 2};

 #ifdef CONFIRM_COMPILATION_ERRORS
     {
         emu::span_t<int, 4> s3 = arr2;
     }
     {
         emu::span_t<int, 2> s2 = arr2;
         emu::span_t<int, 4> s4a = s2;
     }
 #endif
     {
         auto f = [&]() {
             const emu::span_t<int, 4> _s4{arr2, 2};
             static_cast<void>(_s4);
         };
         EXPECT_DEATH(f(), deathstring);
     }

    /*
     // This no longer compiles. There is no suitable conversion from dynamic span to a fixed size span.
     // this should fail - we are trying to assign a small dynamic span to a fixed_size larger one
     emu::span_t<int> av = arr2; auto f = [&]() {
         const emu::span_t<int, 4> _s4 = av;
         static_cast<void>(_s4);
     };
     EXPECT_DEATH(f(), deathstring);
     */
 }

 TEST(span_test, interop_with_std_regex)
 {
     char lat[] = {'1', '2', '3', '4', '5', '6', 'E', 'F', 'G'};
     emu::span_t<char> s = lat;
     const auto f_it = s.begin() + 7;

     std::match_results<emu::span_t<char>::iterator> match;

     std::regex_match(s.begin(), s.end(), match, std::regex(".*"));
     EXPECT_TRUE(match.ready());
     EXPECT_FALSE(match.empty());
     EXPECT_TRUE(match[0].matched);
     EXPECT_TRUE(match[0].first == s.begin());
     EXPECT_TRUE(match[0].second == s.end());

     std::regex_search(s.begin(), s.end(), match, std::regex("F"));
     EXPECT_TRUE(match.ready());
     EXPECT_FALSE(match.empty());
     EXPECT_TRUE(match[0].matched);
     EXPECT_TRUE(match[0].first == f_it);
     EXPECT_TRUE(match[0].second == (f_it + 1));
 }

 TEST(span_test, default_constructible)
 {
     EXPECT_TRUE((std::is_default_constructible<emu::span_t<int>>::value));
     EXPECT_TRUE((std::is_default_constructible<emu::span_t<int, 0>>::value));
     EXPECT_FALSE((std::is_default_constructible<emu::span_t<int, 42>>::value));
 }

 TEST(span_test, std_container_ctad)
 {
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
    // This version of span does not support properly Deduction Guides.
    // this test is just to verify that these compile
    // {
    //     std::vector<int> v{1,2,3,4};
    //     emu::span_t sp{v};
    //     static_assert(std::is_same<decltype(sp), emu::span_t<int>>::value);
    // }
    // {
    //     std::string str{"foo"};
    //     emu::span_t sp{str};
    //     static_assert(std::is_same<decltype(sp), emu::span_t<char>>::value);
    // }
// #ifdef HAS_STRING_VIEW
//     {
//         std::string_view sv{"foo"};
//         emu::span_t sp{sv};
//         static_assert(std::is_same<decltype(sp), emu::span_t<const char>>::value);
//     }
// #endif
#endif
 }

 TEST(span_test, front_back)
 {
    int arr[5] = {1,2,3,4,5};
    emu::span_t<int> s{arr};
    EXPECT_TRUE(s.front() == 1);
    EXPECT_TRUE(s.back() == 5);

    std::set_terminate([] {
        std::cerr << "Expected Death. front_back";
        std::abort();
    });
    emu::span_t<int> s2;
    EXPECT_DEATH(s2.front(), deathstring);
    EXPECT_DEATH(s2.back(), deathstring);
 }
