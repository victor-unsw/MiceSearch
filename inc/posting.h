//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_POSTING_H
#define BAZINGA_POSTING_H

#include "cstdint"
#include "vector"
#include "iostream"
#include "global.h"
#include <tiff.h>

using namespace std;


class PostingList{

    //=====================================
    // INSTANCE VARIABLES
    //=====================================

    uint16_t            SIZE;                                       // Size in total BITS
    vector<uint8_t>    list;                                       // list of postings
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

    vector<uint8_t>* getList(){
        return &list;
    }

    /*
     * insert(id,pos).
     * - inserts a new posting if already not there,
     *   at the end of the list.
     * - returns last inserted docID.
     */
    uint16_t insert(uint16_t docID,uint32_t pos);

};


#endif //BAZINGA_POSTING_H
