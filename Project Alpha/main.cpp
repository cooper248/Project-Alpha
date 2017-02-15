//
//  main.cpp
//  Project Alpha
//
//  Created by Cooper Richardson on 2/11/17.
//  Copyright © 2017 Cooper Richardson. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <assert.h>
#include <random>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <time.h>
using namespace std;

#define crRand (double)rand()/RAND_MAX

// Create globals
class arm{
public:
    vector<double> testTest;
    double muValue;
    double sigmaValue;
    int learner;
    
    void createArm(double,double);
};

// Following function: 'pullArm' copied directly from wiki - Box Muller Transform - No credit taken
double pullValue(double mu, double sigma){
    const double epsilon = std::numeric_limits<double>::min();
    const double two_pi = 2.0*3.14159265358979323846;
    
    static double z0, z1;
    static bool generate;
    generate = !generate;
    
    if (!generate)
        return z1 * sigma + mu;
    
    double u1, u2;
    do
    {
        u1 = rand() * (1.0 / RAND_MAX);
        u2 = rand() * (1.0 / RAND_MAX);
    }
    while ( u1 <= epsilon );
    
    z0 = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
    z1 = sqrt(-2.0 * log(u1)) * sin(two_pi * u2);
    return z0 * sigma + mu;
}

void arm::createArm(double mu, double sigma){
    muValue = mu;
    sigmaValue = sigma;
    learner = 1-crRand+crRand;
};

void startProgram(vector<arm> *allArms,int *numArms){
    srand(time(NULL));
    int generous=5;
    int loose = 20;
    int setMuRange = 10.0;

    cout<< "How many arms would you like to create for this saucy little bandit? ";
    cin>>*numArms;
    cout<< endl;
    for (int i=0;i<*numArms;i++)
    {
        double muValue=generous+(rand()% setMuRange)+crRand-crRand;
        double sigmaValue=rand()% loose+ crRand-crRand;
        
        arm A;
        A.createArm(muValue, sigmaValue);
        allArms->push_back(A);
    }
};

void pullArm(vector<arm> *allArms,int *numArms){
    int pull;
    
    cout<<"Which arm would you like to pull? ";
    cin>>pull;
    while(pull>*numArms){
        cout<<"Sorry, this arm is not within the range of created arms. You may choose between 1 and "<< *numArms<<endl;
        cin>>pull;
    }
    double result = pullValue(allArms->at(pull-1).arm::muValue,allArms->at(pull-1).arm::sigmaValue);
    cout<<"You made "<< result << "dollars." << endl;
};

void checkArms(int *numArms,int iterations, vector<arm> *allArms, vector<vector<double>>*allPullValues){
    
    for(int i=0; i<*numArms; i++)
    {
        vector<double> pullValues;
        for(int j=0; j<=iterations; j++)
        {
            double result= pullValue(allArms->at(i).arm::muValue,allArms->at(i).arm::sigmaValue);
            pullValues.push_back(result);
    }
        allPullValues->push_back(pullValues);
    }
};

void printValues(int iterations, vector<vector<double>> *allPullValues, vector<arm> *allArms){
    for(int i =0;i<allPullValues->size();i++){
        /*for(int j=0;j<iterations;j++){
            cout<<allPullValues->at(i)[j] << " / ";
        } */
        cout<<endl <<endl<< "Your mean value for arm "<< i+1 << " is: "<< allArms->at(i).arm::muValue << endl;
        cout << "Your sigma value for arm "<< i +1<< " is: "<< allArms->at(i).arm::sigmaValue << endl ;
        cout<< endl << "Next set "<< endl;
    }
};

double checkAverage(int* iterations, vector<vector<double>>* allPullValues){
    double sum=0;
    for(int i =0;i<allPullValues->size();i++){
        for(int j=0;j<*iterations;j++){
            sum = sum + allPullValues->at(i)[j];
        }
    }
    int iter = *iterations;
    return sum/iter;
};

int decide(vector<arm> *allArms, int* numArms){
    
    int check= -1000;
    int armToUse;
    double decide = crRand;
    double alpha=.1;
    
    if(decide>alpha)
    {
        cout<<"Greedy action taken "<< endl;
        
    for(int i =0;i<allArms->size();i++){
        if(check<allArms->at(i).arm::learner){
            check=allArms->at(i).arm::learner;
            armToUse=i;
        }
    }
    }
    else{
        armToUse= rand()%*numArms;
        cout<<"random action taken ///////////"<< endl;
    };
    return armToUse;
};

double act(vector<arm>* allArms, int *armNumber){
    double result= pullValue(allArms->at(*armNumber).arm::muValue,allArms->at( *armNumber ).arm::sigmaValue);
    return result;
};

void react(vector<arm> *allArms, double *result, int *armNumber){
    double alpha=.08;
    allArms->at( *armNumber ).arm::learner = *result * alpha + (allArms->at( *armNumber ).arm::learner) * (1-alpha);
};

int main(){
    // Create Variables
    vector<arm> allArms;
    vector<vector<double>> allPullValues;
    int numArms;
    int iterations=100;
    char yesNo;
    double average;
    char wait;
    
    // Function to prompt user and create all arms
    startProgram(&allArms, &numArms);
    checkArms(&numArms,iterations,&allArms, &allPullValues);
    printValues(iterations, &allPullValues, &allArms);
    
    cout<<"would you like to continue? "<< endl;
    cin>> wait;
    
  for(int i=1; i<100000;i++){
        int armNumber=decide(&allArms, &numArms);
        double result = act(&allArms, &armNumber);
        react(&allArms, &result, &armNumber);
        cout<<"Your MAB pulled arm " << armNumber+1 << endl;
  };
    
    
    
    
    
    
    
    

    //FUCNTION ITERATES THROUGH SET NUMBER OF PULLS ON EACH ARM checkArms(&numArms,&iterations,&allArms, &allPullValues);
    
    // FUNCTION PRINTS ALL PULLED VALUES  printValues(&iterations, &allPullValues);
    // FUNCTION CHECKS AVERAGE    average=checkAverage(&iterations, &allPullValues);
    

    /*
     
     User function - CREATES USER INTERFACE TO REENACT A SLOT MACHINE
     
     while(yesNo!='n'&&yesNo!='N'){
    pullArm(&allArms, &numArms);
    cout<< "Would you like to choose another arm? type Y or N ";
        cin>>yesNo;
        cout<<endl;
        }
    */
    
    return 0;
}








