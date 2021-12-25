//
// Created by alex on 25.12.2021.
//

#ifndef LR1_PARSER_GRAMMAR_H
#define LR1_PARSER_GRAMMAR_H

#include <unordered_map>
#include <vector>
#include <set>

#define EPS '$'
#define EPS_POS 1

class Grammar {
    struct Symbol {
        char letter;
        bool term;
    };
    std::vector<Symbol> symbols;
    std::unordered_map<char, int> char2SymbolTable;
    struct Rule {
        int left;
        std::vector<int> right;
    };
    std::vector<std::vector<Rule>> rules;

    struct LR_parser {
        const Grammar& grammar;
        std::vector<std::set<int>> first_len1;
        void init_first_len1();
        std::set<int> first(const std::vector<int>& str);
        struct item {
            std::pair<int, int> rule;
            int symbol;
            int dotPos;

            bool operator<(const item&) const;
        };

        using SetOfItems = std::vector<item>;
        std::vector<SetOfItems> I_list;


    };
    LR_parser lr_parser;
public:
    Grammar(const std::vector<char>& notTerms, const std::vector<char>& terms,
            const std::vector<std::pair<char, std::string>>& rules);
};



#endif //LR1_PARSER_GRAMMAR_H
