//
// naive bayes counter
//
// Yongjoo Park
//

#pragma once

#include <vector>

using namespace std;

namespace pyongjoo {

    class NaiveBayesCounter {

    public:

        typedef vector<unsigned int> naive_bayes_instance;

        typedef vector<naive_bayes_instance*> naive_bayes_data;

        typedef vector<double> naive_bayes_result;

        typedef vector<unsigned int> var_card;

        /** Constructor for this class
         *
         * Get the data to train on, and cardinality of the data.
         *
         * ndata: a vector of data instances. Each data instance will be a
         * vector of size 2.
         *
         * card: should hold dimension information. For example, if the matrix
         * is n_a x n_b matrix, the card should be [n_a, n_b]. In other words,
         * the length of ndata should be same as the product of two elements in
         * card. Information after second elements are simply ignored.
         *
         * Only supports two dimension.
         *
         * This class does not request the ownership, thus the data should be
         * valid until all getting the results.
         */
        NaiveBayesCounter (naive_bayes_data &ndata, var_card &card);

        ~NaiveBayesCounter();


        /** Simple naive bayes trainer.
         *
         * Set the psuedo count to 1.
         */
        void train();

        /** Get tranined results.
         *
         * train() should be called before getting the result.
         *
         * The result is p(a|b), where the index of 'a' changes faster.
         * For example, p(0|1) = 0.1, p(1|1) = 0.9, p(0|0) = 0.5, p(1|0) = 0.5
         * are represented as [0.5, 0.5, 0.1, 0.9].
         *
         * Caller can access the data by iterating the vector typed as
         * naive_bayes_result.
         *
         * Also, caller is responsible for the ownership of the returned data.
         */
        naive_bayes_result& getResults();

        void print();


    private:

        naive_bayes_data *_ndata;

        var_card *_card;

        naive_bayes_result *_trained;

        unsigned int _psuedo_count;


    };
}




