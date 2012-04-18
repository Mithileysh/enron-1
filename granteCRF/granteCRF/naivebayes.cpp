
#include "naivebayes.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include <boost/assert.hpp>

using namespace std;
using namespace boost;
using namespace pyongjoo;

NaiveBayesCounter::NaiveBayesCounter (naive_bayes_data &data, var_card &card)
: _ndata(&data), _card(&card), _trained(NULL), _psuedo_count(1) {

    BOOST_ASSERT(card.size() >= 2);

    unsigned int cardinality = card[0] * card[1];
}


NaiveBayesCounter::~NaiveBayesCounter() {

}


void NaiveBayesCounter::train()
{
    unsigned int car1 = (*_card)[0];
    unsigned int car2 = (*_card)[1];

    _trained = new naive_bayes_result(car1 * car2);


    // linealized matrix to hold counts
    vector<unsigned int> counter(car1 * car2, _psuedo_count);

    // increase the counter in the matrix
    for (naive_bayes_data::iterator it = _ndata->begin();
            it < _ndata->end(); it++)
    {
        naive_bayes_instance *ins = *it;

        BOOST_ASSERT( (*ins)[0] < car1);
        BOOST_ASSERT( (*ins)[1] < car2);

        unsigned int lin_index = (*ins)[0] + (*ins)[1] * car1;

        counter[lin_index]++;
    }

    // normalize so as to get conditional probability p(a|b)
    unsigned int offset = 0;
    for (vector<unsigned int>::iterator it = counter.begin();
            it < counter.end(); it += car1)
    {
        // calculate the norm
        unsigned int sum = 0;
        for (int i = 0; i < car1; i++) {
            sum += *(it + i);
        }

        for (int i = 0; i < car1; i++) {
            (*_trained)[offset + i] = (double) *(it + i) / (double) sum;
        }

        offset += car1;
    }
}

NaiveBayesCounter::naive_bayes_result& NaiveBayesCounter::getResults()
{
    // if you get an error at this part, you probably didn't call train()
    // before.
    BOOST_ASSERT(_trained);

    return *_trained;
}

void NaiveBayesCounter::print()
{
    for (naive_bayes_result::iterator it = _trained->begin();
            it < _trained->end(); it++)
    {
        cout << *it << endl;
    }

}

