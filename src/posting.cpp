//
// Created by Demon on 09/05/16.
//

#include "../inc/posting.h"

int TOTAL_POSTINGS = 0;

//=======================================================================================
// POSTING_LIST
//=======================================================================================

//*************************************************
// CONSTRUCTOR & Destructor
//*************************************************

PostingList::PostingList():SIZE(0),lastID(0), postingCount(0) {
    list.reserve(10);
}

/*
 * destructor().
 * - deletes each posting from list.
 * - deletes lis itself.
 */
PostingList::~PostingList() { }




//*************************************************
// INSERT Method
// - this is the major method; where eventually
// - the documentID is added to the list.
//*************************************************
uint16_t PostingList::insert(uint16_t docID, uint32_t pos) {

    if (list.size() > 0 && lastID == docID){
        // increment doc. frequency
        freq.back()++;
    }else{
        // insert new posting
        compressID(docID);
        freq.push_back(1);
        lastID = docID;
        TOTAL_POSTINGS++;
        postingCount++;
    }

    return docID;
}


//=======================================================================================
// Alpha Encoding
//=======================================================================================

void PostingList::compressID(uint16_t docID) {
    uint16_t    gap = (docID-lastID);
    uint8_t     s   = getSelector(gap);
    if (!gap) {
        cout << "\n**EROR [ gap is 0 ]\n";
        exit(1);
    }
    for (int i=0; i<s; i++) {
        setBIT(0);
        SIZE++;
    }
    for (int i=s; i>=0; i--) {
        uint16_t j = 0;
        j = 1 << i;
        if (j & gap){
            setBIT(1);
        }
        else{
            setBIT(0);
        }
        SIZE++;
    }
}

void PostingList::setBIT(bool bit) {
    uint16_t    tBit    = 7 - (SIZE % 8);
    if (tBit == 7)
        list.push_back(0);
    list.back() |= bit << tBit;
}

uint8_t PostingList::getSelector(uint16_t gap) {
    uint16_t    A = 0xF000,
                B = 0x0F00,
                C = 0x00F0,
                D = 0x000F;

    if (A & gap){
        if (gap & 0x8000)
            return 15;
        if (gap & 0x4000)
            return 14;
        if (gap & 0x2000)
            return 13;
        if (gap & 0x1000)
            return 12;
    }   else if(B & gap){
        if (gap & 0x0800)
            return 11;
        if (gap & 0x0400)
            return 10;
        if (gap & 0x0200)
            return 9;
        if (gap & 0x0100)
            return 8;
    }   else if(C & gap){
        if (gap & 0x0080)
            return 7;
        if (gap & 0x0040)
            return 6;
        if (gap & 0x0020)
            return 5;
        if (gap & 0x0010)
            return 4;
    }   else if(D & gap){
        if (gap & 0x0008)
            return 3;
        if (gap & 0x0004)
            return 2;
        if (gap & 0x0002)
            return 1;
        if (gap & 0x0001)
            return 0;
    }
    return 0;
}