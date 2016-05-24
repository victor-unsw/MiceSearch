//
// Created by Demon on 09/05/16.
//

#ifndef BAZINGA_POSTING_H
#define BAZINGA_POSTING_H

#include "cstdint"
#include "cstring"
#include "string.h"
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

    uint16_t getInitialCost();

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

    uint16_t getFreq(uint16_t docID){
        vector<uint16_t>* decoded = decodePosting(&list);
        uint16_t i = 0;
        uint16_t index = 4000;
        for (; i < decoded->size(); ++i) {
            if ((*decoded)[i] == docID){
                index = i;
                break;
            }
        }
        if (index == 4000 || index >= freq.size()){
            cout << "doc id not correct \n" << endl;
            exit(1);
        }
        delete(decoded);
        return freq[index];
    }

    vector<uint16_t>* decodePosting(vector<uint8_t>* l);

    PostingList* merge(PostingList* p1,PostingList* p2);

    uint16_t fill(ifstream* in);

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
        cout << "doc id length : " << decode->size() << endl;
        cout << "freq length   : " << freq.size() << endl;
        for(auto it = decode->begin();it!=decode->end();it++){
            cout << "["<<*it<<" : " << freq.at(i++) << "] ";
        }
        cout << endl;
        delete decode;
    }

    uint32_t totalLength(){
        return list.size() + (2*freq.size()) + 20;
    }

    void compare(PostingList* p1,PostingList* p2){
        // compare list size
        vector<uint16_t>* l1 = decodePosting(p1->getList());
        vector<uint16_t>* l2 = decodePosting(p2->getList());
        if (l1->size() != l2->size()){
            cout << "list size not equal\n";
            cout << "p1 : " << l1->size() << "\t p2 : " << l2->size() << endl;
            cin.get();exit(1);
        }

        // compare freq size
        if (p1->freq.size() != p2->freq.size()){
            cout << "freq size not equal\n";
            cout << "p1 : " << p1->freq.size() << "\t p2 : " << p2->freq.size() << endl;
            p2->show();
            p1->show();
            cin.get();exit(1);
        }

        // compare list items
        for (int i = 0; i < l1->size(); ++i) {
            if ((*l1)[i] != (*l2)[i]){
                cout << "doc element " << (*l1)[i] << " and " << (*l2)[i] << " not equal\n";
                p1->show();
                p2->show();
                cin.get();exit(1);
            }
        }

        // compare freq. items
        for (int j = 0; j < p1->freq.size(); ++j) {
            if (p1->freq[j] != p2->freq[j]){
                cout << "doc element " << p1->freq[j] << " and " << p2->freq[j] << " not equal\n";
                cin.get();exit(1);
            }
        }
        delete l1;
        delete l2;
    }

};


#endif //BAZINGA_POSTING_H
