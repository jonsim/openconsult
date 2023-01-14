#include <stack>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/common.h"



TEST(PformatTest, args) {
    std::string s = cmn::pformat("hello %s", "world");
    EXPECT_EQ(s, std::string("hello world"));
}



TEST(FormatBytesTest, args) {
    std::vector<uint8_t> bytes {{1u}, {2u}, {111u}};
    EXPECT_EQ(cmn::format_bytes(bytes), std::string("01026f"));
}



TEST(AdvanceTest, within_bound) {
    const std::string s("hello world");
    auto iter = s.begin();
    auto remaining = cmn::advance(iter, 6, s.end());
    EXPECT_EQ(remaining, 0);
    EXPECT_EQ(*iter, s[6]);
}

TEST(AdvanceTest, exactly_bound) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = cmn::advance(iter, 11, s.end());
    EXPECT_EQ(remaining, 0);
    EXPECT_EQ(iter, s.end());
}

TEST(AdvanceTest, beyond_bound) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = cmn::advance(iter, 16, s.end());
    EXPECT_EQ(remaining, 5);
    EXPECT_EQ(iter, s.end());
}

TEST(AdvanceTest, empty_range) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = cmn::advance(iter, 6, s.begin());
    EXPECT_EQ(remaining, 6);
    EXPECT_EQ(iter, s.begin());
}



TEST(RangeTest, ctor_begin_end) {
    std::string s("hello");
    cmn::range<std::string::iterator> range(s.begin(), s.end());
}

TEST(RangeTest, ctor_begin_end_const) {
    const std::string s("hello");
    cmn::range<std::string::const_iterator> range(s.begin(), s.end());
}

TEST(RangeTest, ctor_container) {
    std::string s("hello");
    cmn::range<std::string::iterator> range(s);
}

TEST(RangeTest, ctor_container_const) {
    const std::string s("hello");
    cmn::range<std::string::const_iterator> range(s);
}

TEST(RangeTest, make_range_begin_end) {
    std::string s("hello");
    cmn::make_range(s.begin(), s.end());
}

TEST(RangeTest, make_range_begin_end_const) {
    const std::string s("hello");
    cmn::make_range(s.begin(), s.end());
}

TEST(RangeTest, make_range_container) {
    std::string s("hello");
    cmn::make_range(s);
}

TEST(RangeTest, make_range_container_const) {
    const std::string s("hello");
    cmn::make_range(s);
}

TEST(RangeTest, compare) {
    std::string s("hello");
    auto range1 = cmn::make_range(s);
    auto range2 = cmn::make_range(s.begin(), s.end());
    auto range3 = cmn::make_range(s.begin(), ++s.begin());
    auto range4 = cmn::make_range(++s.begin(), s.end());

    EXPECT_TRUE(range1 == range2);
    EXPECT_FALSE(range2 == range3);
    EXPECT_FALSE(range2 == range4);

    EXPECT_FALSE(range1 != range2);
    EXPECT_TRUE(range2 != range3);
    EXPECT_TRUE(range2 != range4);
}

TEST(RangeTest, foreach) {
    std::string s("hello");
    auto range = cmn::make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    for (char c : range) {
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}

TEST(RangeTest, foreach_const) {
    const std::string s("hello");
    auto range = cmn::make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    for (char c : range) {
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}
