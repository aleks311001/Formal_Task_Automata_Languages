//
// Created by aleks311001 on 22.09.2020.
//

#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include <queue>
#include <exception>
#include <limits>
#include <fstream>
#include <cmath>

#ifndef FORMAL__NKA_H_
#define FORMAL__NKA_H_

const std::string EPS = ""; ///< Epsilon word

/**
 * @brief NKA class
 * @details This class can make DKA and calculate max len of word's prefix, which in L(NKA/DKA)
 */
class NKA {
private:
    using TransitionsType = std::unordered_map<long long, std::unordered_map<std::string, std::set<long long>>>;

    friend class TestNKA;

    std::set<long long> configurations_;
    std::set<char> alphabet_;
    TransitionsType transitions_;
    long long q0_;
    std::set<long long> acceptingConfigurations_;

    /**
     * Add edge from left to right with value word. But it doesn't check, that start and finish in configurations_ set
     * and word in alphabet^*
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     */
    void addTransitionWithoutCheck_(long long left, const std::string& word, long long right);
    /**
     * @details Find all ends configurations of ways from start, that has epsilon transitions,
     * but last transition is not epsilon. start-EPS->q_1-EPS->q_2-EPS->....-EPS->q_n-a->q in endWayConfigurations
     * @param [in] start starting configuration
     * @param [in, out] wasIn set of configurations, that has way from start
     * @param [out] endWayConfigurations all ends configurations of ways with
     * @param [out] hasAcceptingConf true if exist epsilon way from start to accepting configuration
     */
    void findAllWaysOnEpsEdges_(long long start,
                               std::set<long long>& wasIn,
                               std::list<std::pair<std::string, long long>>& endWayConfigurations,
                               bool& hasAcceptingConf);
    /**
     * Add all edges, that can replace epsilon edges for ways from start.
     * If exist way: start-EPS->q_1-EPS->q_2-EPS->....-EPS->q_n-a->q we add start-a->q
     * Also that function make configurations accepting, if exist epsilon way from it to accepting configuration.
     * @param [in] start starting configuration
     * @param [in, out] wasIn set of configurations, that has way from start
     */
    void addTransitionsInEpsWays_(long long start, std::set<long long>& wasIn);
    /**
     * Remove all epsilon transitions
     */
    void removeEpsilonTransitions_();

    /**
     * Find all configurations, which can way from q0
     * @return set of these configurations
     */
    std::set<long long> getSetUsefulConfigurations_();
    /**
     * For each configuration make a unique number in [0, number of Configurations)
     * @return map: configuration -> number
     */
    std::unordered_map<long long, size_t> getMapConfigurationToNumber_();
    /**
     * Make big configuration for new NKA, as a subset of set all configurations
     * @param [in] configurations subset all configurations
     * @param [in] indexesConfigs map: configuration -> number
     * @return new configuration
     */
    static long long makeConfigurationFromOthers_(const std::set<long long>& configurations,
                                                  const std::unordered_map<long long, size_t>& indexesConfigs);
    /**
     * Check that set of configurations has accepting configuration
     * @param [in] configurations checked configurations
     * @return result of check
     */
    bool checkSetConfigsOnAccepting_(const std::set<long long>& configurations);

    /// Create new configurations and edges from old accepting configurations to this
    void makeOneAcceptingConfiguration_();

    /// Check that number of configurations less log_2(max(long long)), for building DKA
    void checkNumberOfConfigurations_();
    /**
     * Find all edges, which has start configuration in set configurations
     * @param [in] configurations set of start configurations
     * @return map of founded edges
     */
    std::unordered_map<std::string, std::set<long long>>
    findAllTransitionsFromSetConfigurations_(const std::set<long long>& configurations);
    /**
     * Add edge from nowConf in newNKA (for building DKA)
     * @param [int, out] newNKA DKA
     * @param [in] nowConf pair of edge's start configuration in newNKA and set of configurations in this
     * @param [in] numsConfigurations function from configurations to indexes
     * @param [in] configurationsQueue queue for insert next configuration
     */
    void addTransitionsInBuildingDKA_(NKA& newNKA,
                                      const std::pair<long long, std::set<long long>>& nowConf,
                                      const std::unordered_map<long long, size_t>& numsConfigurations,
                                      std::queue<std::pair<long long, std::set<long long>>>& configurationsQueue);

public:
    /**
     * Build NKA from regular expression
     * @param [in] regular expression in polish notation
     * @return made NKA
     */
    friend NKA makeNKA(const std::string& regular);

    NKA(long long q0 = 0,
        const std::set<char>& alphabet = std::set<char>(),
        const std::set<long long>& configurations = std::set<long long>(),
        const std::set<long long>& acceptingConfigurations = std::set<long long>(),
        const TransitionsType& transitions = TransitionsType());
    NKA(long long q0,
        const std::set<char>& alphabet,
        long long numConfigurations,
        const std::set<long long>& acceptingConfigurations = std::set<long long>(),
        const TransitionsType& transitions = TransitionsType());

    NKA(const NKA& other);
    NKA(NKA&& other);
    NKA& operator= (const NKA& other);
    NKA& operator= (NKA&& other);

    /**
     * Add configuration "add"
     * @param [in] add added configuration
     * @throw invalid_argument if added configuration already contains
     */
    void addConfiguration(long long add);
    /**
     * Check, that start and finish in configurations_ set and word in alphabet^*.
     * After it call addTransitionWithoutCheck_
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     * @throw invalid_argument if added configuration or word does not contains in configurations or alphabet
     */
    void addTransition(long long left, const std::string& word, long long right);
    /**
     * Call addTransition(left, string(word), right)
     * @param [in] left start of edge configuration
     * @param [in] word value on edge
     * @param [in] right finish of edge configuration
     * @throw invalid_argument if added configuration or word does not contains in configurations or alphabet
     */
    void addTransition(long long left, char word, long long right);
    /**
     * Make configuration "add" accepting
     * @param add made configuration
     * @throw invalid_argument if added configuration already contains in accepting configuration
     */
    void addAcceptingConfiguration(long long add);

    /**
     * Generate and add unique configuration
     * @param [in] min generated configuration greater then this
     * @return added configuration
     */
    long long addNewConfiguration(long long min = 0);

    /// Replace multi symbols edge on single-symbol edges
    void replaceMultiSymbolsEdges();

    /// Change all epsilon transitions on one-letter.
    void changeEpsTransitions();

    /// Delete All configurations which has not edges
    void delUselessConfigurations();

    /**
     * Transform this in NKA with a single edge type from each state
     */
    void makeExplicitWays();

    /**
     * Make DKA. Call:
     * @code
     *     replaceMultiSymbolsEdges();
     *     changeEpsTransitions();
     *     makeExplicitWays();
     * @endcode
     */
    void makeDKA();

    /**
     * Union two non-intersecting NKAs and return new start configuration second NKA
     * @param [in] other second NKA
     * @return start configuration second NKA
     */
    long long unionNKA(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this) . L(other)
     * @param [in] other second NKA
     */
    void mul(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this) + L(other)
     * @param [in] other second NKA
     */
    void add(const NKA& other);
    /**
     * Make new NKA: L(new NKA) = L(this)*
     */
    void star();

    /**
     * Make reverse NKA: L(new NKA) = L(this)^R
     */
    void makeReverse();
    /**
     * Calculate max len of word's prefix, which in L(this NKA)
     * @param [in] word
     * @return len
     */
    size_t lenMaxPrefOf(const std::string& word);
};

#endif //FORMAL__NKA_H_
