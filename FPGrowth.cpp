//
//  FPGrowth.cpp
//  FrequentPatternMining


#include "FPGrowth.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#pragma -mark public method

FPGrowth::FPGrowth(float _minSupport)
{
    //init root node
    root = new FPTreeNode("",NULL,0);
    //set min support
    minSupport = _minSupport;
}

void FPGrowth::initFromFile(string fileName)
{
    cout << "Init from file: " << fileName  << " ,with min support: " << minSupport <<endl;
    flush(cout);
    start = clock();
    buildHeaderTable(fileName);
    buildFPTree(fileName);
    prefix = NULL; //insure that first mining will creat a new prefix
}

void FPGrowth::outputToFile(string fileName)
{
    ofstream ofile;
    ofile.open(fileName.c_str());
    if (!ofile) {
        cout << "Can not open file :" << fileName << endl;
        return;
    }
    
    vector<FPFreqResult> result;
    output(result);
    time_t t_end = time(NULL) ;
    cout<< "Total time used for mining: "<< (clock()-start)/double(CLOCKS_PER_SEC) << " (sec)" << endl;
    flush(cout);
    vector<FPFreqResult>::iterator it;
    for (it = result.begin(); it != result.end(); it ++) {
        vector<FPTransItem>::iterator itemIt;
        for (itemIt = it->items.begin(); itemIt != it->items.end(); itemIt ++) {
            ofile << itemIt->name << " " ;
        }
        ofile << " : " << it->freq << endl;
    }
    t_end = time(NULL) ;
    cout<< "Total time used for mining and output: "<< (clock()-start)/double(CLOCKS_PER_SEC) << " (sec)" << endl;
    flush(cout);
    ofile.close();

}

void FPGrowth::output(vector<FPFreqResult> &result)
{
    miningFPTree(result);
}

//any allocated space in trans need caller to delete immediately
void FPGrowth::initFromTrans(vector<FPTrans> &trans)
{
    buildHeaderTable(trans);
    buildFPTree(trans);
}

#pragma -mark private method

void FPGrowth::buildHeaderTable(string fileName)
{
    ifstream ifile;
    ifile.open(fileName.c_str());
    if (!ifile) {
        cout << "Can not open file :" << fileName << endl;
        return;
    }
    
    string tran; //get a transaction
    int index = 0; //index in HeaderTable
    int transCount = 0;
    while (getline(ifile,tran)) {
        transCount ++;
        stringstream strStream(tran);
        string item;
        while (strStream >> item) {
            if (nameIndex.find(item) != nameIndex.end()) {
                FPHeaderTable[nameIndex[item]]->freq ++;
            }
            else{ //item doesn't exit in Header Table
                nameIndex[item] = index;
                FPHeaderTableNode *headerTableNode = new FPHeaderTableNode(item);
                FPHeaderTable.push_back(headerTableNode);
                index ++;
            }
        }
    }
    ifile.close();
    
    
    //delete items that not match min Support
    vector<FPHeaderTableNode *>::iterator it;
    minCountSupport = (float)transCount * minSupport;
    for (it = FPHeaderTable.begin(); it != FPHeaderTable.end(); it++)
    {
        FPHeaderTableNode *node = *it;
        if ((float)(node->freq) < minCountSupport) {
            it = FPHeaderTable.erase(it);
            it --;
        }
    }
    
    sortHeaderTable();
}

