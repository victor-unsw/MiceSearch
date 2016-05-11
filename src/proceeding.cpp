//
// Created by Demon on 09/05/16.
//

#include "../inc/proceeding.h"

Proceeding::Proceeding(string t,uint16_t docID,uint32_t pos):term(NULL),length(0),tf(0),postings(NULL){
    if (t.size() > 256 || t.size() <= 0){
        cout << "ERROR [ t length ] : " << t.size() << endl;
        exit(1);
    }

    // initialize the term credentials
    this->length    = uint8_t(t.size());
    this->term      = new char[length];
    strncpy(this->term,t.c_str(),length);

    // insert the document ID & position
    insert(docID,pos);
}

Proceeding::~Proceeding() {
    delete postings;
}

inline void Proceeding::incrementTF() {
    tf++;
}


/*
 * insert(id,pos).
 * - inserts the docID with position into the posting list.
 * - increments the total frequency count.
 *
 */
uint16_t Proceeding::insert(uint16_t docID,uint32_t pos) {

    // increment the total frequency count
    incrementTF();

    // initialize a new posting list if
    // it doesn't exist already
    if (!postings)
        postings = new PostingList;

    // insert the document
    return postings->insert(docID,pos);
}