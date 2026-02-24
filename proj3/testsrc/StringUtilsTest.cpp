#include <gtest/gtest.h>
#include "StringUtils.h"

TEST(StringUtilsTest, SliceTest){
    EXPECT_EQ(StringUtils::Slice("abcdef", 1, 4), "bcd");
    EXPECT_EQ(StringUtils::Slice("abcdef", 0), "abcdef");          // end==0 => to end
    EXPECT_EQ(StringUtils::Slice("abcdef", 1, -1), "bcde");
    EXPECT_EQ(StringUtils::Slice("abcdef", 0, 1), "a");
    EXPECT_EQ(StringUtils::Slice("abcdef", 1, 1), "");             // empty range
    EXPECT_EQ(StringUtils::Slice("abcdef", 2, 1), "");             // start > end => empty

    EXPECT_EQ(StringUtils::Slice("abcdef", 6, 7), "");             // start==len => empty
    EXPECT_EQ(StringUtils::Slice("abcdef", 0, -7), "");            // end becomes negative then change to 0 => empty

    EXPECT_EQ(StringUtils::Slice("", 1, 4), "");

}

TEST(StringUtilsTest, Capitalize){
    EXPECT_EQ(StringUtils::Capitalize("abc"), "Abc");
    EXPECT_EQ(StringUtils::Capitalize(""), "");
}

TEST(StringUtilsTest, Upper) {
    EXPECT_EQ(StringUtils::Upper("a"), "A");
    EXPECT_EQ(StringUtils::Upper("ab"), "AB");
    EXPECT_EQ(StringUtils::Upper("aBcD"), "ABCD");
    EXPECT_EQ(StringUtils::Upper("ABC"), "ABC");
    EXPECT_EQ(StringUtils::Upper("123!"), "123!");
    EXPECT_EQ(StringUtils::Upper(""), "");
}

TEST(StringUtilsTest, Lower){
    EXPECT_EQ(StringUtils::Lower("A"),"a");
    EXPECT_EQ(StringUtils::Lower("AB"),"ab");
    EXPECT_EQ(StringUtils::Lower("ABCDEF"),"abcdef");
    EXPECT_EQ(StringUtils::Lower("AbCdEF"),"abcdef");
    EXPECT_EQ(StringUtils::Lower("123!"), "123!");
    EXPECT_EQ(StringUtils::Lower(""),"");
}

TEST(StringUtilsTest, LStrip){
    // Basic spaces
    EXPECT_EQ(StringUtils::LStrip("   abc"), "abc");
    EXPECT_EQ(StringUtils::LStrip("abc"), "abc");
    EXPECT_EQ(StringUtils::LStrip("   abc   "), "abc   "); // left only

    // Tabs/newlines mixed (isspace)
    EXPECT_EQ(StringUtils::LStrip("\t\n  abc"), "abc");
    EXPECT_EQ(StringUtils::LStrip("\n\tabc"), "abc");

    // All whitespace -> empty
    EXPECT_EQ(StringUtils::LStrip("     "), "");
    EXPECT_EQ(StringUtils::LStrip("\t\n\r"), "");

    // Empty string
    EXPECT_EQ(StringUtils::LStrip(""), "");

    // Leading whitespace but then punctuation/numbers
    EXPECT_EQ(StringUtils::LStrip("   123"), "123");
    EXPECT_EQ(StringUtils::LStrip("   !@#"), "!@#");

    // Whitespace in the middle should NOT be removed
    EXPECT_EQ(StringUtils::LStrip("  a b c"), "a b c");
}

