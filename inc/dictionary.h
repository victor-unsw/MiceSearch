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
        return SIZE;
    }

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

    // ------------ TEMP METHODS
    void show(){
        std::map<string,Proceeding*> ordered(map.begin(),map.end());
        auto it = ordered.begin();
        for (int i = 0; i < 100; ++i) {
            cout << it->first << endl;
            it++;
        }
    }
};

#endif //BAZINGA_DICTIONARY_H
