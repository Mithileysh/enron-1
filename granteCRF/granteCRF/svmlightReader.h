

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
    SVMLightReader(const char *infile);

    vector< data_instance > &data_vector();
    vector< data_idx_instance > &data_idx_vector();
    vector<unsigned int> &observations();

    vector<unsigned int> &card();

    void print();


private:

    vector< data_instance > _data_vector;
    vector< data_idx_instance > _data_idx_vector;
    vector<unsigned int> _observations;

    vector<unsigned int> _card;


};

}
