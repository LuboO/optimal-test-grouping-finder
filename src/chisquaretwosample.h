#ifndef CHISQUARETWOSAMPLE_H
#define CHISQUARETWOSAMPLE_H

#include <iostream>

#include <vector>
#include <tuple>
#include <stdexcept>
#include <cmath>
#include <boost/math/distributions/chi_squared.hpp>

class ChiSquareTwoSample {
public:
    ChiSquareTwoSample() = delete;

    static std::pair<double, double> calcStatistic(
            const std::vector<double> & obs,
            const std::vector<double> & exp) {
        if(obs.size() != exp.size()) {
            throw std::runtime_error("obs and exp differ in size");
        }
        std::vector<double> obsBins;
        std::vector<double> expBins;
        std::tie(obsBins, expBins) = calcBins(obs, exp);

        using namespace boost::math;
        std::cout << "Degrees of freedom: " << expBins.size() - 1 << std::endl;
        auto stat = calcChiSquareStat(obsBins, expBins);
        auto pValue = 1 - cdf(chi_squared(expBins.size() - 1), stat);

        return {stat, pValue};
    }

private:
    /**
     * @brief calcChiSquareStat Will calculate value of two sample Pearsons chi-squared test statistic.
     * The resulting value is given as X^ = \sum_(i=1)^n ((obs_i - exp_i)^2/exp_i). Each bins vector must
     * have n elements!
     * @param obsBins Observed bin values
     * @param expBins Expected bin values
     * @return Statistic
     */
    static double calcChiSquareStat(
            const std::vector<double> & obsBins,
            const std::vector<double> & expBins) {
        double rval = 0;
        /* Both bin vectors are equally long */
        for(size_t i = 0; i < obsBins.size(); ++i) {
            rval += std::pow((obsBins[i] - expBins[i]), 2) / expBins[i];
        }
        return rval;
    }

    /**
     * @brief calcBins Will create the bins with actual values from which the chi-squared
     * statistic will be calculated. All expectations lesser than MIN_EXPECTATION will
     * be summed into single bin (or added to an existing), returned bins will
     * not contain zeroes.
     * Note: This grouping is better that the one I used in my diploma thesis,
     * as there I forgot to sum expectations very close to zero and therefore
     * I calculated with incomplete expectation. Here, it is fixed, however
     * the results now slightly differ. Be aware of that (not too much).
     * @param obs Observed distribution points
     * @param exp Expected distribution points
     * @return bins on which can be applies chi-squared test
     */
    static std::pair<std::vector<double>, std::vector<double>> calcBins(
            const std::vector<double> & obs,
            const std::vector<double> & exp) {
        /* Minimal expectation value in a single bin */
        const static double MIN_EXPECTATION = 5;

        std::vector<double> obsBins;
        std::vector<double> expBins;
        double obsTail = 0;
        double expTail = 0;

        for(size_t i = 0; i < exp.size(); ++i) {
            if(exp[i] > MIN_EXPECTATION) {
                /* Expectation is greater than min,
                 * just push values into result objects. */
                obsBins.push_back(obs[i]);
                expBins.push_back(exp[i]);
            } else {
                /* Expectation is lesser than 5, add it into tail.
                 * The tail will be appended to both bins at the end of fnc. */
                obsTail += obs[i];
                expTail += exp[i];
            }
        }

        /* If the expectation tail is bigger that minimum, create separate bin.
         * Otherwise just add it to the last bin. */
        if(expTail > MIN_EXPECTATION) {
            obsBins.push_back(obsTail);
            expBins.push_back(expTail);
        } else {
            obsBins.back() += obsTail;
            expBins.back() += expTail;
        }

        return {obsBins, expBins};
    }
};

#endif // CHISQUARETWOSAMPLE_H
