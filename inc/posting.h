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

class Posting{
public:
public:
    uint32_t        df;
    uint16_t        length;
    vector<uint8_t> code;

    uint32_t        lastPos;

    Posting():df(0),length(0),lastPos(0){}

};

class PostingList{

    //=====================================
    // INSTANCE VARIABLES
    //=====================================

    uint16_t            SIZE;                                       // Size in total BITS
    vector<Posting>     list;                                       // list of postings
    uint16_t            lastID;

    //=====================================
    // METHODS
    //=====================================
    uint8_t getSelector(uint16_t gap);
    uint8_t getPosSelector(uint32_t gap);

    inline void setBIT(bool bit,Posting& p);

    void insert(uint32_t gap);
    void insert(uint16_t gap);
    void insert(uint16_t ID,uint32_t pos,bool isPos);

    void compressID(uint16_t docID,uint32_t pos);

public:
    PostingList();
    ~PostingList();

    uint16_t getSize(){
        return SIZE;
    }

    vector<Posting>* getList(){
        return &list;
    }

    uint16_t getListSize(){
        return SIZE;
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
