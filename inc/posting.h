//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_POSTING_H
#define BAZINGA_POSTING_H

#include "cstdint"
#include "vector"
#include "iostream"
#include "fstream"
#include "global.h"
#include <tiff.h>

using namespace std;


class PostingList{

    //=====================================
    // INSTANCE VARIABLES
    //=====================================

    uint16_t            postingCount;                               // total unique doc ID.
    uint16_t            SIZE;                                       // Size in total BITS
    vector<uint8_t>     list;                                       // list of postings
    vector<uint16_t>    freq;                                       // frequency per doc ID.
    uint16_t            lastID;

    //=====================================
    // METHODS
    //=====================================
    uint8_t getSelector(uint16_t gap);

    inline void setBIT(bool bit);

    void compressID(uint16_t docID);

public:
    PostingList();
    ~PostingList();

    uint16_t getSize(){
        return SIZE;
    }

    uint16_t getPostingCount(){
        return uint16_t(freq.size());
    }

    vector<uint8_t>* getList(){
        return &list;
    }

    vector<uint16_t>* getFreqList(){
        return &freq;
    }

    uint16_t getListSize(){
        return uint16_t(list.size());
    }

    vector<uint16_t>* decodePosting(vector<uint8_t>* l);

    PostingList* merge(PostingList* p1,PostingList* p2);

    /*
     * insert(id,pos).
     * - inserts a new posting if already not there,
     *   at the end of the list.
     * - returns last inserted docID.
     */
    uint16_t insert(uint16_t docID,uint32_t pos);


    void show(){
        vector<uint16_t>* decode = decodePosting(&list);
        int i=0;
        cout << "freq length : " << freq.size() << endl;
        for(auto it = decode->begin();it!=decode->end();it++){
            cout << "["<<*it<<" : " << freq.at(i++) << "] ";
        }
        cout << endl;
    }

};


#endif //BAZINGA_POSTING_H