TEST(StringUtilsTest, RStrip){
    // Basic spaces
    EXPECT_EQ(StringUtils::RStrip("abc   "), "abc");
    EXPECT_EQ(StringUtils::RStrip("abc"), "abc");
    EXPECT_EQ(StringUtils::RStrip("   abc   "), "   abc"); // right only

    // Tabs / newlines / mixed whitespace
    EXPECT_EQ(StringUtils::RStrip("abc\t\n  "), "abc");
    EXPECT_EQ(StringUtils::RStrip("abc\n\t"), "abc");

    // All whitespace -> empty
    EXPECT_EQ(StringUtils::RStrip("     "), "");
    EXPECT_EQ(StringUtils::RStrip("\t\n\r"), "");

    // Empty string
    EXPECT_EQ(StringUtils::RStrip(""), "");

    // Trailing whitespace after numbers / punctuation
    EXPECT_EQ(StringUtils::RStrip("123   "), "123");
    EXPECT_EQ(StringUtils::RStrip("!@#   "), "!@#");

    // Whitespace in the middle should NOT be removed
    EXPECT_EQ(StringUtils::RStrip("a b c   "), "a b c");
}

TEST(StringUtilsTest, Strip){
    EXPECT_EQ(StringUtils::Strip("   abc   "), "abc");
    EXPECT_EQ(StringUtils::Strip("\t\n  abc \r\n"), "abc");
    EXPECT_EQ(StringUtils::Strip("abc"), "abc");
    EXPECT_EQ(StringUtils::Strip(""), "");
    EXPECT_EQ(StringUtils::Strip("     "), "");
    EXPECT_EQ(StringUtils::Strip("  a b c  "), "a b c"); // keep inner spaces
}

TEST(StringUtilsTest, Center){
    // width <= len -> unchanged
    EXPECT_EQ(StringUtils::Center("abc", 0), "abc");
    EXPECT_EQ(StringUtils::Center("abc", 2), "abc");
    EXPECT_EQ(StringUtils::Center("abc", 3), "abc");

    // padding with spaces (extra space goes to the right like Python)
    EXPECT_EQ(StringUtils::Center("abc", 4), "abc ");
    EXPECT_EQ(StringUtils::Center("abc", 5), " abc ");
    EXPECT_EQ(StringUtils::Center("abc", 6), " abc  ");

    // custom fill
    EXPECT_EQ(StringUtils::Center("abc", 5, '-'), "-abc-");
    EXPECT_EQ(StringUtils::Center("abc", 6, '-'), "-abc--");

    // empty string
    EXPECT_EQ(StringUtils::Center("", 4), "    ");
    EXPECT_EQ(StringUtils::Center("", 3, '*'), "***");
}

TEST(StringUtilsTest, LJust){
    EXPECT_EQ(StringUtils::LJust("abc", 0), "abc");
    EXPECT_EQ(StringUtils::LJust("abc", 2), "abc");
    EXPECT_EQ(StringUtils::LJust("abc", 3), "abc");
    EXPECT_EQ(StringUtils::LJust("abc", 5), "abc  ");

    EXPECT_EQ(StringUtils::LJust("abc", 5, '.'), "abc..");
    EXPECT_EQ(StringUtils::LJust("", 3), "   ");
    EXPECT_EQ(StringUtils::LJust("", 3, '_'), "___");
}

TEST(StringUtilsTest, RJust){
    EXPECT_EQ(StringUtils::RJust("abc", 0), "abc");
    EXPECT_EQ(StringUtils::RJust("abc", 2), "abc");
    EXPECT_EQ(StringUtils::RJust("abc", 3), "abc");
    EXPECT_EQ(StringUtils::RJust("abc", 5), "  abc");

    EXPECT_EQ(StringUtils::RJust("abc", 5, '.'), "..abc");
    EXPECT_EQ(StringUtils::RJust("", 3), "   ");
    EXPECT_EQ(StringUtils::RJust("", 3, '_'), "___");
}

TEST(StringUtilsTest, Replace){
    EXPECT_EQ(StringUtils::Replace("abcabc", "ab", "X"), "XcXc");
    EXPECT_EQ(StringUtils::Replace("aaaa", "aa", "b"), "bb");         // non-overlapping typical behavior
    EXPECT_EQ(StringUtils::Replace("hello", "z", "X"), "hello");      // not found
    EXPECT_EQ(StringUtils::Replace("", "a", "b"), "");                // empty input
    EXPECT_EQ(StringUtils::Replace("abc", "abc", "X"), "X");          // whole string
    EXPECT_EQ(StringUtils::Replace("ababab", "aba", "X"), "Xbab");    // overlap-sensitive case (common implementation result)
}

