

#include "factorBuilder.h"
#include "svmlightReader.h"

#include <FactorType.h>
#include <LinearFactorType.h>
#include <FactorGraph.h>
#include <FactorGraphModel.h>
#include <ParameterEstimationMethod.h>
#include <InferenceMethod.h>
#include <TreeInference.h>
#include <MaximumLikelihood.h>


using namespace std;
using namespace boost;
using namespace Grante;
using namespace pyongjoo;


FactorBuilder::FactorBuilder(unsigned int order) : _chain_order(order) {

}


void FactorBuilder::build (vector< data_instance > &data_vector,
            vector< data_idx_instance > &data_idx_vector,
            vector< unsigned int > &observations,
            vector< unsigned int > &card)
{
    // Read in training data
    //vector<Grante::ParameterEstimationMethod::labeled_instance_type>
    //    _training_data;

    //vector<Grante::InferenceMethod*> inf_method;


    // Create the pairwise factor type
    unsigned int topic_card = card[0];
    unsigned int feature_card = card[1];

    std::vector<unsigned int> card_naive(1, topic_card);   // cardinality of variables
    std::vector<double> w_naive(topic_card * feature_card, 0.0);

    // Since we are using smaller varialbe cardinality, we will need to feed some
    // data to each factor later

    // this factory type is to link between words and ricipient (or topic)
    Grante::FactorType* ft_naive = new Grante::FactorType("naive", card_naive, w_naive);
    _model.AddFactorType(ft_naive);


    // due to the temporal locality, we assume there exists some correaltion
    // between different recipients.

    std::vector<unsigned int> card_topic(2, topic_card);   // cardinality of variables
    std::vector<double> w_topic(2, 0.0);        // degree of freedom is 2

    // name, card, data_size: As for FactorType,
    // A: vector of length prod_card (card[0]*...*card[card.size()-1]), that
    //    contains indices 0,1,... tying elements in the energy table.  The
    //    special element -1 is reserved for fixed zero values.  Let total_a
    //    be the total number of unique indices >=0.  The indices must be
    //    gap-free, that is, they must start with zero and in increments of
    //    one.
    // w: Must be non-empty.  If data_size==0, then w.size()==total_a.  If
    //    data_size>=1, then w.size()==total_a*data_size.


    // create a parameter typing matrix
    vector<int> A;
    
    for (int ai = 0; ai < topic_card * topic_card; ai++) {
        // if it's diagonal
        if (ai / topic_card == ai % topic_card)
            A.push_back(1);
        else
            A.push_back(0);
    }

    Grante::FactorType* ft_topic = new Grante::LinearFactorType("topic", card_topic, w_topic, 0, A);
    _model.AddFactorType(ft_topic);

    // Create a Factor Graph with Topic nodes
    std::vector<unsigned int> ft_naive_varcard(observations.size(), topic_card);
    Grante::FactorGraph* fg = new Grante::FactorGraph(&_model, ft_naive_varcard);

    // Back up the factor graph into member variable
    _fg = fg;


    // Add factors to the Factor Graph

    // add naive bayes factors
    for (unsigned int ci = 0; ci < observations.size(); ++ci) {

        std::vector<unsigned int> factor_varindex(1, ci);

        if (data_vector[ci].size() <= 0) {
            cout << "empty data at: " << ci << endl;
            BOOST_ASSERT(false);
        }

        Grante::Factor* fac = new Grante::Factor(ft_naive, factor_varindex,
                data_vector[ci], data_idx_vector[ci]);
        //Grante::Factor* fac = new Grante::Factor(ft_naive, factor_varindex,
        //        data_vector[ci]);

        fg->AddFactor(fac);
    }


    // add edges between topic (or recipient) nodes
    for (unsigned int ci = 0; ci < observations.size(); ++ci) {
        // we add edges looking forward
        for (unsigned int ii = 1; ii <= _chain_order; ii++) {
            if (ci + ii >= observations.size()) {
                continue;
            }
            else {
                // create a link between node ci and node ci+ii
                vector<unsigned int> factor_varindex;
                factor_varindex.push_back(ci);
                factor_varindex.push_back(ci + ii);

                std::vector<double> data_empty;

                Grante::Factor *fac = new Grante::Factor(ft_topic, factor_varindex, data_empty);

                fg->AddFactor(fac);
            }
        }
    }


    // Add observation
    Grante::ParameterEstimationMethod::labeled_instance_type lit(fg,
            new Grante::FactorGraphObservation(observations));

    _training_data.push_back(lit);



    /*
    // Add inference method: here its a chain, hence use the tree inference
    // method
    inf_method.push_back(new Grante::TreeInference(fg));

    // Train the model using regularized maximum likelihood estimation
    Grante::MaximumLikelihood mle(&model);
    mle.SetupTrainingData(training_data, inf_method);
    mle.Train(1.0e-4);
    std::cout << "Finished training." << std::endl;
    */

}



void FactorBuilder::estimateParameters()
{
    vector<Grante::InferenceMethod*> inf_method;

    // Add inference method: here its a chain, hence use the tree inference
    // method
    inf_method.push_back(new Grante::TreeInference(_fg));

    // Train the model using regularized maximum likelihood estimation
    Grante::MaximumLikelihood mle(&_model);
    mle.SetupTrainingData(_training_data, inf_method);
    mle.Train(1.0e-4);
    std::cout << "Finished training." << std::endl;
}



double FactorBuilder::trainingAccuracy()
{
    // Perform MAP prediction
    Grante::TreeInference tinf(_fg); // fg
    _fg->ForwardMap();       // update energies
    std::vector<unsigned int> map_state;
    tinf.MinimizeEnergy(map_state);


    unsigned int correctGuess = 0;

    const FactorGraphObservation* obs = _training_data[0].second;
    std::vector<unsigned int> observed_states = obs->State();

    BOOST_ASSERT (observed_states.size() == map_state.size());

    for (int i = 0; i < observed_states.size(); i++) {
        if (observed_states[i] == map_state[i]) {
            correctGuess++;
        }
    }

    return (double) correctGuess / (double) observed_states.size();
}

