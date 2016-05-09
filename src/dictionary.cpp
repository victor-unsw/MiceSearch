//
// Created by Demon on 10/05/16.
//

#include "../inc/dictionary.h"

Dictionary::Dictionary():SIZE(0) { }

/*
 * insert(term).
 * - inserts the term into the dictionary.
 * - if proceeding already exist, then appends the docID to it.
 * - else creates a new instance of proceeding.
 */
void Dictionary::insert(const string& term,uint16_t docID,uint32_t pos) {
    unordered_map<string,Proceeding*>::iterator value = map.find(term);

    if (value == map.end()){

        // new term inserted
        map.insert(std::make_pair(term,new Proceeding(term,docID,pos)));
        SIZE++;

    } else{

        // term already exists
        value->second->insert(docID,pos);
    }
}

/*
 * get(term).
 * - returns the proceeding for given term.
 */
Proceeding* Dictionary::get(const string term) {
    unordered_map<string, Proceeding *>::iterator value = map.find(term);

    if (value == map.end()) {
        cout << "returning NULL for proceeding";
        return NULL;
    }

    return value->second;
}

/*
 * returns false if term doesn't exist.
 */
bool Dictionary::exist(const string term) {
    return map.find(term) != map.end();
}