TEST(StringUtilsTest, Split){
    //split = ""
    using V = std::vector<std::string>;
    EXPECT_EQ(StringUtils::Split("a b c"), V({"a","b","c"}));
    EXPECT_EQ(StringUtils::Split("a  b   c"), V({"a","b","c"}));
    EXPECT_EQ(StringUtils::Split(" a\tb\nc "), V({"a","b","c"}));
    EXPECT_EQ(StringUtils::Split("a b\t c\nd"), V({"a","b","c","d"}));

    EXPECT_EQ(StringUtils::Split("   "), V({}));
    EXPECT_EQ(StringUtils::Split(""), V({}));
    EXPECT_EQ(StringUtils::Split("\t\n"), V({}));

    //split != ""
    EXPECT_EQ(StringUtils::Split("a,b,,c", ","), V({"a","b","","c"}));
    EXPECT_EQ(StringUtils::Split(",a,", ","), V({"","a",""}));
    EXPECT_EQ(StringUtils::Split("", ","), V({""}));

    //multiple char seperator
    EXPECT_EQ(StringUtils::Split("one::two::three", "::"), V({"one", "two", "three"}));
    EXPECT_EQ(StringUtils::Split("a::::b", "::"),V({"a", "", "b"}));

    //seperator not found
    EXPECT_EQ(StringUtils::Split("abcdef", "::"),V({"abcdef"}));

    //seperator = input string
    EXPECT_EQ(StringUtils::Split("abc", "abc"),V({"", ""}));

    //empty input string
    EXPECT_EQ(StringUtils::Split("", "::"), V({""}));

    //overlap seperator
    EXPECT_EQ(StringUtils::Split("ababa", "aba"),V({"", "ba"}));
}


TEST(StringUtilsTest, Join){
    EXPECT_EQ(StringUtils::Join(",", std::vector<std::string>{"a","b","c"}), "a,b,c");
    EXPECT_EQ(StringUtils::Join("-", std::vector<std::string>{"a"}), "a");
    EXPECT_EQ(StringUtils::Join(",", std::vector<std::string>{}), "");
    EXPECT_EQ(StringUtils::Join("", std::vector<std::string>{"a","b"}), "ab");
    EXPECT_EQ(StringUtils::Join("::", std::vector<std::string>{"","b",""}), "::b::");
}

TEST(StringUtilsTest, ExpandTabs){
    EXPECT_EQ(StringUtils::ExpandTabs("\t"), "    ");
    EXPECT_EQ(StringUtils::ExpandTabs("a\t"), "a   ");
    EXPECT_EQ(StringUtils::ExpandTabs("ab\t"), "ab  ");
    EXPECT_EQ(StringUtils::ExpandTabs("abc\t"), "abc ");
    EXPECT_EQ(StringUtils::ExpandTabs("abcd\t"), "abcd    ");

    EXPECT_EQ(StringUtils::ExpandTabs("a\tb\t"), "a   b   "); //multitab
    EXPECT_EQ(StringUtils::ExpandTabs("a\t\nb\t"), "a   \nb   "); //multiline
}

TEST(StringUtilsTest, EditDistance){
    EXPECT_EQ(StringUtils::EditDistance("", ""), 0);
    EXPECT_EQ(StringUtils::EditDistance("abc", ""), 3);
    EXPECT_EQ(StringUtils::EditDistance("", "abc"), 3);

    EXPECT_EQ(StringUtils::EditDistance("ECS050", "ECS122A"), 4);
    EXPECT_EQ(StringUtils::EditDistance("abcd", "bcda"), 2);

    EXPECT_EQ(StringUtils::EditDistance("ABC", "abc", false), 3);
    EXPECT_EQ(StringUtils::EditDistance("ABC", "abc", true), 0);
}
