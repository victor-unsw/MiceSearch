//
// Created by Demon on 09/05/16.
//

#include "../inc/proceeding.h"

//*************************************************
// CONSTRUCTOR & Destructor
//*************************************************

Proceeding::Proceeding():term(NULL),length(0),tf(0),postings(NULL) { }

Proceeding::Proceeding(string t,uint16_t docID,uint32_t pos):term(NULL),length(0),tf(0),postings(NULL){
    if (t.size() > 256 || t.size() <= 0){
        cout << "ERROR [ t length ] : " << t.size() << endl;
        exit(1);
    }

    // initialize the term credentials
    initializeTerm(uint8_t(t.size()));
    strncpy(this->term,t.c_str(),length);

    // insert the document ID & position
    insert(docID,pos);
}

Proceeding::~Proceeding() {
    delete postings;
}


//*************************************************
//  INITIALIZE_TERM(size)
// -sets length to size.
// -arranges memory for term.
//*************************************************
void Proceeding::initializeTerm(uint8_t size) {
    this->length    = size;
    this->term      = new char[size];
    memset(term,0,size);
}

//*************************************************
//  INSERT(id,pos)
// -inserts the docID with position into the posting list.
// -increments the total frequency count
//*************************************************
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


//*************************************************
//  FLUSH(ostream)
// -sends the complete proceeding to the output stream.
// -returns total length of bytes sent.
//*************************************************
uint16_t Proceeding::flush(ofstream *out) {
    uint16_t    totalBytes  = 0;
    uint8_t     termLength  = getTermLength();
    uint16_t    postingSize = postings->getListSize();
    totalBytes += 2;                                        // 'total_length'
    totalBytes += 1;                                        // 'term length'
    totalBytes += termLength;                               // bytes taken by each character in term
    totalBytes += 4;                                        // 'tf' is 32 bit a.k.a 4 byte
    totalBytes += 2;                                        // PostingList 'SIZE' byte
    totalBytes += postingSize;                              // 'list' bytes

    //cout << "writting : " << totalBytes << "at : " << out->tellp() << endl;
    out->write((char*)&totalBytes,sizeof(totalBytes));
    //cout << "writting : " << termLength << "at : " << out->tellp() << endl;
    out->write((char*)&termLength,sizeof(termLength));
    //cout << "writting : " << term << "at : " << out->tellp() << endl;
    out->write(term,termLength);
    //cout << "writting : " << tf << "at : " << out->tellp() << endl;cin.get();
    out->write((char*)&tf,sizeof(tf));
    //cout << "writting : " << postingSize << "at : " << out->tellp() << endl;
    out->write((char*)&postingSize,sizeof(postingSize));

    //cout << "writting size : " << postingSize << " at : " << out->tellp() << endl;
    vector<uint8_t>* v = postings->getList();
    out->write((char*)&((*v)[0]),v->size());
    //cout << "done at : " << out->tellp() << endl;
    //cin.get();
    return totalBytes;
}


//*************************************************
//  FILL(istream)
// -fills the proceeding from given position in
//  input stream
// -returns total length of bytes read.
//*************************************************
uint16_t Proceeding::fill(ifstream *in) {
    uint16_t    totalBytes = 0;
    uint8_t     termLength = 0;
    uint16_t    postingLength = 0;
    uint16_t    bytesRemaining = 0;

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&totalBytes,sizeof(totalBytes));               // total Bytes to be read
    bytesRemaining = uint16_t(totalBytes - sizeof(totalBytes));
    //cout << "total bytes : " << totalBytes << "\t remain : " << bytesRemaining<<endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&termLength, sizeof(termLength));
    bytesRemaining -= sizeof(termLength);
    //cout << "term length : " << unsigned(termLength) << "\t remain : " << bytesRemaining<< endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    initializeTerm(termLength);
    in->read(this->term,termLength);
    bytesRemaining -= termLength;
    //cout << "term : " << getTerm() <<"\t remain : " << bytesRemaining << endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&tf,sizeof(tf));
    bytesRemaining -= sizeof(tf);
    //cout << "tf : " << tf <<"\t remain : " << bytesRemaining << endl;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&postingLength,sizeof(postingLength));
    bytesRemaining -= sizeof(postingLength);
    //cout << "length : " << postingLength <<"\t remain : " << bytesRemaining << endl;cin.get();

    if (bytesRemaining != postingLength){
        cout << "not equal bytes remaining : " << bytesRemaining << " : " << postingLength << endl;
        //cin.get();
    }

    if (!postings)
        postings = new PostingList;
    vector<uint8_t>* v = postings->getList();
    uint8_t* temp = new uint8_t[postingLength];
    memset(temp,0,postingLength);

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)temp,postingLength);
    for (int i = 0; i < postingLength; ++i) {
        v->push_back(temp[i]);
    }
    //cout << "cp : " << in->tellg() << endl;
    //cin.get();
    return totalBytes;
}


//*************************************************
// UTILITY METHODS
//*************************************************

inline void Proceeding::incrementTF() {
    tf++;
}
