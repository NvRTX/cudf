#include <gtest/gtest.h>
#include <vector>

#include "nvstrings/NVStrings.h"

#include "./utils.h"

std::vector<const char*> hstrs{ "Examples aBc", "thesé", nullptr, "ARE THE", "tést strings", "" };

TEST(TestCase, ToLower)
{
    NVStrings* strs = NVStrings::create_from_array(hstrs.data(),hstrs.size());
    NVStrings* got = strs->lower();
    const char* expected[] = { "examples abc", "thesé", nullptr, "are the", "tést strings", "" };
    EXPECT_TRUE( verify_strings(got,expected));
    NVStrings::destroy(got);
    NVStrings::destroy(strs);
}

TEST(TestCase, ToUpper)
{
    NVStrings* strs = NVStrings::create_from_array(hstrs.data(),hstrs.size());
    NVStrings* got = strs->upper();
    const char* expected[] = { "EXAMPLES ABC", "THESÉ", nullptr, "ARE THE", "TÉST STRINGS", "" };
    EXPECT_TRUE( verify_strings(got,expected));
    NVStrings::destroy(got);
    NVStrings::destroy(strs);
}

TEST(TestCase, Swapcase)
{
    NVStrings* strs = NVStrings::create_from_array(hstrs.data(),hstrs.size());
    NVStrings* got = strs->swapcase();
    const char* expected[] = { "eXAMPLES AbC", "THESÉ", nullptr, "are the", "TÉST STRINGS", "" };
    EXPECT_TRUE( verify_strings(got,expected));
    NVStrings::destroy(got);
    NVStrings::destroy(strs);
}

TEST(TestCase, Capitalize)
{
    NVStrings* strs = NVStrings::create_from_array(hstrs.data(),hstrs.size());
    NVStrings* got = strs->capitalize();
    const char* expected[] = { "Examples abc", "Thesé", nullptr, "Are the", "Tést strings", "" };
    EXPECT_TRUE( verify_strings(got,expected));
    NVStrings::destroy(got);
    NVStrings::destroy(strs);
}

TEST(TestCase, Title)
{
    NVStrings* strs = NVStrings::create_from_array(hstrs.data(),hstrs.size());
    NVStrings* got = strs->title();
    const char* expected[] = { "Examples Abc", "Thesé", nullptr, "Are The", "Tést Strings", "" };
    EXPECT_TRUE( verify_strings(got,expected));
    NVStrings::destroy(got);
    NVStrings::destroy(strs);
}

int main( int argc, char** argv )
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();
}