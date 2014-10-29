//
//  main.cpp
//  FrequentPatternMining


#include <iostream>
#include "FPGrowth.h"

int main(int argc, const char * argv[])
{
    
    float minSupport;
    FPGrowth *fp;   
    
    //test retail.dat
    minSupport = (float)40/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("retail.dat");
    fp->outputToFile("retail_40.dat");
    delete fp;
    
    minSupport = (float)45/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("retail.dat");
    fp->outputToFile("retail_45.dat");
    delete fp;
    
    minSupport = (float)50/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("retail.dat");
    fp->outputToFile("retail_50.dat");
    delete fp;
    
    minSupport = (float)55/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("retail.dat");
    fp->outputToFile("retail_55.dat");
    delete fp;
    
    minSupport = (float)60/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("retail.dat");
    fp->outputToFile("retail_60.dat");
    delete fp;
    
    //test T10I4D100K.dat
    minSupport = (float)0.5/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("T10I4D100K.dat");
    fp->outputToFile("T10I4D100K_0.5.dat");
    delete fp;
    
    minSupport = (float)0.4/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("T10I4D100K.dat");
    fp->outputToFile("T10I4D100K_0.4.dat");
    delete fp;
    
    minSupport = (float)0.3/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("T10I4D100K.dat");
    fp->outputToFile("T10I4D100K_0.3.dat");
    delete fp;
    
    minSupport = (float)0.2/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("T10I4D100K.dat");
    fp->outputToFile("T10I4D100K_0.2.dat");
    delete fp;
    
    minSupport = (float)0.1/100;
    fp = new FPGrowth(minSupport);
    fp->initFromFile("T10I4D100K.dat");
    fp->outputToFile("T10I4D100K_0.1.dat");
    delete fp;
    
    return 0;
}

