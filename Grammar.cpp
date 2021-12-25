//
// Created by alex on 25.12.2021.
//

#include "Grammar.h"

Grammar::Grammar(const std::vector<char>& notTerms, const std::vector<char>& terms,
                 const std::vector<std::pair<char, std::string>>& rulesInput) {
    symbols.push_back({-1, false});
    for (auto i:notTerms)
        symbols.push_back({i, false});
    for (auto i:terms)
        symbols.push_back({i, true});
    for (int i = 0; i < symbols.size(); ++i)
        char2SymbolTable[symbols[i].letter] = i;
    rules = std::vector<std::vector<Rule>>(notTerms.size() + 1);
    rules[0].push_back({0, {char2SymbolTable['S']}});
    for (auto i:rulesInput) {
        std::vector<int> right;
        for (auto j:i.second)
            right.push_back(char2SymbolTable[j]);
        rules[char2SymbolTable[i.first]].push_back({char2SymbolTable[i.first], right});
    }
}


bool Grammar::LR_parser::item::operator<(const Grammar::LR_parser::item& t2) const {
    if(rule != t2.rule)
        return rule < t2.rule;
    if(symbol != t2.symbol) return symbol < t2.symbol;
    return dotPos < t2.dotPos;
}

void Grammar::LR_parser::init_first_len1() {
    first_len1 = std::vector<std::set<int>>(grammar.symbols.size());
    for(int symbol = 0; symbol < grammar.symbols.size(); ++symbol){
        if(grammar.symbols[symbol].term)
            first_len1[symbol].insert(symbol);
    }
    bool changed;
    do {
        changed = false;
        for (auto& gr_rules:grammar.rules)
            for (auto& rule:gr_rules) {
                int prev_set_size = first_len1[rule.left].size();
                int right_part_symbol = 0;
                do {
                    for (auto& symbol:first_len1[rule.right[right_part_symbol]])
                        if(symbol != EPS_POS) first_len1[rule.left].insert(symbol);
                } while (first_len1[rule.right[right_part_symbol]].find(EPS_POS) !=
                         first_len1[rule.right[right_part_symbol]].end() &&
                         ++right_part_symbol < rule.right.size());
                if(right_part_symbol == rule.right.size())
                    first_len1[rule.left].insert(EPS_POS);
                if (prev_set_size != first_len1[rule.left].size())
                    changed = true;
            }
    } while (changed);
}

std::set<int> Grammar::LR_parser::first(const std::vector<int>& str) {
    if(str.empty())
        return {EPS_POS};
    std::set<int> answer;
    for(int symbol:str) {
        for(auto c:first_len1[symbol])
            answer.insert(c);
        if(first_len1[symbol].find(EPS_POS) == first_len1[symbol].end())
            break;
    }
    return answer;
}