void FPGrowth::buildHeaderTable(vector<FPTrans> &trans)
{
    int index = 0; //index in HeaderTable
    int transCount = 0;
    vector<FPTrans>::iterator transIt;
    for (transIt = trans.begin(); transIt != trans.end(); transIt++) {
        transCount ++;
        vector<FPTransItem>::iterator itemIt;
        for (itemIt = transIt->items.begin(); itemIt != transIt->items.end(); itemIt++) {
            if (nameIndex.find((itemIt)->name) != nameIndex.end()) {
                FPHeaderTable[nameIndex[(itemIt)->name]]->freq += (itemIt)->count;
            }
            else{ //item doesn't exit in Header Table
                nameIndex[(itemIt)->name] = index;
                FPHeaderTableNode *headerTableNode = new FPHeaderTableNode((itemIt)->name,(itemIt)->count);
                FPHeaderTable.push_back(headerTableNode);
                index ++;
            }
        }
    }
    
    //delete items that not match min Support
    vector<FPHeaderTableNode *>::iterator it;
    
    for (it = FPHeaderTable.begin(); it != FPHeaderTable.end(); it++)
    {
        FPHeaderTableNode *node = *it;
        if ((float)(node->freq) < minCountSupport) {
            it = FPHeaderTable.erase(it);
            it --;
        }
    }
    
    sortHeaderTable();
}


bool CompareHeaderTableNode(FPHeaderTableNode *a,FPHeaderTableNode *b)
{
    return a->freq > b->freq;
}
void FPGrowth::sortHeaderTable()
{
    sort(FPHeaderTable.begin(), FPHeaderTable.end(), CompareHeaderTableNode);

    //update name index
    int index = 0;
    nameIndex.clear();
    vector<FPHeaderTableNode *>::iterator it;
    for (it = FPHeaderTable.begin(); it != FPHeaderTable.end(); it++)
    {
        FPHeaderTableNode *node = *it;
        nameIndex[node->name] = index;
        index ++;
    }
    itemCount = index;
}


void FPGrowth::insertTran(vector<string> &items)
{
    FPTreeNode *currentTreeNode = root;
    unordered_map<string, FPTreeNode*> *currentChildren;
    currentChildren = &(currentTreeNode->children);
    vector<string>::iterator it;
    for (it = items.begin(); it != items.end(); it ++) {
        
        //if prefix match
        if (currentChildren->find(*it) != currentChildren->end()) {
            (*currentChildren)[*it]->count ++;
            currentTreeNode = (*currentChildren)[*it];
            currentChildren = &(currentTreeNode->children);
        }
        else{
            FPTreeNode *newTreeNode = new FPTreeNode(*it,currentTreeNode);
            (*currentChildren)[*it] = newTreeNode;
            newTreeNode->parent = currentTreeNode; //set parent
            currentTreeNode = newTreeNode;
            currentChildren = &(newTreeNode->children);
            
            //update FPHeaderTable
            if (FPHeaderTable[nameIndex[*it]]->head == NULL) {
                FPHeaderTable[nameIndex[*it]]->head = currentTreeNode;
                FPHeaderTable[nameIndex[*it]]->end = currentTreeNode;
            }
            else {
                FPHeaderTable[nameIndex[*it]]->end->link = currentTreeNode;
                FPHeaderTable[nameIndex[*it]]->end = currentTreeNode;
            }
        }
        

    }
}

void FPGrowth::insertTran(vector<FPTransItem*> &items)
{
    FPTreeNode *currentTreeNode = root;
    unordered_map<string, FPTreeNode*> *currentChildren;
    currentChildren = &(currentTreeNode->children); //a map from item name to children tree node
    
    vector<FPTransItem*>::iterator it;
    for (it = items.begin(); it != items.end(); it ++) {
        
        //if prefix match, search next
        if (currentChildren->find((*it)->name) != currentChildren->end()) {
            
            (*currentChildren)[(*it)->name]->count += (*it)->count;
            
            currentTreeNode = (*currentChildren)[(*it)->name];
            currentChildren = &(currentTreeNode->children);
        }
        else{ //prefix not match
            FPTreeNode *newTreeNode = new FPTreeNode((*it)->name,currentTreeNode);
            
            newTreeNode->count = (*it)->count;
            
            (*currentChildren)[(*it)->name] = newTreeNode;
            newTreeNode->parent = currentTreeNode; //set parent
            currentTreeNode = newTreeNode;
            currentChildren = &(newTreeNode->children);
            
            //update FPHeaderTable
            if (FPHeaderTable[nameIndex[(*it)->name]]->head == NULL) {
                FPHeaderTable[nameIndex[(*it)->name]]->head    = currentTreeNode;
                FPHeaderTable[nameIndex[(*it)->name]]->end     = currentTreeNode;
            }
            else {
                FPHeaderTable[nameIndex[(*it)->name]]->end->link   = currentTreeNode;
                FPHeaderTable[nameIndex[(*it)->name]]->end         = currentTreeNode;
            }
        }

    }
}

