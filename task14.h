//
// Created by aleks311001 on 04.11.2020.
//

#ifndef FORMALPRACTICE1__TASK14_H_
#define FORMALPRACTICE1__TASK14_H_

#include "NKA.h"
#include "Builder.h"

NKA makeNKA(const std::string& regular) {
    std::vector<NKA> stack;
    bool error = false;
    for (auto symbol: regular) {
        if (symbol == '.') {
            if (stack.size() < 2) {
                error = true;
                break;
            }
            stack[stack.size() - 2].mul(stack.back());
            stack.pop_back();
        } else if (symbol == '+') {
            if (stack.size() < 2) {
                error = true;
                break;
            }
            stack[stack.size() - 2].add(stack.back());
            stack.pop_back();
        } else if (symbol == '*') {
            stack.back().star();
        } else {
            BuilderNKA builder;
            NKA& pushedNKA = builder.makeNKA(3, 2)
                                    .addAcceptingConfigurations({1})
                                    .getNKA();
            stack.push_back(std::move(pushedNKA));
            stack.back().addTransition_(0, symbol, 1);
        }
    }

    if (error || stack.size() != 1) {
        throw std::range_error("Regular isn't correct");
    }

    return stack.back();
}

size_t task14(const std::string& regular, std::string word) {
    NKA nka = makeNKA(regular);
    nka.makeReverse();

    std::reverse(word.begin(), word.end());

    return nka.lenMaxPrefOf(word);
}

#endif //FORMALPRACTICE1__TASK14_H_
