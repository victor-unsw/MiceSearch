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


uint16_t PostingList::fill(ifstream *in) {
    uint16_t    totalBytes = 0;
    uint16_t    postingLength = 0;
    uint16_t    frequencyCount = 0;
    uint16_t    bytesRemaining = 0;

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&totalBytes,sizeof(totalBytes));               // total Bytes to be read
    bytesRemaining = uint16_t(totalBytes - sizeof(totalBytes));
    //cout << "total bytes : " << totalBytes << "\t remain : " << bytesRemaining<<endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&postingLength,sizeof(postingLength));
    bytesRemaining -= sizeof(postingLength);
    //cout << "length : " << postingLength <<"\t remain : " << bytesRemaining << endl;cin.get();

    uint8_t* temp = new uint8_t[postingLength];
    memset(temp,0,postingLength);
    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)temp,postingLength);

    for (int i = 0; i < postingLength; ++i) {
        list.push_back(temp[i]);
    }
    delete [] temp;
    bytesRemaining -= postingLength;
    //cout << "read posting list of size : "<< list.size() << "\t remain : " << bytesRemaining << endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&frequencyCount,sizeof(frequencyCount));
    bytesRemaining -= sizeof(frequencyCount);
    //cout << "freq list count : " << frequencyCount <<"\t remain : " << bytesRemaining << endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    uint16_t * freqList = new uint16_t[frequencyCount];
    memset(freqList,0,size_t(frequencyCount*2));
    in->read((char*)freqList,frequencyCount*2);

    for (uint16_t j = 0; j < frequencyCount; ++j)
        freq.push_back(freqList[j]);
    delete [] freqList;
    bytesRemaining -= frequencyCount*2;
    //cout << "read freq list of size : "<< freq.size() << "\t remain : " << bytesRemaining << endl;cin.get();

    if (bytesRemaining != 0){
        cout << "not equal bytes remaining : " << bytesRemaining << " : " << postingLength << endl;
        //cin.get();
    }

    //cin.get();
    return totalBytes;
}

uint16_t PostingList::getInitialCost() {

    uint16_t SPACE = sizeof(postingCount);
    SPACE += sizeof(SIZE);
    SPACE += sizeof(list);
    SPACE += sizeof(freq);
    SPACE += sizeof(lastID);

}

//*************************************************
// INSERT Method
// - this is the major method; where eventually
// - the documentID is added to the list.
//*************************************************
uint16_t PostingList::insert(uint16_t docID, uint32_t pos) {
    uint16_t SPACE = 0;

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

        SPACE += 1;     //list; 1 byte (approx)
        SPACE += 2;     //freq; 2 byte
    }

    return SPACE;
}


PostingList* PostingList::merge(PostingList *p1, PostingList *p2) {
    PostingList* p  = new PostingList;

    vector<uint16_t>* l1 = decodePosting(p1->getList());
    vector<uint16_t>* l2 = decodePosting(p2->getList());

    bool common     = l1->back() == l2->front();                    // common doc. id found at end & front

    for (auto i = l1->begin(); i != l1->end() ; ++i)
        p->insert(*i,0);
    for (auto i = l2->begin() + (common?1:0); i != l2->end() ; ++i)
        p->insert(*i,0);

    p->getFreqList()->erase(p->freq.begin(),p->freq.end());
    for (auto i = p1->freq.begin();i != p1->freq.end(); i++)
        p->freq.push_back(*i);
    if (common)
        p->freq.back() += p2->freq.front();
    for (auto i = p2->freq.begin() + (common?1:0); i != p2->freq.end(); i++)
        p->freq.push_back(*i);

    delete l1;
    delete l2;

    return p;
}


/*
PostingList* PostingList::merge(PostingList *p1, PostingList *p2) {

    PostingList* p = new PostingList;

    vector<uint16_t>* l1 = decodePosting(p1->getList());
    vector<uint16_t>* l2 = decodePosting(p2->getList());

    for (auto i = l1->begin(); i != l1->end() ; ++i)
        p->insert(*i,0);
    for (auto i = l2->begin(); i != l2->end() ; ++i)
        p->insert(*i,0);

    p->getFreqList()->erase(p->freq.begin(),p->freq.end());
    for (auto i = p1->freq.begin(); i != p1->freq.end() ; ++i)
        p->freq.push_back(*i);

    if (p1->getList()->back() == p2->getList()->front()) {
        cout << "found equal " << unsigned(p1->getList()->back()) << " and " << unsigned(p2->getList()->front()) << endl;cin.get();
        p->freq.back() += p2->freq.front();
        for (auto i = p2->freq.begin()+1; i != p2->freq.end() ; ++i)
            p->freq.push_back(*i);
    } else{
        for (auto i = p2->freq.begin(); i != p2->freq.end(); ++i)
            p->freq.push_back(*i);
    }

    delete l1;
    delete l2;
    return p;
}*/

vector<uint16_t>* PostingList::decodePosting(vector<uint8_t> *l) {

    vector<uint16_t>* result = new vector<uint16_t>;

    int     bitPosition     = 7;
    bool    readSelector    = true;

    uint8_t selector        = 0;
    uint16_t body           = 0;

    for (auto it=l->begin(); it!=l->end(); it++) {

        for (int j=bitPosition; j >= 0; j--) {
            int test = 0;test |= 1<< j;
            int bit = 0;
            if (*it & test)
                bit = 1;

            if (readSelector) {
                if (bit){
                    readSelector = false;
                    selector++;
                    j++;        // increase bit position
                }else{
                    selector++;
                }
            }else{
                if (bit) {
                    body |= 1 << (selector-1);
                    selector--;
                }else{
                    selector--;
                }
                if (!selector) {
                    readSelector    = true;
                    result->size() == 0 ? result->push_back(body) : result->push_back(result->back()+body);
                    body            = 0;
                }
            }
        }

    }
    return result;
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