//sort a transaction use nameIndex
unordered_map<string, int> *gNameIndex = NULL;
bool CompareItem(string a,string b)
{
    return (*gNameIndex)[a] < (*gNameIndex)[b];
}
bool CompareTransItem(FPTransItem *a,FPTransItem *b)
{
    return (*gNameIndex)[a->name] < (*gNameIndex)[b->name];
}
void FPGrowth::buildFPTree(string fileName)
{
    ifstream ifile;
    ifile.open(fileName.c_str());
    if (!ifile) {
        cout << "Can not open file :" << fileName << endl;
        return;
    }
    
    string tran; //get a transaction

    while (getline(ifile,tran)) {
        stringstream strStream(tran);
        string item;
        vector<string> items; //transaction items
        while (strStream >> item) {
            if (nameIndex.find(item) != nameIndex.end()) {
                items.push_back(item); //only add item that match minSupport
            }
        }
        
        //sort items
        gNameIndex = &nameIndex;
        sort(items.begin(), items.end(), CompareItem);
        gNameIndex = NULL;
        
        insertTran(items);//insert a modified transaction to FPTree
    }
    ifile.close();
}

void FPGrowth::buildFPTree(vector<FPTrans> &trans)
{
    vector<FPTrans>::iterator transIt;
    vector<FPTransItem>::iterator itemIt;
    for (transIt = trans.begin(); transIt != trans.end(); transIt++) {
        vector<FPTransItem*> items;
        for (itemIt = transIt->items.begin(); itemIt != transIt->items.end(); itemIt++) {
            if (nameIndex.find((itemIt)->name) != nameIndex.end()) {
                items.push_back(&(*itemIt)); //only add item that match minSupport
            }
        }
        //sort items
        gNameIndex = &nameIndex;
        sort(items.begin(), items.end(), CompareTransItem);
        gNameIndex = NULL;
        insertTran(items);//insert a modified transaction to FPTree
    }
}

#pragma -mark  Mining
void FPGrowth::printSingleResult(vector<FPFreqResult> &result)
{
    FPFreqResult resultLine;
    vector<FPTransItem *>::iterator it;
    it = prefix->end() - 1;
    int supportCount = (*it)->count;
    for (; it >= prefix->begin(); it --) {
        FPTransItem item((*it)->name,0);
        resultLine.items.push_back(item);
    }
    resultLine.freq = supportCount;
    result.push_back(resultLine);
}

