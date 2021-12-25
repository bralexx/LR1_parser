//
// Created by alex on 25.12.2021.
//

#ifndef LR1_PARSER_GRAMMAR_H
#define LR1_PARSER_GRAMMAR_H

#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>
#include <map>

#define SPECIAL_CHAR_NUM 2
#define EPS '$'
#define EPS_INDEX 1
#define START_SYMBOL_INDEX 0
#define START_RULE_INDEX_FIRST 0
#define START_RULE_INDEX_SECOND 0

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
        int src_pos;
    };
    std::vector<std::vector<Rule>> rules;
    std::vector<std::pair<char, std::string>> rules_src;

    struct LR_parser {
        bool built = false;
        const Grammar& grammar;
        std::vector<std::set<int>> first_len1;
        void init_first_len1();
        std::set<int> first(const std::vector<int>& str);
        struct Item {
            std::pair<int, int> rule;
            int symbol;
            int dotPos;

            bool operator<(const Item&) const;
        };

        struct AutomatonNode {
            std::vector<int> edge;
            explicit AutomatonNode(size_t alphabet_size);
        };
        std::vector<AutomatonNode> automaton;
        using SetOfItems = std::set<Item>;
        std::map<SetOfItems, int> I_sets;

        SetOfItems closure(const SetOfItems& I);
        SetOfItems go_to(const SetOfItems& I, int X);
        void build_automaton();

        enum Type {
            Shift,
            Reduce,
            Accept,
            Error
        };
        struct Cell {
            Type type= Error;
            int arg1 = -1, arg2 = -1;
        };
        std::vector<std::vector<Cell>> canonicalTable;

        void setLR1CanonicalTable();

        LR_parser() = delete;
        explicit LR_parser(const Grammar& grammar_);
        void build_parser();
    };
    LR_parser lr_parser;
public:
    Grammar(const std::vector<char>& notTerms, const std::vector<char>& terms,
            const std::vector<std::pair<char, std::string>>& rules);

    std::vector<std::pair<char, std::string>> LR_check(const std::string& str);
};



#endif //LR1_PARSER_GRAMMAR_H
