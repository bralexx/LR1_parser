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
    EXPECT_EQ(try_rule_sequence(g.LR_check("dd")), "dd");
    EXPECT_EQ(try_rule_sequence(g.LR_check("ccccccccccdcccccd")), "ccccccccccdcccccd");
    EXPECT_EQ(g.LR_check("d").empty(), true);
}