#ifndef LR1_PARSER_TEST_FUNCTIONS_H
#define LR1_PARSER_TEST_FUNCTIONS_H
#include <string>
#include <vector>
#include <list>

std::string list2str(const std::list<char>& list);

std::string try_rule_sequence(const std::vector<std::pair<char, std::string>>& rules);


#endif //LR1_PARSER_TEST_FUNCTIONS_H