void FPGrowth::miningFPTree(vector<FPFreqResult> &result)
{
    if (prefix == NULL){ //First mining
        prefix = new vector<FPTransItem *>;
    }
    
    if (FPHeaderTable.size() == 0) { //mining arrives root
        if (prefix->size() > 0) {
            printSingleResult(result);
        }
        return;
    }
    
    if (prefix->size() > 0) { //print current prefix
        printSingleResult(result);
    }
    
    //mining each item in HeaderTable
    vector<FPHeaderTableNode *>::iterator it;
    
    for (it = FPHeaderTable.end() - 1; it >= FPHeaderTable.begin(); it--)
    {
        FPTransItem *newPrefix = new FPTransItem((*it)->name,0);
        vector<FPTrans> trans;
        FPTreeNode *cur;
        for (cur = (*it)->head; cur != (*it)->end; cur = cur->link) {
            vector<FPTransItem> items;
            //search from node to root,build a modified trans
            FPTreeNode *curToRoot;
            int basicCount = cur->count;
            for (curToRoot = cur->parent;curToRoot->parent != NULL;curToRoot = curToRoot->parent){
                FPTransItem item(curToRoot->name,basicCount);
                items.push_back(item);
            }
            FPTrans tranLine = FPTrans(items);
            trans.push_back(tranLine);
        }
        
        //when cur == (*it)->end
        vector<FPTransItem> items;
        //search from node to root,build a modified trans
        FPTreeNode *curToRoot;
        int basicCount = cur->count;
        for (curToRoot = cur->parent;curToRoot->parent != NULL;curToRoot = curToRoot->parent){
            FPTransItem item(curToRoot->name,basicCount);
            items.push_back(item);
        }
        FPTrans tranLine = FPTrans(items);
        trans.push_back(tranLine);
        
        newPrefix->count = (*it)->freq;
        
        //add prefix
        prefix->push_back(newPrefix);
        
        //recursive mining
        FPGrowth *fpTemp = new FPGrowth(minSupport);
        fpTemp->minCountSupport = minCountSupport;
        fpTemp->initFromTrans(trans);
        fpTemp->prefix = prefix;

        fpTemp->miningFPTree(result);
        
        delete fpTemp;
        prefix->pop_back();
    }

}

#pragma -mark destructor
FPGrowth::~FPGrowth()
{
    if (root != NULL) {
        delete root;
    }
    if (prefix != NULL && prefix->size() == 0) {
        vector<FPTransItem *>::iterator it;
        for (it = prefix->begin(); it != prefix->end(); it++)
        {
            if (*it!=NULL) {
                delete *it;
            }
        }
        delete prefix;
    }
    vector<FPHeaderTableNode *>::iterator it;
    for (it = FPHeaderTable.begin(); it != FPHeaderTable.end(); it++)
    {
        delete *it;
    }
}


#pragma -mark debug method

void FPGrowth::outputHeaderTable()
{
    cout << "HeaderTable: "<< endl;
    vector<FPHeaderTableNode *>::iterator it;
    for (it = FPHeaderTable.begin(); it != FPHeaderTable.end(); it++)
    {
        FPHeaderTableNode *node = *it;
        cout << "name:" << node->name
            << "\tfreq: "<< node->freq
            << "\thead: "<<node->head
            << "\tend: "<<node->end
            <<endl;
    }
    cout << endl;
}

void FPGrowth::outputTran(vector<string> items)
{
    vector<string>::iterator it;
    cout << "Tran items: " ;
    for (it = items.begin(); it != items.end(); it ++) {
        cout << *it << " ";
    }
    cout << endl;
}

void FPGrowth::outputTree()
{
    FPTreeNode* cur = root;
    queue<FPTreeNode *>q;
    q.push(cur);
    cout << "Current Tree is:" << endl;
    while (!q.empty()) {
        cur = q.front();
        q.pop();
        cout << "Name: " << cur->name << "\tCount: " << cur->count << "\tChildren num: "<< cur->children.size() << endl;
        unordered_map<string, FPTreeNode*>::iterator it;
        for (it = cur->children.begin(); it != cur->children.end(); it ++) {
            q.push((*it).second);
        }
    }
    cout << endl;
}

void FPGrowth::outputPrefix()
{
    cout << "Current Prefix is: "<<endl;
    vector<FPTransItem *>::iterator it;
    it = prefix->end() - 1;
    int supportCount = (*it)->count;
    for (; it >= prefix->begin(); it --) {
        cout << (*it)->name << " ";
    }
    cout << supportCount << endl;
}

void FPGrowth::outputFreq(){
    vector<FPTransItem *>::iterator it;
    it = prefix->end() - 1;
    int supportCount = (*it)->count;
    cout << "================================" << endl;
    for (; it >= prefix->begin(); it --) {
        cout << (*it)->name << " ";
    }
    cout << supportCount << endl;
    cout << "================================" << endl << endl;
}
