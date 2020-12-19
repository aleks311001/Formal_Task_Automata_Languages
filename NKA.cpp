//
// Created by aleks311001 on 22.09.2020.
//

#include "NKA.h"
#include "Builder.h"

NKA::NKA(): configurations_(),
            alphabet_(),
            transitions_(),
            q0_(),
            acceptingConfigurations_() {}

NKA::NKA(const NKA& other):
        configurations_(other.configurations_),
        alphabet_(other.alphabet_),
        transitions_(other.transitions_),
        q0_(other.q0_),
        acceptingConfigurations_(other.acceptingConfigurations_) {}

NKA::NKA(NKA&& other):
        configurations_(std::move(other.configurations_)),
        alphabet_(std::move(other.alphabet_)),
        transitions_(std::move(other.transitions_)),
        q0_(other.q0_),
        acceptingConfigurations_(std::move(other.acceptingConfigurations_)) {}

NKA& NKA::operator=(const NKA& other) {
    q0_ = other.q0_;
    alphabet_ = other.alphabet_;
    configurations_ = other.configurations_;
    acceptingConfigurations_ = other.acceptingConfigurations_;
    transitions_ = other.transitions_;

    return *this;
}

NKA& NKA::operator=(NKA&& other) {
    q0_ = other.q0_;
    alphabet_ = std::move(other.alphabet_);
    configurations_ = std::move(other.configurations_);
    acceptingConfigurations_ = std::move(other.acceptingConfigurations_);
    transitions_ = std::move(other.transitions_);

    return *this;
}

void NKA::addTransitionWithoutCheck_(ConfigurationType left, const std::string& word, ConfigurationType right) {
    if (!transitions_.contains(left)) {
        transitions_.emplace(left, std::unordered_map<std::string, std::set<ConfigurationType>>());
    }
    if (!transitions_[left].contains(word)) {
        transitions_[left].emplace(word, std::set<ConfigurationType>());
    }
    if (!transitions_[left][word].contains(right)) {
        transitions_[left][word].insert(right);
    }
}
void NKA::addTransition_(ConfigurationType left, const std::string& word, ConfigurationType right) {
    if (!configurations_.contains(left) ||
        !configurations_.contains(right)) {
        throw std::invalid_argument("configurations does not contains added argument of transition");
    }

    for (char symbol: word) {
        if (!alphabet_.contains(symbol)) {
            throw std::invalid_argument("alphabet does not contains symbol of added word of transition");
        }
    }

    addTransitionWithoutCheck_(left, word, right);
}
void NKA::addTransition_(ConfigurationType left, char word, ConfigurationType right) {
    addTransition_(left, std::string(&word, 1), right);
}

NKA::ConfigurationType NKA::addNewConfiguration_(ConfigurationType min) {
    for (ConfigurationType i = min; ; ++i) {
        if (!configurations_.contains(i)) {
            configurations_.insert(i);
            return i;
        }
    }
}

void NKA::replaceMultiSymbolsEdges() {
    for (auto& conf: configurations_) {
        if (!transitions_.contains(conf)) {
            continue;
        }

        std::vector<std::string> badStrings;
        for (auto& pair: transitions_[conf]) {
            if (pair.first.size() < 2) {
                continue;
            }

            badStrings.push_back(pair.first);
            ConfigurationType leftConf = conf;
            ConfigurationType rightConf = 0;
            for (size_t i = 0; i < pair.first.size() - 1; ++i) {
                rightConf = addNewConfiguration_(rightConf);
                addTransition_(leftConf, std::string(&pair.first[i], 1), rightConf);
                leftConf = rightConf;
            }

            for (auto& finalConf: pair.second) {
                addTransition_(leftConf, std::string(&pair.first.back(), 1), finalConf);
            }
        }

        for (auto& bad: badStrings) {
            transitions_[conf].erase(bad);
        }
    }
}

void NKA::findAllWaysOnEpsEdges_(ConfigurationType start,
                                 std::set<ConfigurationType>& wasIn,
                                 std::list<Edge>& endWayConfigurations,
                                 bool& hasAcceptingConf) {
    wasIn.insert(start);
    if (!transitions_.contains(start)) {
        return;
    }

    for (auto& edges: transitions_[start]) {
        if (edges.first == EPS) {
            for (auto& endConf: edges.second) {
                if (!hasAcceptingConf && acceptingConfigurations_.contains(endConf)) {
                    hasAcceptingConf = true;
                }
                if (!wasIn.contains(endConf)) {
                    findAllWaysOnEpsEdges_(endConf, wasIn, endWayConfigurations, hasAcceptingConf);
                }
            }
        } else {
            for (auto& endConf: edges.second) {
                endWayConfigurations.emplace_back(edges.first, endConf);
            }
        }
    }
}

