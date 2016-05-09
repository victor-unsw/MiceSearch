//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_POSTING_H
#define BAZINGA_POSTING_H

#include "cstdint"
#include "vector"
#include "iostream"
#include <tiff.h>

using namespace std;

class Posting{

private:
    uint16_t docID;

public:
    Posting(uint16_t id = 0);

    /*
     * getDocID().
     * - returns the docID
     */
    inline const uint16_t getDocID(){
        return docID;
    }

    /*
     * setDocID(id).
     * - set docID = id;
     */
    void setDocID(uint16_t docID);
};


class PostingList{

private:
    uint16_t            SIZE;                                       // Size in total BITS
    vector<Posting*>  * list;                                       // list of postings

public:
    PostingList();
    ~PostingList();

    uint16_t getSize(){
        return SIZE;
    }

    /*
     * get(i).
     * - returns the posting at index 'i'.
     */
    Posting* get(uint16_t index){
        return (*list)[index];
    }

    vector<Posting*>* getList(){
        return list;
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
