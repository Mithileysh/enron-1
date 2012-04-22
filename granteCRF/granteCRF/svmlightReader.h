

#pragma once

#include <vector>

using namespace std;


namespace pyongjoo {

typedef vector<double> data_instance;
typedef vector<unsigned int> data_idx_instance;


class SVMLightReader {

public:
    /** Read the feature data from the file and set the parameters properly
     *
     *
     */
    SVMLightReader(const char *test_file, const char *train_file,
            const char *observed_index_file, const char *hidden_index_file);

    vector< data_instance > &data_vector();
    vector< data_idx_instance > &data_idx_vector();
    vector<unsigned int> &observations();

    vector<unsigned int> &partial_observations();
    vector<unsigned int> &var_subset();

    vector<unsigned int> &card();

    void print();


private:

    vector< data_instance > _data_vector;
    vector< data_idx_instance > _data_idx_vector;
    vector<unsigned int> _observations;

    vector<unsigned int> _partial_observations;
    vector<unsigned int> _var_subset;

    vector<unsigned int> _card;


};

}
