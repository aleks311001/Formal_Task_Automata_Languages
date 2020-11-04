//
// Created by aleks311001 on 22.09.2020.
//

#include "NKA.h"

NKA::NKA(long long q0,
         const std::set<char>& alphabet,
         const std::set<long long>& configurations,
         const std::set<long long>& acceptingConfigurations,
         const TransitionsType& transitions):
        configurations_(configurations),
        alphabet_(alphabet),
        transitions_(transitions),
        q0_(q0),
        acceptingConfigurations_(acceptingConfigurations) {}

NKA::NKA(long long q0,
         const std::set<char>& alphabet,
         long long numConfigurations,
         const std::set<long long int>& acceptingConfigurations,
         const NKA::TransitionsType& transitions):
         NKA(q0, alphabet, std::set<long long>(), acceptingConfigurations, transitions) {
    for (int i = 0; i < numConfigurations; ++i) {
        configurations_.insert(i);
    }
}

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

void NKA::addConfiguration(long long add) {
    if (configurations_.contains(add)) {
        throw std::invalid_argument("configurations already contains added argument");
    }
    configurations_.insert(add);
}
void NKA::addTransitionWithoutCheck_(long long int left, const std::string& word, long long int right) {
    if (!transitions_.contains(left)) {
        transitions_.emplace(left, std::unordered_map<std::string, std::set<long long>>());
    }
    if (!transitions_[left].contains(word)) {
        transitions_[left].emplace(word, std::set<long long>());
    }
    if (!transitions_[left][word].contains(right)) {
        transitions_[left][word].insert(right);
    }
}
void NKA::addTransition(long long left, const std::string& word, long long right) {
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
void NKA::addTransition(long long int left, char word, long long int right) {
    addTransition(left, std::string(&word, 1), right);
}
void NKA::addAcceptingConfiguration(long long add) {
    if (acceptingConfigurations_.contains(add)) {
        throw std::invalid_argument("accepting configurations already contains added argument");
    }
    acceptingConfigurations_.insert(add);
}

long long NKA::addNewConfiguration(long long min) {
    for (long long i = min; ; ++i) {
        if (!configurations_.contains(i)) {
            addConfiguration(i);
            return i;
        }
    }
}

void NKA::replaceMultiSymbolsEdges() {
    for (auto& conf: configurations_) {
        if (transitions_.contains(conf)) {
            std::vector<std::string> badStrings;
            for (auto& pair: transitions_[conf]) {
                if (pair.first.size() > 1) {
                    badStrings.push_back(pair.first);
                    long long leftConf = conf;
                    long long rightConf = 0;
                    for (size_t i = 0; i < pair.first.size() - 1; ++i) {
                        rightConf = addNewConfiguration(rightConf);
                        addTransition(leftConf, std::string(&pair.first[i], 1), rightConf);
                        leftConf = rightConf;
                    }

                    for (auto& finalConf: pair.second) {
                        addTransition(leftConf, std::string(&pair.first.back(), 1), finalConf);
                    }
                }
            }

            for (auto& bad: badStrings) {
                transitions_[conf].erase(bad);
            }
        }
    }
}

void NKA::findAllWaysOnEpsEdges_(long long start,
                                 std::set<long long>& wasIn,
                                 std::list<std::pair<std::string, long long>>& endWayConfigurations,
                                 bool& hasAcceptingConf) {
    wasIn.insert(start);
    if (!transitions_.contains(start)) {
        return;
    }

    for (auto& pair: transitions_[start]) {
        if (pair.first == EPS) {
            for (auto& endConf: pair.second) {
                if (!hasAcceptingConf && acceptingConfigurations_.contains(endConf)) {
                    hasAcceptingConf = true;
                }
                if (!wasIn.contains(endConf)) {
                    findAllWaysOnEpsEdges_(endConf, wasIn, endWayConfigurations, hasAcceptingConf);
                }
            }
        } else {
            for (auto& endConf: pair.second) {
                endWayConfigurations.emplace_back(pair.first, endConf);
            }
        }
    }
}
void NKA::addTransitionsInEpsWays_(long long start, std::set<long long>& wasIn) {
    wasIn.insert(start);

    std::list<std::pair<std::string, long long>> ends;
    std::set<long long> wasInForFindEnds;
    bool hasAccept = false;
    findAllWaysOnEpsEdges_(start, wasInForFindEnds, ends, hasAccept);

    for (auto& pair: ends) {
        addTransition(start, pair.first, pair.second);
        if (!wasIn.contains(pair.second)) {
            addTransitionsInEpsWays_(pair.second, wasIn);
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
    std::set<long long> wasIn;
    addTransitionsInEpsWays_(q0_, wasIn);
    removeEpsilonTransitions_();
}

std::set<long long> NKA::getSetUsefulConfigurations_() {
    std::set<long long> newConfigurations;
    std::queue<long long> qConfigurations;
    qConfigurations.push(q0_);
    newConfigurations.insert(q0_);

    while (!qConfigurations.empty()) {
        long long now = qConfigurations.front();
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
    std::set<long long> newConfigurations = getSetUsefulConfigurations_();

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
    std::unordered_map<long long, size_t> result;

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
long long NKA::makeConfigurationFromOthers_(const std::set<long long int>& configurations,
                                            const std::unordered_map<long long int, size_t>& indexesConfigs) {
    unsigned long long result = 0;

    for (auto& conf: configurations) {
        result |= (1ull << indexesConfigs.at(conf));
    }

    return result;
}
bool NKA::checkSetConfigsOnAccepting_(const std::set<long long int>& configurations) {
    for (auto conf: configurations) {
        if (acceptingConfigurations_.contains(conf)) {
            return true;
        }
    }

    return false;
}

void NKA::checkNumberOfConfigurations_() {
    if (configurations_.size() > std::numeric_limits<long long>::digits - 1) {
        delUselessConfigurations();
        if (configurations_.size() > std::numeric_limits<long long>::digits - 1) {
            throw std::range_error("number of configurations over max");
        }
    }
}
std::unordered_map<std::string, std::set<long long>> NKA::findAllTransitionsFromSetConfigurations_(const std::set<long long>& configurations) {
    std::unordered_map<std::string, std::set<long long>> newTransitions;

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
                                       const std::pair<long long, std::set<long long>>& nowConf,
                                       const std::unordered_map<long long, size_t>& numsConfigurations,
                                       std::queue<std::pair<long long, std::set<long long>>>& configurationsQueue) {
    auto newTransition = findAllTransitionsFromSetConfigurations_(nowConf.second);

    for (auto& pair: newTransition) {
        long long newConfRight = makeConfigurationFromOthers_(pair.second, numsConfigurations);

        if (!newNKA.configurations_.contains(newConfRight)) {
            newNKA.addConfiguration(newConfRight);
            configurationsQueue.emplace(newConfRight, pair.second);
        }

        if (!newNKA.acceptingConfigurations_.contains(newConfRight) && checkSetConfigsOnAccepting_(pair.second)) {
            newNKA.addAcceptingConfiguration(newConfRight);
        }

        newNKA.addTransition(nowConf.first, pair.first, newConfRight);
    }
}
void NKA::makeExplicitWays() {
    checkNumberOfConfigurations_();

    std::unordered_map<long long, size_t> numsConfigurations = getMapConfigurationToNumber_();

    long long newQ0 = makeConfigurationFromOthers_({q0_}, numsConfigurations);
    NKA newNKA (newQ0, alphabet_);
    newNKA.addConfiguration(newQ0);
    if (acceptingConfigurations_.contains(q0_)) {
        newNKA.addAcceptingConfiguration(newQ0);
    }

    std::queue<std::pair<long long, std::set<long long>>> configurationsQueue;
    configurationsQueue.emplace(newQ0, std::set<long long>({q0_}));

    while (!configurationsQueue.empty()) {
        auto nowConf = configurationsQueue.front();
        configurationsQueue.pop();

        addTransitionsInBuildingDKA_(newNKA, nowConf, numsConfigurations, configurationsQueue);
    }

    *this = std::move(newNKA);
}
void NKA::makeDKA() {
    replaceMultiSymbolsEdges();
    changeEpsTransitions();
    makeExplicitWays();
}

void NKA::makeOneAcceptingConfiguration_() {
    long long newAcc = addNewConfiguration();

    for (auto& conf: acceptingConfigurations_) {
        addTransition(conf, EPS, newAcc);
    }

    acceptingConfigurations_.clear();
    acceptingConfigurations_.insert(newAcc);
}

void NKA::makeReverse() {
    if (acceptingConfigurations_.size() > 1) {
        makeOneAcceptingConfiguration_();
    }

    NKA reversed(*acceptingConfigurations_.begin(), alphabet_, configurations_, {q0_});

    for (auto& transition: transitions_) {
        for (auto& pair: transition.second) {
            std::string word = pair.first;
            std::reverse(word.begin(), word.end());

            for (auto& final_conf: pair.second) {
                reversed.addTransition(final_conf, word, transition.first);
            }
        }
    }

    *this = std::move(reversed);
}

long long NKA::unionNKA(const NKA& other) {
    std::unordered_map<long long, long long> oldToNewConfigurations;

    long long newConf = 0;
    for (auto& conf: other.configurations_) {
        newConf = addNewConfiguration(newConf);
        oldToNewConfigurations.emplace(conf, newConf);
    }

    for (auto& accConf: other.acceptingConfigurations_) {
        addAcceptingConfiguration(oldToNewConfigurations[accConf]);
    }

    for (auto& transition: other.transitions_) {
        for (auto& pair: transition.second) {
            for (auto& final_conf: pair.second) {
                addTransition(oldToNewConfigurations[transition.first], pair.first, oldToNewConfigurations[final_conf]);
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
    addTransition(acceptConfLeft, EPS, startConfRight);
    acceptingConfigurations_.erase(acceptConfLeft);
}
void NKA::add(const NKA& other) {
    auto startConfLeft = q0_;
    auto startConfRight = unionNKA(other);

    q0_ = addNewConfiguration();
    addTransition(q0_, EPS, startConfLeft);
    addTransition(q0_, EPS, startConfRight);
}
void NKA::star() {
    makeOneAcceptingConfiguration_();
    auto acceptConf = *acceptingConfigurations_.begin();

    addTransition(acceptConf, EPS, q0_);
    q0_ = acceptConf;
}
size_t NKA::lenMaxPrefOf(const std::string& word)  {
    makeDKA();

    size_t maxLen = 0;
    long long confNow = q0_;
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
