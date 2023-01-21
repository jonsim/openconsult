#include <stack>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "openconsult/src/common.h"

using namespace cmn;

TEST(PformatTest, args) {
    std::string s = pformat("hello %s", "world");
    EXPECT_EQ(s, std::string("hello world"));
}



TEST(FormatBytesTest, args) {
    std::vector<uint8_t> bytes {{1u}, {2u}, {111u}};
    EXPECT_EQ(format_bytes(bytes), std::string("01026f"));
}



TEST(AdvanceTest, within_bound) {
    const std::string s("hello world");
    auto iter = s.begin();
    auto remaining = advance(iter, 6, s.end());
    EXPECT_EQ(remaining, 0);
    EXPECT_EQ(*iter, s[6]);
}

TEST(AdvanceTest, exactly_bound) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = advance(iter, 11, s.end());
    EXPECT_EQ(remaining, 0);
    EXPECT_EQ(iter, s.end());
}

TEST(AdvanceTest, beyond_bound) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = advance(iter, 16, s.end());
    EXPECT_EQ(remaining, 5);
    EXPECT_EQ(iter, s.end());
}

TEST(AdvanceTest, empty_range) {
    std::string s("hello world");
    auto iter = s.begin();
    auto remaining = advance(iter, 6, s.begin());
    EXPECT_EQ(remaining, 6);
    EXPECT_EQ(iter, s.begin());
}



TEST(RangeTest, ctor_begin_end) {
    std::string s("hello");
    range<std::string::iterator> range(s.begin(), s.end());
}

TEST(RangeTest, ctor_begin_end_const) {
    const std::string s("hello");
    range<std::string::const_iterator> range(s.begin(), s.end());
}

TEST(RangeTest, ctor_container) {
    std::string s("hello");
    range<std::string::iterator> range(s);
}

TEST(RangeTest, ctor_container_const) {
    const std::string s("hello");
    range<std::string::const_iterator> range(s);
}

TEST(RangeTest, make_range_begin_end) {
    std::string s("hello");
    make_range(s.begin(), s.end());
}

TEST(RangeTest, make_range_begin_end_const) {
    const std::string s("hello");
    make_range(s.begin(), s.end());
}

TEST(RangeTest, make_range_container) {
    std::string s("hello");
    make_range(s);
}

TEST(RangeTest, make_range_container_const) {
    const std::string s("hello");
    make_range(s);
}

TEST(RangeTest, compare) {
    std::string s("hello");
    auto range1 = make_range(s);
    auto range2 = make_range(s.begin(), s.end());
    auto range3 = make_range(s.begin(), ++s.begin());
    auto range4 = make_range(++s.begin(), s.end());

    EXPECT_TRUE(range1 == range2);
    EXPECT_FALSE(range2 == range3);
    EXPECT_FALSE(range2 == range4);

    EXPECT_FALSE(range1 != range2);
    EXPECT_TRUE(range2 != range3);
    EXPECT_TRUE(range2 != range4);
}

TEST(RangeTest, empty) {
    std::string s("hello");
    EXPECT_FALSE(make_range(s).empty());
    EXPECT_FALSE(make_range(s.begin(), s.end()).empty());
    EXPECT_FALSE(make_range(s.begin(), ++s.begin()).empty());
    EXPECT_FALSE(make_range(++s.begin(), s.end()).empty());
    EXPECT_TRUE( make_range(s.begin(), s.begin()).empty());
    EXPECT_TRUE( make_range(s.end(), s.end()).empty());
}

TEST(RangeTest, size) {
    std::string s("hello");
    EXPECT_EQ(5, make_range(s).size());
    EXPECT_EQ(5, make_range(s.begin(), s.end()).size());
    EXPECT_EQ(1, make_range(s.begin(), ++s.begin()).size());
    EXPECT_EQ(4, make_range(++s.begin(), s.end()).size());
    EXPECT_EQ(0, make_range(s.begin(), s.begin()).size());
    EXPECT_EQ(0, make_range(s.end(), s.end()).size());
}

TEST(RangeTest, foreach) {
    std::string s("hello");
    auto range = make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    for (char c : range) {
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}

TEST(RangeTest, foreach_const) {
    const std::string s("hello");
    auto range = make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    for (char c : range) {
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}

TEST(RangeTest, dereference) {
    std::string s("hello");
    auto range = make_range(s);
    EXPECT_EQ('h', *range);
    *range = 'j';
    EXPECT_EQ('j', *range);
}

TEST(RangeTest, dereference_const) {
    const std::string s("hello");
    auto range = make_range(s);
    EXPECT_EQ('h', *range);
}

TEST(RangeTest, arrow) {
    struct Char {
        char c;
    };
    std::vector<Char> chars {{'h'}, {'e'}, {'l'}, {'l'}, {'o'}};
    auto range = make_range(chars);
    EXPECT_EQ('h', range->c);
    range->c = 'j';
    EXPECT_EQ('j', range->c);
}

TEST(RangeTest, arrow_const) {
    struct Char {
        char c;
    };
    const std::vector<Char> chars {{'h'}, {'e'}, {'l'}, {'l'}, {'o'}};
    auto range = make_range(chars);
    EXPECT_EQ('h', range->c);
}

TEST(RangeTest, increment) {
    std::string s("hello");
    auto range = make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    while (!range.empty()) {
        char c = *(range++);
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}

TEST(RangeTest, increment_const) {
    const std::string s("hello");
    auto range = make_range(s);
    std::stack<char> stack {{ 'o', 'l', 'l', 'e', 'h' }};
    while (!range.empty()) {
        char c = *(range++);
        ASSERT_EQ(c, stack.top());
        stack.pop();
    }
}