void NKA::addTransitionsInEpsWays_(ConfigurationType start, std::set<ConfigurationType>& wasIn) {
    wasIn.insert(start);

    std::list<Edge> ends;
    std::set<ConfigurationType> wasInForFindEnds;
    bool hasAccept = false;
    findAllWaysOnEpsEdges_(start, wasInForFindEnds, ends, hasAccept);

    for (auto& edge: ends) {
        addTransition_(start, edge.word, edge.final);
        if (!wasIn.contains(edge.final)) {
            addTransitionsInEpsWays_(edge.final, wasIn);
        }
    }

    if (hasAccept) {
        acceptingConfigurations_.insert(wasInForFindEnds.begin(), wasInForFindEnds.end());
    }
}

void NKA::removeEpsilonTransitions_() {
    for (auto& left: configurations_) {
        if (transitions_.contains(left)) {
            transitions_[left].erase(EPS);
        }
    }
}

void NKA::changeEpsTransitions() {
    std::set<ConfigurationType> wasIn;
    addTransitionsInEpsWays_(q0_, wasIn);
    removeEpsilonTransitions_();
}

std::set<NKA::ConfigurationType> NKA::getSetUsefulConfigurations_() {
    std::set<ConfigurationType> newConfigurations;
    std::queue<ConfigurationType> qConfigurations;
    qConfigurations.push(q0_);
    newConfigurations.insert(q0_);

    while (!qConfigurations.empty()) {
        ConfigurationType now = qConfigurations.front();
        qConfigurations.pop();

        if (!transitions_.contains(now)) {
            break;
        }
        for (auto& pair: transitions_[now]) {
            for (auto& conf: pair.second) {
                if (!newConfigurations.contains(conf)) {
                    qConfigurations.push(conf);
                    newConfigurations.insert(conf);
                }
            }
        }
    }

    return newConfigurations;
}

void NKA::delUselessConfigurations() {
    std::set<ConfigurationType> newConfigurations = getSetUsefulConfigurations_();

    for (auto& conf: configurations_) {
        if (!newConfigurations.contains(conf)) {
            if (acceptingConfigurations_.contains(conf)) {
                acceptingConfigurations_.erase(conf);
            }
            if (transitions_.contains(conf)) {
                transitions_.erase(conf);
            }
        }
    }

    configurations_ = std::move(newConfigurations);
}

std::unordered_map<long long, size_t> NKA::getMapConfigurationToNumber_() {
    std::unordered_map<ConfigurationType, size_t> result;

    int i = 1;
    for (auto conf: configurations_) {
        if (q0_ == conf) {
            result.emplace(conf, 0);
        } else {
            result.emplace(conf, i);
            ++i;
        }
    }

    return result;
}

NKA::ConfigurationType NKA::makeConfigurationFromOthers_(const std::set<ConfigurationType>& configurations,
                                                         const std::unordered_map<ConfigurationType, size_t>& indexesConfigs) {
    ConfigurationType result = 0;

    for (auto& conf: configurations) {
        result |= (1ull << indexesConfigs.at(conf));
    }

    return result;
}

bool NKA::checkSetConfigsOnAccepting_(const std::set<ConfigurationType>& configurations) {
    for (auto conf: configurations) {
        if (acceptingConfigurations_.contains(conf)) {
            return true;
        }
    }

    return false;
}

void NKA::checkNumberOfConfigurations_() {
    if (configurations_.size() > std::numeric_limits<ConfigurationType>::digits - 1) {
        delUselessConfigurations();
        if (configurations_.size() > std::numeric_limits<ConfigurationType>::digits - 1) {
            throw std::range_error("number of configurations over max");
        }
    }
}

std::unordered_map<std::string, std::set<long long>>
NKA::findAllTransitionsFromSetConfigurations_(const std::set<ConfigurationType>& configurations) {
    std::unordered_map<std::string, std::set<ConfigurationType>> newTransitions;

    for (auto& conf: configurations) {
        for (auto& pair: transitions_[conf]) {
            if (!newTransitions.contains(pair.first)) {
                newTransitions.emplace(pair);
            } else {
                newTransitions[pair.first].insert(pair.second.begin(), pair.second.end());
            }
        }
    }

    return newTransitions;
}

void NKA::addTransitionsInBuildingDKA_(NKA& newNKA,
                                       const std::unordered_map<ConfigurationType, size_t>& numsConfigurations,
                                       std::queue<relationConfigurations>& configurationsQueue) {
    auto nowConf = configurationsQueue.front();
    configurationsQueue.pop();

    auto newTransition = findAllTransitionsFromSetConfigurations_(nowConf.oldConf);

    for (auto& pair: newTransition) {
        ConfigurationType newConfRight = makeConfigurationFromOthers_(pair.second, numsConfigurations);

        if (!newNKA.configurations_.contains(newConfRight)) {
            newNKA.configurations_.insert(newConfRight);
            configurationsQueue.emplace(newConfRight, pair.second);
        }

        if (!newNKA.acceptingConfigurations_.contains(newConfRight) && checkSetConfigsOnAccepting_(pair.second)) {
            newNKA.acceptingConfigurations_.insert(newConfRight);
        }

        newNKA.addTransition_(nowConf.newConf, pair.first, newConfRight);
    }
}

