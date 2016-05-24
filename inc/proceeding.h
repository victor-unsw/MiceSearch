//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_PROCEEDING_H
#define BAZINGA_PROCEEDING_H

#include "cstdint"
#include "posting.h"
#include "string"
#include "array"
#include "global.h"
#include "fstream"
#include "algorithm"
#include "cstring"
#include "string.h"
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

    uint16_t getInitialCost();

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
    uint16_t flushPosting(fstream* out);

    uint16_t fill(ifstream* in);
    uint16_t fill(fstream *in);

    Proceeding* merge(Proceeding* p1,Proceeding* p2);

    //=============================================================== TEMP

    void compare(Proceeding* p1,Proceeding* p2){
        // compare term
        //cout << "p1 : " << p1->getTerm() << "\np2 : " << p2->getTerm() << endl;cin.get();

        if (p1->getTerm().compare(p2->getTerm())) {
            cout << "t1 : " << p1->getTerm() << "\t t2 : " << p2->getTerm() << endl;
            cout << "term not equal\n";
            cin.get();exit(1);
        }

        if (p1->getTf() != p2->getTf()){
            cout << "tf1 : " << p1->getTf() << "\ttf2 : " << p2->getTf() << " not equal " <<endl;
            cin.get();exit(1);
        }

        //cout << "comparing postings";
        p1->postings->compare(p1->postings,p2->postings);
    }

    uint32_t totalLength(){
        uint16_t SPACE = sizeof(length);
        SPACE += sizeof(char*);
        SPACE += sizeof(tf);
        SPACE += sizeof(PostingList*);
        SPACE += sizeof(*postings);
        return SPACE;
    }

};

#endif //BAZINGA_PROCEEDING_H
