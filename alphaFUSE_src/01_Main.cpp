///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////                            Feature Selection as a One-Player Game                         //////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    This code is an implementation of the algorithm described in (https://hal.inria.fr/inria-00484049/document)    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The feature selection algorithm is run on the training set "dataset.dat.                                          //
// The reading function can be modified in dataset.cpp                                                               //
// The algorithm is based on bandit reinforcement learning, and is an improved version of UCT                        //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Created : 2018-03-12                                                                                              //
// Updated : 2019-07-26                                                                                              //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

//including functions
#include "mainf.h"
#include "dataset.h"
#include "select_features.h"

//including classes
#include "class_Params.h"   //Contains all the important simulation parameters



int main ( int argc, char *argv[] )
{

    int counter;
    printf("Program Name Is: %s",argv[0]);
    if(argc==1)
    {
        cout << endl << "No Extra Command Line Argument Passed Other Than Program Name" << endl << endl;
    }
    if(argc>=2)
    {
        cout << endl << "Number Of Arguments Passed:" << argc << endl;
        cout << endl << "----Following Are The Command Line Arguments Passed----" << endl;
        for(counter=0;counter<argc;counter++)
            printf("\nargv[%d]: %s",counter,argv[counter]);
    }

    cout << endl << endl << endl;



    int Nrepeat = stoi(argv[7]);
    for (int rseed=0; rseed<Nrepeat; rseed++)
    {
        Find_Features(argv, rseed);
        cout << endl;
    }

    return 0;
}


void Find_Features(char *argv[], int rseed)
{
    //just checking if 32bit or 64bit
    //vector<float> vector_float;
    //cout << endl << "   max_size_float  = " << vector_float.max_size() << " = 2^" << log(vector_float.max_size())/log(2) << endl << endl << endl;
    //cin.get();



        cout << endl;
        cout << " |-----------------------------------------------------------------------------------------------|" << endl;
        cout << " |-------------------------------- FEATURE SELECTION - alphaFUSE --------------------------------|" << endl;
        cout << " |-----------------------------------------------------------------------------------------------|" << endl << endl;




//=====================================================================================================================================//
//================================================== Important simulation parameters ==================================================//
//=====================================================================================================================================//


    double alpha = 1;    //Transposition parameter
	double boot_frac = stod(argv[1]);  //size of the training subset (usefull to avoid overfitting)
    int Nt = stoi(argv[2]);        //Number of iterations of the simulation
    bool MA = 0;             //Many-Armed behavior, put 0 for Discrete and 1 for Continuous (only discrete is currently implemented)
    double q = 0.9;         //Random expansion parameter, used to control the average depth in the random phase, |q|<1, high q -> deep exploration
    int k = stoi(argv[5]);               //Number of nearest neighbors involved in the reward function calculation
        double r2 = 1;           //Ratio of reward training set size / Training set size (not used anymore)
    double ce = 2;           //UCB exploration control parameter (used in both discrete and continuous heuristic)
    double c = 20;           //Continuous heuristic exploration parameter (RAVE score weight)
    double cl = 200;         //l-RAVE/g-RAVE weight
    double b = 0.26;         //Discrete heuristic exploration parameter, |b|<1


    bool Reward_policy = stoi(argv[6]);  //0 for Accuracy (ACC) and 1 for Area Under Curve (AUC)
    bool KNN_policy = 0;     //0 for naive kNN search, 1 for KD-Tree kNN search


    bool with_data = 1;       //1 for reading the training set, 0 for tests with theoretical training set
    bool sub = 0;             //True if we want to reduce the training set size
         int n_sub = 500;     //desired size of the new sub training set
    bool pFS = 0;             //True if we want to perform a previous feature selection
        unordered_set<int> Features = {};
                                      //Pre-selection of features because FUSE calculation may become too heavy when there is too many features
                                      //for example, This feature subset contain the best 150 ranked after their Relief score



//============================================================= Training set ==========================================================//

    cout << endl << "  Reading the dataset ..." << endl << endl;



    vector<vector<float>> L0;

    if (with_data)
    {
        string data = argv[3];            //Training set file
        string labels = argv[4];         //Training set file
        L0 = concatenate(read_matrix(data), read_matrix(labels));  //training set matrix L[n][f+1] read from set from the file
    }

    else
    {
        L0 = linear_dataset(300, 30, 10);
        save_dataset(L0,string("Linear"));
    }

    int n = L0.size();
    int f = L0[0].size()-1;
    int n2 = n*boot_frac;

    vector<vector<float>> L = subsample(L0,n,f,n2, rseed);

    int m0 = 0.5*n2;
    int m = min(50,m0);              //Size of aggressive subsample



//=====================================================================================================================================//
//======================================================= Running the simulation ======================================================//
//=====================================================================================================================================//

    Params params = Params(pFS,KNN_policy,Reward_policy,Nt,MA,q,k,m,r2,ce,c,cl,b,L,Features,rseed, alpha);
    Run_feature_selection(params);
}
