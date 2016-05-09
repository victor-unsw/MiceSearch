//
// Created by Demon on 09/05/16.
//

#include "../inc/posting.h"

int TOTAL_POSTINGS = 0;

//===============================================================
// POSTING_LIST
//===============================================================

PostingList::PostingList():SIZE(0),list(NULL) { }

/*
 * destructor().
 * - deletes each posting from list.
 * - deletes lis itself.
 */
PostingList::~PostingList() {
//    for (auto it= list->begin();it != list->end(); it++)
//        delete *it;
    delete list;
}

/*
 * insert(id,pos).
 * - inserts a new posting if already not there,
 *   at the end of the list.
 * - returns last inserted docID.
 */
uint16_t PostingList::insert(uint16_t docID, uint32_t pos) {
    if (!list)
        list = new vector<uint16_t>;

    if (list->size() > 0 && (*list).back() == docID){
        // update last posting

    }else{
        // insert new posting
        list->push_back(docID);
        SIZE+= 16;                                   // incrementing 16 bits currently per entry
        TOTAL_POSTINGS++;
    }

    return docID;
}



//===============================================================
// POSTING
//===============================================================

Posting::Posting(uint16_t id):docID(id) { }


/*
 * setDocID(id).
 * - set docID = id;
 */
void Posting::setDocID(uint16_t docID) {
    this->docID = docID;
}