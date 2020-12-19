//
// Created by aleks311001 on 12.12.2020.
//

#ifndef FORMALPRACTICE1__BUILDER_H_
#define FORMALPRACTICE1__BUILDER_H_

#include "NKA.h"

class BuilderNKA {
private:
    NKA nka_ = {};

public:
    NKA& getNKA() {
        return nka_;
    }

    BuilderNKA& setQ0(const NKA::ConfigurationType& q0) {
        nka_.q0_ = q0;
        return *this;
    }

    BuilderNKA& addAlphabet(const std::set<char>& alphabet) {
        if (nka_.alphabet_.empty()) {
            nka_.alphabet_ = alphabet;
        } else {
            nka_.alphabet_.insert(alphabet.begin(), alphabet.end());
        }
        return *this;
    }

    BuilderNKA& addConfigurations(const std::set<NKA::ConfigurationType>& configurations) {
        if (nka_.configurations_.empty()) {
            nka_.configurations_ = configurations;
        } else {
            nka_.configurations_.insert(configurations.begin(), configurations.end());
        }
        return *this;
    }

    BuilderNKA& addAcceptingConfigurations(const std::set<NKA::ConfigurationType>& acceptingConfigurations) {
        if (nka_.configurations_.empty()) {
            nka_.acceptingConfigurations_ = acceptingConfigurations;
        } else {
            nka_.acceptingConfigurations_.insert(acceptingConfigurations.begin(), acceptingConfigurations.end());
        }
        return *this;
    }

    BuilderNKA& addTransition(NKA::ConfigurationType left, const std::string& word, NKA::ConfigurationType right) {
        nka_.addTransition_(left, word, right);
        return *this;
    }
    BuilderNKA& addTransition(NKA::ConfigurationType left, char word, NKA::ConfigurationType right) {
        nka_.addTransition_(left, word, right);
        return *this;
    }

    /**
     * Make NKA with alphabet = {'a', 'b', 'c'}, q0 = 0, configurations = {0}
     * @return
     */
    BuilderNKA& makeDefaultNKA() {
        addAlphabet({'a', 'b', 'c'});
        addConfigurations({0});
        setQ0(0);
        return *this;
    }

    BuilderNKA& makeNKA(size_t sizeAlphabet, size_t sizeConfigurations) {
        if (sizeAlphabet > 26) {
            throw std::range_error("In this function max alphabet size = 26\n");
        }

        for (char ch = 'a'; ch < 'a' + static_cast<char>(sizeAlphabet); ++ch) {
            addAlphabet({ch});
        }
        for (NKA::ConfigurationType conf = 0; conf < static_cast<NKA::ConfigurationType>(sizeConfigurations); ++conf) {
            addConfigurations({conf});
        }
        setQ0(0);

        return *this;
    }

    BuilderNKA& clear() {
        nka_ = NKA();
        return *this;
    }
};

#endif //FORMALPRACTICE1__BUILDER_H_
