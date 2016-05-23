//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_DICTIONARY_H
#define BAZINGA_DICTIONARY_H

#include "proceeding.h"
#include "unordered_map"
#include "map"
#include "string"
#include "global.h"

class Dictionary{

private:
    uint32_t                                        SIZE;
    uint32_t                                        SPACE;
    uint32_t                                        totalInsertions;
    std::unordered_map<std::string,Proceeding*>     map;


public:
    Dictionary();
    ~Dictionary();

    inline uint32_t getSize(){
        return map.size();
    }

    uint32_t getSpace(){
        return SPACE;
    }

    uint32_t getTotalInsertions(){
        return totalInsertions;
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
    std::map<string,Proceeding*>* fillOrdered(ifstream* in,uint32_t size);

    //===============================================================
    // Temp methods
    //===============================================================

    std::unordered_map<std::string,Proceeding*>* getMap(){
        return &map;
    };

    void show(){
        std::map<string,Proceeding*> ord(map.begin(),map.end());
        for(auto it=ord.begin();it!=ord.end();it++){
            cout << it->first << endl;
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
            total += i->second->totalLength() + 8;
        }
        return total;
    }

    void showTerms(){
        for (auto i = map.begin(); i != map.end() ; ++i) {
            cout << i->second->getTerm() << endl;
        }
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

    uint32_t flushPosting(fstream* out){
        uint32_t totalBytes = 0;

        // sort the keys
        std::map<string,Proceeding*> ordered(map.begin(),map.end());

        for (auto i = ordered.begin(); i != ordered.end(); ++i) {
            totalBytes += i->second->flushPosting(out);
        }
        return totalBytes;
    }

    uint32_t fillPosting(ifstream* in,uint32_t size){
        uint32_t totalBytes = 0;

        while (totalBytes != size && !in->eof()) {
            PostingList* p = new PostingList;
            totalBytes += p->fill(in);
        }
        return totalBytes;
    }
};

#endif //BAZINGA_DICTIONARY_H