void NKA::makeExplicitWays() {
    checkNumberOfConfigurations_();

    std::unordered_map<ConfigurationType, size_t> numsConfigurations = getMapConfigurationToNumber_();

    ConfigurationType newQ0 = makeConfigurationFromOthers_({q0_}, numsConfigurations);
    BuilderNKA builderNewNKA;
    builderNewNKA.addConfigurations({newQ0}).setQ0(newQ0).addAlphabet(alphabet_);
    if (acceptingConfigurations_.contains(q0_)) {
        builderNewNKA.addAcceptingConfigurations({newQ0});
    }

    std::queue<relationConfigurations> configurationsQueue;
    configurationsQueue.emplace(newQ0, std::set<ConfigurationType>({q0_}));

    NKA& newNKA = builderNewNKA.getNKA();

    while (!configurationsQueue.empty()) {
        addTransitionsInBuildingDKA_(newNKA, numsConfigurations, configurationsQueue);
    }

    *this = std::move(newNKA);
}

void NKA::makeDKA() {
    replaceMultiSymbolsEdges();
    changeEpsTransitions();
    makeExplicitWays();
}

void NKA::makeOneAcceptingConfiguration_() {
    ConfigurationType newAcc = addNewConfiguration_();

    for (auto& conf: acceptingConfigurations_) {
        addTransition_(conf, EPS, newAcc);
    }

    acceptingConfigurations_.clear();
    acceptingConfigurations_.insert(newAcc);
}

void NKA::makeReverse() {
    if (acceptingConfigurations_.size() > 1) {
        makeOneAcceptingConfiguration_();
    }

    BuilderNKA builderReversedNKA;
    builderReversedNKA.setQ0(*acceptingConfigurations_.begin())
                      .addAlphabet(alphabet_)
                      .addConfigurations(configurations_)
                      .addAcceptingConfigurations({q0_});

    for (auto& transition: transitions_) {
        for (auto& pair: transition.second) {
            std::string word = pair.first;
            std::reverse(word.begin(), word.end());

            for (auto& final_conf: pair.second) {
                builderReversedNKA.addTransition(final_conf, word, transition.first);
            }
        }
    }

    *this = std::move(builderReversedNKA.getNKA());
}

NKA::ConfigurationType NKA::unionNKA(const NKA& other) {
    std::unordered_map<ConfigurationType, ConfigurationType> oldToNewConfigurations;

    ConfigurationType newConf = 0;
    for (auto& conf: other.configurations_) {
        newConf = addNewConfiguration_(newConf);
        oldToNewConfigurations.emplace(conf, newConf);
    }

    for (auto& accConf: other.acceptingConfigurations_) {
        acceptingConfigurations_.insert(oldToNewConfigurations[accConf]);
    }

    for (auto& transition: other.transitions_) {
        for (auto& pair: transition.second) {
            for (auto& final_conf: pair.second) {
                addTransition_(oldToNewConfigurations[transition.first], pair.first, oldToNewConfigurations[final_conf]);
            }
        }
    }

    return oldToNewConfigurations[other.q0_];
}

void NKA::mul(const NKA& other) {
    if (acceptingConfigurations_.size() > 1) {
        makeOneAcceptingConfiguration_();
    }

    auto acceptConfLeft = *acceptingConfigurations_.begin();
    auto startConfRight = unionNKA(other);
    addTransition_(acceptConfLeft, EPS, startConfRight);
    acceptingConfigurations_.erase(acceptConfLeft);
}

void NKA::add(const NKA& other) {
    auto startConfLeft = q0_;
    auto startConfRight = unionNKA(other);

    q0_ = addNewConfiguration_();
    addTransition_(q0_, EPS, startConfLeft);
    addTransition_(q0_, EPS, startConfRight);
}

void NKA::star() {
    makeOneAcceptingConfiguration_();
    auto acceptConf = *acceptingConfigurations_.begin();

    addTransition_(acceptConf, EPS, q0_);
    q0_ = acceptConf;
}

size_t NKA::lenMaxPrefOf(const std::string& word)  {
    makeDKA();

    size_t maxLen = 0;
    ConfigurationType confNow = q0_;
    for (size_t i = 0; i < word.size(); ++i) {
        std::string symbol(&word[i], 1);
        if (!transitions_.contains(confNow) || !transitions_[confNow].contains(symbol)) {
            break;
        }
        confNow = *transitions_[confNow][symbol].begin();

        if (acceptingConfigurations_.contains(confNow)) {
            maxLen = i + 1;
        }
    }

    return maxLen;
}
