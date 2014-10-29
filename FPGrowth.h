//
//  FPGrowth.h
//  FrequentPatternMining

#ifndef __FrequentPatternMining__FPGrowth__
#define __FrequentPatternMining__FPGrowth__

#include <vector>
#include <unordered_map>
#include <string>
#include <queue>
#include <time.h>

using namespace std;

struct FPTreeNode {
    int count;
    string name; //item name
    unordered_map<string, FPTreeNode *> children;
    FPTreeNode* parent;
    FPTreeNode* link; //next node that share same name
    
    FPTreeNode(string _name,FPTreeNode* _parent,int _count = 1)
    {
        name = _name;
        parent = _parent;
        count = _count;
        children.clear();
        link = NULL;
    }
    ~FPTreeNode()
    {
        unordered_map<string, FPTreeNode*>::iterator it;
        for (it = children.begin(); it != children.end(); it ++) {
            if(it->second!= NULL)
            {
                delete it->second;
            }
        }
    }
};

struct FPHeaderTableNode {
    string name;
    int    freq; //total frequency
    FPTreeNode* head; //link to FPTree
    FPTreeNode* end; 
    FPHeaderTableNode(string _name,int _freq = 1,FPTreeNode* _head = NULL)
    {
        name = _name;
        freq = _freq;
        head = _head;
    }
};

//describe transaction
struct FPTransItem {
    string name;
    int count;
    FPTransItem(string _name,int _count)
    {
        name  = _name;
        count = _count;
    }
};
struct FPTrans {
    vector<FPTransItem> items;
    FPTrans(vector<FPTransItem> &_items)
    {
        items = _items;
    }
};

//result
struct FPFreqResult {
    int freq;
    vector<FPTransItem> items;
};

class FPGrowth {
private:
    clock_t start;//use for time calucate
    
    float minCountSupport;//absulate minimum support count
    float minSupport; //threshold value
    int   itemCount; //count of items that march minSupport
    
    FPTreeNode* root;
    vector<FPTransItem *> *prefix;
    
    //item name & frequency & head
    vector<FPHeaderTableNode *> FPHeaderTable;
    
    //name > headerTable index (hash map), nameIndex[0] store the max freq item
    unordered_map<string, int> nameIndex;
    
    
    //init HeaderTable and name-to-HeaderTable inidex
    void buildHeaderTable(string fileName);
    void buildHeaderTable(vector<FPTrans> &trans);
    void sortHeaderTable();
    
    //init FPTree
    void buildFPTree(string fileName);
    void buildFPTree(vector<FPTrans> &trans);
    
    //insert a tran to a tree
    void insertTran(vector<string> &items);
    void insertTran(vector<FPTransItem*> &items);
    
    //mining
    void miningFPTree(vector<FPFreqResult> &result);
    void printSingleResult(vector<FPFreqResult> &result);
    

public:
    FPGrowth(float _minSupport);
    ~FPGrowth();
    void initFromFile(string fileName);
    void initFromTrans(vector<FPTrans> &trans);
    
    void output(vector<FPFreqResult> &result);
    void outputToFile(string fileName);
    
    //FOR DEBUG
    void outputHeaderTable();
    void outputTran(vector<string> items);
    void outputTree();
    void outputPrefix();
    void outputFreq();
};


#endif /* defined(__FrequentPatternMining__FPGrowth__) */
