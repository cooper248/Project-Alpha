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
// Found from following location: https://en.wikipedia.org/wiki/Box–Muller_transform
// Function actions: Creates a random value given a normal distrubtion after a mean and standard deviation are given
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
// Fucntion actions: initializes arm with its starting mean, sigma and learning value
void arm::createArm(double mu, double sigma){
    muValue = mu;
    sigmaValue = sigma;
    learner = 1-crRand+crRand;
};

//Function actions: determines the generosity and looseness of each slot machine arm. Prompts user for number of arms and creates said number of arms.
void startProgram(vector<arm> *allArms,int *numArms){
    srand(time(NULL));
    int generous=0;
    int loose = 10;
    int setMuRange = 80.0;

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

// Allows user to select which arm they would like to pull, then outputs the winning values to user.
void pullArm(vector<arm> *allArms,int *numArms){
    int pull;
    
    cout<< endl <<"Which arm would you like to pull? ";
    cin>>pull;
    while(pull>*numArms){
        cout<<"Sorry, this arm is not within the range of created arms. You may choose between 1 and "<< *numArms<<endl;
        cin>>pull;
    }
    double result = pullValue(allArms->at(pull-1).arm::muValue,allArms->at(pull-1).arm::sigmaValue);
    cout<<"You made "<< result << "dollars." << endl;
};

// Checks the values of created arms and places data into a vector which printValues() can then call upon
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

// Prints data from checkArms() vectors along with the created mean and sigma values for user to verify
void printValues(int iterations, vector<vector<double>> *allPullValues, vector<arm> *allArms){
    
    char wait;
    
    for(int i =0;i<allPullValues->size();i++){
        for(int j=0;j<iterations;j++){
            cout<<allPullValues->at(i)[j] << " / ";
        }
        cout<<endl <<endl<< "Your mean value for arm "<< i+1 << " is: "<< allArms->at(i).arm::muValue << endl;
        cout << "Your sigma value for arm "<< i +1<< " is: "<< allArms->at(i).arm::sigmaValue << endl ;
        cout<< endl << "Next set "<< endl;
    }
    cout<<"would you like to continue? ";
    cin>> wait;
};

// Decide which arm to pull based on 10% exploration and 90% being a greedy mongrel
int decide(vector<arm> *allArms, int* numArms){
    
    int check= -10;
    int armToUse;
    double decide = crRand;
    double exploreRate=.1;
    
    if(decide>exploreRate)
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

// Pulls the arm based on the decision from decide()
double act(vector<arm>* allArms, int *armNumber){
    double result= pullValue(allArms->at(*armNumber).arm::muValue,allArms->at( *armNumber ).arm::sigmaValue);
    return result;
};

// updates value of pull each time the act() function is used in order to learn
void react(vector<arm> *allArms, double *result, int *armNumber){
    double alpha=.1;
    allArms->at( *armNumber ).arm::learner = *result * alpha + (allArms->at( *armNumber ).arm::learner) * (1-alpha);
};

// Agent runs through each decide act react scenario to determine best resulted outcome and saves information in vector
int agentDoYoThang(vector<arm> &allArms, int &numArms,vector<double> *results){
    int armNumber;
    for(int i=1; i<10000;i++){
        armNumber=decide(&allArms, &numArms);
        double result = act(&allArms, &armNumber);
        react(&allArms, &result, &armNumber);
        cout<<"Your MAB pulled arm " << armNumber+1 <<" and won a value of "<< result<< " dollars."<< endl;
        results->push_back(result);
    };
    return armNumber;
};

// outputs file to .txt file which can be used to paste info into excel
void createFile(vector<double> *results){
    ofstream allResults;
    allResults.open("slotResults.txt");
    for(int i =0;i<results->size();i++){
        allResults << results->at(i)  <<"\n";
    }
    allResults.close();
};

// allows user to pull whichever created arm they'd like to test the system
void userPlay(vector<arm> &allArms, int &numArms){
    char yesNo;
    while(yesNo!='n'&&yesNo!='N'){
        pullArm(&allArms, &numArms);
        cout<< endl << "You feelin' lucky punk? type Y or N ";
        cin>>yesNo;
        cout<<endl;
    }
    
};

// Test A - Average of pulls converges to created mean value
void testA(int iterations, vector<vector<double>>* allPullValues, vector<arm> *allArms){
    double sum=0;
    int armNumber;
    
    cout<<"Which arm would you like to test to see if the average converges properly? ";
    cin>> armNumber;
    while(armNumber>allArms->size()){
        cout<<"Sorry, this arm is not within the range of created arms. You may choose between 1 and "<< allArms->size()<<endl;
            cout<<"Let me ask you again. Which arm would you like to test to see if the average converges properly?  ";
        cin>>armNumber;
    }
    
    for(int j=0;j<iterations;j++){
        sum = sum + allPullValues->at(armNumber-1)[j];
    }
    
    cout<< endl << "Calculated average pull for arm "<< armNumber << " is "<< sum/iterations<<".";
    cout<< endl << "Pre-Determined mu value for arm "<< armNumber << " is "<< allArms->at(armNumber-1).arm::muValue<<".";
    assert(allArms->at(armNumber-1).arm::muValue<=(sum/iterations)*1.01&&allArms->at(armNumber-1).arm::muValue>=(sum/iterations)*.99);
    cout<< endl << "Well the program didn't crash, so it looks like you're good to go."<<endl;
};

// Test B - If obvious best choice is available, prove after many pulls that this arm will be chosen a majority of the time
void testB(vector<vector<double>> *allPullValues, vector<arm> *allArms,int armNumber){
    
    cout<<endl<< "Please review the following data values to match with calculated best option";
    for(int i =0;i<allPullValues->size();i++){
        cout<<endl<< "Your mean value for arm "<< i+1 << " is: "<< allArms->at(i).arm::muValue << endl;
        cout << "Your sigma value for arm "<< i +1<< " is: "<< allArms->at(i).arm::sigmaValue << endl ;
        cout<< endl << "Next values "<< endl;
    }
    int test=-1;
    int check;
    for(int i =0;i<allPullValues->size();i++){
    if(allArms->at(i).arm::learner>test){
        test=allArms->at(i).arm::learner;
        check = i;
    }
    }
    cout<< "Final ending value for best arm based on the learner: " << check +1<< endl;
    cout<< "Final ending value from running code: "<< armNumber+1 <<endl;
    assert(check==armNumber);
    cout<<"Looks like we check out yet again. Test B is good to go!"<<endl;
};

int main(){
    // Create Variables
    vector<arm> allArms;
    vector<vector<double>> allPullValues;
    vector<double> results;
    
    int numArms;
    int armNumber;
    int iterations=10000;

    startProgram(&allArms, &numArms);
    checkArms(&numArms,iterations,&allArms, &allPullValues);
    printValues(iterations, &allPullValues, &allArms);
    armNumber = agentDoYoThang(allArms, numArms,&results);
    createFile(&results);
    userPlay(allArms,numArms);
    testA(iterations, &allPullValues, &allArms);
    testB(&allPullValues, &allArms,armNumber);
    return 0;
}


