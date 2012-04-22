
#include "svmlightReader.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>



using namespace std;
using namespace pyongjoo;
using namespace boost;


SVMLightReader::SVMLightReader(const char *test_file, const char *train_file,
            const char *observed_index_file)
{
    // Read in the training data

    vector< data_instance > data_vector;
    vector< data_idx_instance > data_idx_vector;
    vector<unsigned int> observations;

    unsigned int train_offset = 0;


    // first element is to hold largest recipient label
    // second element is to hold largest word label
    vector<unsigned int> largest_label(2, 0);

    ifstream infile;

    infile.open (test_file);
    string line;

    while (!infile.eof())
    {
        getline(infile, line);

        if (line.empty()) break;

        vector<unsigned int> data_idx;
        vector<double> data_element;

        typedef vector< string > split_vector_type;

        split_vector_type tokened_ins;
        split(tokened_ins, line, is_any_of(" "), token_compress_on);

        // get recipient index (target value of this instance)
        unsigned int recipient_index = (unsigned int) atoi(tokened_ins[0].c_str());

        observations.push_back(recipient_index);

        if (recipient_index > largest_label[0])
            largest_label[0] = recipient_index;


        // get each feature
        for (split_vector_type::iterator it = tokened_ins.begin() + 1;
                it < tokened_ins.end(); it++) {

            string pair = *it;

            split_vector_type tokened_pair;
            split(tokened_pair, pair, is_any_of(":"), token_compress_on);

            unsigned int feature_index = (unsigned int) atoi(tokened_pair[0].c_str());
            double feature_value = (double) atof(tokened_pair[1].c_str());

            data_idx.push_back(feature_index);

            data_element.push_back(feature_value);

            if (feature_index > largest_label[1])
                largest_label[1] = feature_index;
        }

        // to prevent an exception
        if (data_idx.size() == 0) {
            data_idx.push_back(0);
            data_element.push_back(1);
        }

        data_idx_vector.push_back(data_idx);
        data_vector.push_back(data_element);

        train_offset += 1;
    }

    infile.close();


    infile.open (train_file);
    line = "";

    while (!infile.eof())
    {
        getline(infile, line);

        if (line.empty()) break;

        vector<unsigned int> data_idx;
        vector<double> data_element;

        typedef vector< string > split_vector_type;

        split_vector_type tokened_ins;
        split(tokened_ins, line, is_any_of(" "), token_compress_on);

        // get recipient index (target value of this instance)
        unsigned int recipient_index = (unsigned int) atoi(tokened_ins[0].c_str());

        observations.push_back(recipient_index);

        if (recipient_index > largest_label[0])
            largest_label[0] = recipient_index;


        // get each feature
        for (split_vector_type::iterator it = tokened_ins.begin() + 1;
                it < tokened_ins.end(); it++) {

            string pair = *it;

            split_vector_type tokened_pair;
            split(tokened_pair, pair, is_any_of(":"), token_compress_on);

            unsigned int feature_index = (unsigned int) atoi(tokened_pair[0].c_str());
            double feature_value = (double) atof(tokened_pair[1].c_str());

            data_idx.push_back(feature_index);

            data_element.push_back(feature_value);

            if (feature_index > largest_label[1])
                largest_label[1] = feature_index;
        }

        // to prevent an exception
        if (data_idx.size() == 0) {
            data_idx.push_back(0);
            data_element.push_back(1);
        }

        data_idx_vector.push_back(data_idx);
        data_vector.push_back(data_element);
    }

    infile.close();


    _data_vector = data_vector;
    _data_idx_vector = data_idx_vector;
    _observations = observations;



    // Make partially observed data

    infile.open (observed_index_file);
    line = "";

    for (int pi = 0; pi < train_offset; pi++) {
        _var_subset.push_back((unsigned int) pi);
    }

    while (!infile.eof())
    {
        getline(infile, line);

        if (line.empty()) break;

        unsigned int var_label = train_offset + 
            (unsigned int) atoi(line.c_str());

        _var_subset.push_back(var_label);
    }


    vector<unsigned int>::iterator itr;
    for (itr = _var_subset.begin(); itr < _var_subset.end();
            itr++) {
        unsigned int index = *itr;
        _partial_observations.push_back(_observations[index]);
    }

    infile.close();


    // Make hidden var set

    vector<unsigned int> flags;
    flags.resize(data_vector.size(), 1);

    for (int i = 0; i < _var_subset.size(); i++) {
        int index = _var_subset[i];
        flags[index] = 0;
    }

    for (int i = 0; i < flags.size(); i++) {
        if (flags[i] == 1) {
            _var_hiddenset.push_back((unsigned int) i);
        }
    }


    _card.push_back(largest_label[0] + 1);
    _card.push_back(largest_label[1] + 1);
}


vector< data_instance > &SVMLightReader::data_vector()
{
    return _data_vector;
}


vector< data_idx_instance > &SVMLightReader::data_idx_vector()
{
    return _data_idx_vector;
}


vector<unsigned int> &SVMLightReader::observations()
{
    return _observations;
}


vector<unsigned int> &SVMLightReader::partial_observations()
{
    return _partial_observations;
}


vector<unsigned int> &SVMLightReader::var_subset()
{
    return _var_subset;
}


vector<unsigned int> &SVMLightReader::var_hiddenset()
{
    return _var_hiddenset;
}

vector<unsigned int> &SVMLightReader::card()
{
    return _card;
}



void SVMLightReader::print()
{
    for (int i = 0; i < _observations.size(); i++) {
        cout << _observations[i] << ' ';

        data_idx_instance data_idx = _data_idx_vector[i];
        data_instance data = _data_vector[i];

        for (int j = 0; j < data_idx.size(); j++) {
            cout << data_idx[j] << ':';
            cout << data[j] << ' ';
        }

        cout << endl << endl;
    }
}

