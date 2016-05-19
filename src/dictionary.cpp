//
// Created by Demon on 10/05/16.
//

#include "../inc/dictionary.h"
//*************************************************
// CONSTRUCTOR & Destructor
//*************************************************

Dictionary::Dictionary():SIZE(0) { }
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
        //cout << term << " : inserting id :" << docID << endl;
        map.insert(std::make_pair(term,new Proceeding(term,docID,pos)));
        SIZE++;

    } else{

        // term already exists
        value->second->insert(docID,pos);
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

    for (auto i = map.begin(); i != map.end(); ++i)
        totalBytes += i->second->flush(out);

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
