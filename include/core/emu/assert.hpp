#pragma once

#include <emu/config.hpp>
#include <emu/macro.hpp>

#include <boost/assert.hpp>

#define EMU_ASSERT(cond) BOOST_ASSERT(cond)
#define EMU_ASSERT_MSG(cond, msg) BOOST_ASSERT_MSG(cond, msg)

#define EMU_VERIFY(cond) BOOST_VERIFY(cond)
#define EMU_VERIFY_MSG(cond, msg) BOOST_VERIFY_MSG(cond, msg)

#define EMU_ASSERT_IS_VOID BOOST_ASSERT_IS_VOID

#define EMU_ASSERT_ASSUME(cond) \
    do {                        \
        EMU_ASSERT(cond);       \
        EMU_ASSUME(cond);       \
    } while(0)

#define EMU_ASSERT_ASSUME_MSG(cond, msg) \
    do {                                 \
        EMU_ASSERT_MSG(cond, msg);       \
        EMU_ASSUME(cond);                \
    } while(0)

#define EMU_UNREACHABLE __builtin_unreachable()

#define EMU_ASSUME_UNREACHABLE EMU_ASSERT_ASSUME(false)

#define EMU_ASSUME_UNREACHABLE_MSG(msg) EMU_ASSERT_ASSUME_MSG(false, msg)

#define EMU_EXPECTS(cond) Expects(cond)
#define EMU_ENSURES(cond) Ensures(cond)
