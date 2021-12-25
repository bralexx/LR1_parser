#include <gtest/gtest.h>
#include "../Grammar.h"
#include <vector>
#include <string>
#include "test_functions.h"

TEST(SimpleTests, test_from_neerc) {
    Grammar g({'S', 'C'}, {'c', 'd'}, {{'S', "CC"},
                                       {'C', "cC"},
                                       {'C', "d"}});
    EXPECT_EQ(try_rule_sequence(g.LR_check("cdd")), "cdd");
    EXPECT_EQ(try_rule_sequence(g.LR_check("cdcccd")), "cdcccd");
    for(int i = 0; i< 100;++i) for(int j = 0; j < 100;++j) {
        std::string s1(i, 'c');
        std::string s2(j, 'c');
        EXPECT_EQ(try_rule_sequence(g.LR_check(s1+'d'+s2+'d')), s1+'d'+s2+'d');
    }
    for(int bitmask = 0; bitmask < 65536; ++bitmask) {
        std::string s;
        for(int bit = 0; bit < 16; ++bit) {
            s.push_back((bitmask>>bit)&1 ? 'd':'c');
        }
        auto x = g.LR_check(s);
        int xx = (bitmask-1)&bitmask;
        int xxx = 1<<15;
        if(((bitmask-1)&bitmask) == (1<<15))
            EXPECT_EQ(try_rule_sequence(g.LR_check(s)),s);
        else {
            EXPECT_TRUE(g.LR_check(s).empty());
            if(!g.LR_check(s).empty()) {
                std::cerr << bitmask;
            }
        }
    }
    EXPECT_EQ(g.LR_check("d").empty(), true);
}