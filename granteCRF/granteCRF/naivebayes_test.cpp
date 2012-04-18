//
// NaiveBayesCounter test file
//
// Yongjoo Park
//

#include "naivebayes.h" 
#include <fstream>
#include <boost/assert.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

using namespace pyongjoo;
using namespace boost;

int main (int argc, const char *argv[])
{
    // Read in a data file
    ifstream infile;
    infile.open (argv[1]);

    BOOST_ASSERT(!infile.fail());


    NaiveBayesCounter::naive_bayes_data data;

    vector<unsigned int> largest_label(2, 0);

    string line;

    while (!infile.eof())
    {
        getline(infile, line);

        if (line.empty()) break;

        NaiveBayesCounter::naive_bayes_instance *ins =
            new NaiveBayesCounter::naive_bayes_instance();

        char_separator<char> sep("\t");
        tokenizer<char_separator<char> > tokens(line, sep);
        BOOST_FOREACH(string t, tokens)
        {
            ins->push_back((unsigned int) atoi(t.c_str()));
        }

        if ((*ins)[0] > largest_label[0])
            largest_label[0] = (*ins)[0];
        if ((*ins)[1] > largest_label[1])
            largest_label[1] = (*ins)[1];

        data.push_back(ins);
    }

    cout << "print input" << endl;

    for (NaiveBayesCounter::naive_bayes_data::iterator it = data.begin();
            it < data.end(); it++) {
        NaiveBayesCounter::naive_bayes_instance *ins = *it;
        cout << (*ins)[0] << ' ' << (*ins)[1] << endl;
    }
    cout << endl;

    NaiveBayesCounter::var_card card;
    card.push_back(largest_label[0] + 1);
    card.push_back(largest_label[1] + 1);

    NaiveBayesCounter c(data, card);

    c.train();

    c.print();

    return 0;
}

