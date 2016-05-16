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

PostingList::PostingList():SIZE(0),lastID(0),list(NULL) {
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

    compressID(docID,pos);
    lastID = docID;
    TOTAL_POSTINGS++;

    return docID;
}


//=======================================================================================
// Alpha Encoding
//=======================================================================================

void PostingList::compressID(uint16_t docID,uint32_t pos) {
    if (lastID == 0 || list.size() == 0 || docID != lastID){      // new document ID to be inserted
        Posting p;
        list.push_back(p);
        SIZE += (8)*4;                                              // 4 bytes added ; df & list length
        insert(docID,pos,false);
    }
    insert(docID,pos,true);
    lastID = docID;
}

inline void PostingList::setBIT(bool bit,Posting& p){
    vector<uint8_t>& code = p.code;
    uint16_t    tBit    = uint16_t(7 - (p.length % 8));
    if (tBit == 7){
        code.push_back(0);
        SIZE += 8;                  // increase by 1 byte.
    }
    code.back() |= bit << tBit;
}

uint8_t PostingList::getPosSelector(uint32_t gap){
    int j=31;
    for (; j >=0 ; j--) {
        int temp = 0; temp |= 1 << j;
        if (temp&gap)
            break;
    }
    return uint8_t(j < 0 ? 0 : j);
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


void PostingList::insert(uint32_t gap){
    Posting& p = list.back();

    uint8_t     s   = getPosSelector(gap);
    if (gap <= 0) {
        cout << "\n**EROR [ gap is 0 ]\n";
        exit(1);
    }
    //cout << gap << " ";
    //cout << "[";
    for (int i=0; i<s; i++) {
        setBIT(0,p);
        //cout << 0;
        p.length++;
    }
    //cout << " ";
    for (int i=s; i>=0; i--) {
        uint32_t j = 0;
        j = 1 << i;
        if (j & gap){
            setBIT(1,p);
            //cout << 1;
        }
        else{
            //cout << 0;
            setBIT(0,p);
        }
        p.length++;
    }
    //cout << "]";
}

void PostingList::insert(uint16_t gap){
    Posting& p = list.back();

    uint8_t     s   = getSelector(gap);

    if (gap <= 0) {
        cout << "\n**EROR [ gap is 0 ]\n";
        exit(1);
    }
    //cout << gap << " ";
    //cout << "[";
    for (int i=0; i<s; i++) {
        setBIT(0,p);
        //cout << 0;
        p.length++;
    }
    //cout << " ";
    for (int i=s; i>=0; i--) {
        uint16_t j = 0;
        j = 1 << i;
        if (j & gap){
            setBIT(1,p);
            //cout << 1;
        }
        else{
            //cout << 0;
            setBIT(0,p);
        }
        p.length++;
    }
    //cout << "]";
}

void PostingList::insert(uint16_t ID,uint32_t pos,bool isPos){
    if (isPos){
        insert(uint32_t(pos - list.back().lastPos));
        list.back().df++;
        list.back().lastPos = pos;
    }else{
        insert(uint16_t(ID - lastID));
    }
}
