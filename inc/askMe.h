//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_ASKME_H
#define BAZINGA_ASKME_H

#include "global.h"
#include "indexer.h"
#include "cstring"
#include "string.h"

class SearchInfo{
private:
    Information*        info    = NULL;
    Storage*            store   = NULL;
    vector<uint32_t>*   pt      = NULL;
    vector<uint32_t>*   pos     = NULL;

    string index_file;
public:
    SearchInfo(Information* i,string file);

    int termIndex(string s);

    Proceeding* getProceeding(string s);

    bool exist(string s);

    vector<string>* getTerms(){
        vector<string>* results = new vector<string>;
        for (auto i = pt->begin(); i != pt->end(); ++i) {
            results->push_back(store->get(*i));
        }
        return results;
    }

};

static bool pairCompare(const std::pair<int,double_t>& firstElement,const std::pair<int,double_t>& secondElement){
    return firstElement.second > secondElement.second;
}

static bool pairStringCompare(const std::pair<double_t,string>& firstElement,const std::pair<double_t,string>& secondElement){
    return (firstElement.first > secondElement.first);
}
static bool pairDoubleCompare(const std::pair<double_t,string>& firstElement,const std::pair<double_t,string>& secondElement){
    return (firstElement.first >= secondElement.first) && (firstElement.second.compare(firstElement.second) > 0);
}

class QueryEngine{

private:
    Information  *   dictionary;
    SearchInfo   *   searchInfo;
    string           inputFolder;

    void incrementFrequency(Proceeding* p,vector<uint16_t>* r,double_t* freq);
    void incrementFrequency(pair<Proceeding*,vector<uint16_t>*> p,double_t* freq);

    void showFrequencies(double_t* freq){
        for (int i = 0; i < 2001; ++i) {
            if (!freq[i])
                continue;
            cout<< "[ " << i << " : " << freq[i] << " ] ";
        }
    }
    void updateDocumentID(vector<uint16_t>* souce,vector<uint16_t>* target);

    vector<int> getSortedID(vector<int> id,double_t* freq);

    vector<string> filesSorted(vector<pair<string,int>> files,vector<int> id,double_t* freq);

public:
    QueryEngine(Information *d = NULL,SearchInfo* s = NULL,string folder= NULL):dictionary(d),searchInfo(s),inputFolder(folder) { }

    vector<string> SvS(vector<string> queryTerms);

    vector<uint16_t>* intersect(vector<uint16_t> *l1, vector<uint16_t> *l2);

    vector<uint16_t>* decode(vector<uint8_t> *l);

};


#endif //BAZINGA_ASKME_H
