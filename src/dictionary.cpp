//
// Created by Demon on 10/05/16.
//

#include "../inc/dictionary.h"
//*************************************************
// CONSTRUCTOR & Destructor
//*************************************************

Dictionary::Dictionary():SIZE(0),SPACE(0),totalInsertions(0) {
    SPACE   += sizeof(SIZE);
    SPACE   += sizeof(SPACE);
    SPACE   += sizeof(map);
}
Dictionary::~Dictionary(){
    for (auto i = map.begin(); i != map.end() ; ++i)
        delete i->second;
}


//*************************************************
//  INSERT(term)
// -inserts the term into the dictionary.
// -if proceeding already exist, then append the
//  docID to it.
// -else create a new instance of proceeding.
//*************************************************
void Dictionary::insert(const string& term,uint16_t docID,uint32_t pos) {
    unordered_map<string,Proceeding*>::iterator value = map.find(term);
    if (value == map.end()){

        // new term inserted
        Proceeding* p = new Proceeding(term,docID,pos);
        SPACE += p->getInitialCost();
        SPACE += p->getPostingList()->getInitialCost();
        SPACE += 2+1;
        map.insert(std::make_pair(term,p));
        SIZE++;

    } else{

        // term already exists
        totalInsertions++;
        SPACE += value->second->insert(docID,pos);
    }
}


//*************************************************
//  Get(term)
// -returns the proceeding for given term.
//*************************************************
Proceeding* Dictionary::get(const string term) {
    unordered_map<string, Proceeding *>::iterator value = map.find(term);

    if (value == map.end()) {
        cout << "returning NULL for proceeding";
        return NULL;
    }

    return value->second;
}


//*************************************************
//  EXIST(term)
// -returns false if term doesn't exist.
//*************************************************
bool Dictionary::exist(const string term) {
    return map.find(term) != map.end();
}


//*************************************************
//  FLUSH(ostream)
// -sends the complete dictionary to the output stream.
// -returns total length of bytes sent.
//*************************************************
uint32_t Dictionary::flush(ofstream *out) {
    uint32_t totalBytes = 0;

    // sort the keys
    std::map<string,Proceeding*> ordered(map.begin(),map.end());

    for (auto i = ordered.begin(); i != ordered.end(); ++i) {
        totalBytes += i->second->flush(out);
    }

    return totalBytes;
}


//*************************************************
//  FILL(istream)
// -fills the dictionary from given position in
//  input stream
// -returns total length of bytes read.
//*************************************************
uint32_t Dictionary::fill(ifstream *in,uint32_t size) {
    uint32_t totalBytes = 0;

    while (totalBytes != size && !in->eof()) {
        Proceeding *p = new Proceeding;
        totalBytes += p->fill(in);
        map.insert(std::make_pair(p->getTerm(),p));
    }
    return totalBytes;
}

std::map<string,Proceeding*> * Dictionary::fillOrdered(ifstream *in, uint32_t size) {
    std::map<string,Proceeding*>* ordered = new std::map<string,Proceeding*>;
    uint32_t totalBytes = 0;
    uint16_t totalInsertions = 0;
    uint32_t filled = 0;
    while (totalBytes != size && !in->eof()) {
        Proceeding *p = new Proceeding;
        //cout << "- before " << in->tellg() << endl;
        totalBytes += p->fill(in);
        //cout << p->getTerm() << endl;
        //cout << "- after  " << in->tellg() << endl;
        ordered->insert(std::make_pair(p->getTerm(),p));
        totalInsertions++;
        filled++;
    }
    //cout << "filled [ " << filled << " ] \n";
    return ordered;
}