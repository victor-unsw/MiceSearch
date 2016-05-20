//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_PROCEEDING_H
#define BAZINGA_PROCEEDING_H

#include "cstdint"
#include "posting.h"
#include "string"
#include "array"
#include "fstream"
#include "algorithm"
#include "iterator"

class Proceeding{

private:
    uint8_t         length;                                 // length of characters in term
    char        *   term;                                   // no '\0' character specified in end
    uint32_t        tf;                                     // total frequency
    PostingList *   postings;                               // pointer to postings

    Proceeding(Proceeding& p);

    void initializeTerm(uint8_t size);

public:
    Proceeding();
    Proceeding(string t,uint16_t docID,uint32_t pos = 0);
    ~Proceeding();

    /*
     * getTermLength().
     * - return the total length of characters
     *   in the term.
     */
    uint8_t getTermLength(){
        return length;
    }

    /*
     * getTerm().
     * - returns string from c_string 'term'.
     */
    string getTerm(){
        return string(term,0,length);
    }

    /*
     * return pointer to posting list.
     */
    PostingList* getPostingList() const{
        return postings;
    }

    /*
     * getTf().
     * - returns the total term frequency
     *   in the 'collection'.
     */
    uint32_t getTf(){
        return tf;
    }

    //===============================================================
    // MUTATOR Methods
    //===============================================================

    inline void incrementTF();

    uint16_t insert(uint16_t docID,uint32_t pos);

    uint16_t flush(ofstream* out);
    uint16_t flush(fstream* out);

    uint16_t fill(ifstream* in);

    Proceeding* merge(Proceeding* p1,Proceeding* p2);

    //=============================================================== TEMP
    void deleteList(){
        delete postings;
    }

    void show(){
        cout << "Term : " << term << "\tTF : " << tf << endl;
    }
};

#endif //BAZINGA_PROCEEDING_H
