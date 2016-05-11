//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_DICTIONARY_H
#define BAZINGA_DICTIONARY_H

#include "proceeding.h"
#include "unordered_map"
#include "map"
#include "string"

class Dictionary{

private:
    uint32_t                                        SIZE;
    std::unordered_map<std::string,Proceeding*>     map;


public:
    Dictionary();
    ~Dictionary();

    inline uint32_t getSize(){
        return map.size();
    }

    //===============================================================
    // MUTATOR Methods
    //===============================================================

    /*
     * insert(term).
     * - inserts the term into the dictionary.
     * - if proceeding already exist, then appends the docID to it.
     * - else creates a new instance of proceeding.
     */
    void insert(const string& term,uint16_t docID,uint32_t pos);

    /*
     * get(term).
     * - returns the proceeding for given term.
     */
    Proceeding* get(const string term);

    /*
     * returns false if term doesn't exist.
     */
    bool exist(const string term);

    uint32_t flush(ofstream* out);

    uint32_t fill(ifstream* in,uint32_t size);

    //===============================================================
    // Temp methods
    //===============================================================

    void show(){
        std::map<string,Proceeding*> ordered(map.begin(),map.end());
        auto it = ordered.begin();
        for (int i = 0; i < 100; ++i) {
            cout << it->first << endl;
            it++;
        }
    }

    int getTotalFrequency(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getTf();
        }
        return total;
    }

    int getTotalPostingSpace(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getPostingList()->getList()->size();
        }
        return total;
    }

    int totalTermSpace(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getTerm().length();
        }
        return total;
    }

    vector<Proceeding*> getAllProceeding(){
        vector<Proceeding*> p;
        for(auto it=map.begin();it!=map.end();it++)
            p.push_back(it->second);
        return p;
    }
};

#endif //BAZINGA_DICTIONARY_H
