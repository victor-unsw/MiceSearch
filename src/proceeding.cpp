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
    delete [] term;
    delete  postings;
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
    uint16_t    totalPostingSize = 0;

    totalBytes += 2;                                        // 'total_length'
    totalBytes += 1;                                        // 'term length'
    totalBytes += termLength;                               // bytes taken by each character in term
    totalBytes += 4;                                        // 'tf' is 32 bit a.k.a 4 byte
    totalBytes += 2;                                        // PostingList 'GLOBAL_SIZE' 2 byte


    vector<Posting>* list = postings->getList();
    for(auto it=list->begin();it!=list->end();it++){
        totalPostingSize += 4;
        totalPostingSize += 2;
        totalPostingSize += it->code.size();
        //totalBytes += 4;                                    // df
        //totalBytes += 2;                                    // list variable size
        //totalBytes += it->code.size();                      // list size
    }
    totalBytes += totalPostingSize;

    //cout << "writting : " << totalBytes << "at : " << out->tellp() << endl;
    out->write((char*)&totalBytes,sizeof(totalBytes));
    //cout << "writting : " << unsigned(termLength) << "at : " << out->tellp() << endl;
    out->write((char*)&termLength,sizeof(termLength));
    //cout << "writting : " << getTerm() << "at : " << out->tellp() << endl;
    out->write(getTerm().c_str(),termLength);
    //cout << "writting : " << tf << "at : " << out->tellp() << endl;cin.get();
    out->write((char*)&tf,sizeof(tf));
    //cout << "writting : " << totalPostingSize << "at : " << out->tellp() << endl;
    out->write((char*)&totalPostingSize,sizeof(totalPostingSize));

    for(auto it=list->begin();it!=list->end();it++){
        //cout << "written df : " << it->df << " at : " << out->tellp() << endl;
        out->write((char*)&it->df,sizeof(it->df));

        //cout << "written size : " << it->code.size() << " at : " << out->tellp() << endl;
        uint16_t s = uint16_t(it->code.size());
        out->write((char*)&s,sizeof(s));

        //cout << "writting code at : " << out->tellp() << endl;
        vector<uint8_t>* v = &(it->code);
        out->write((char*)&((*v)[0]),s);
    }

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
    uint16_t    totalPostingLength = 0;
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
    in->read((char*)&totalPostingLength,sizeof(totalPostingLength));
    bytesRemaining -= sizeof(totalPostingLength);
    //cout << "length : " << totalPostingLength <<"\t remain : " << bytesRemaining << endl;cin.get();

    if (bytesRemaining != totalPostingLength){
        cout << "not equal bytes remaining : " << bytesRemaining << " : " << totalPostingLength << endl;
        cin.get();
    }

    // initialize posting list
    if (!postings)
        postings = new PostingList;

    vector<Posting>* v = postings->getList();
    //cout << "bytes remaining : " << bytesRemaining << endl;
    while (bytesRemaining > 0){
        Posting p;
        bytesRemaining -= fillPosting(in,&p);
        v->push_back(p);
        //cout << "bytes remaining : " << bytesRemaining << endl;
    }

    return totalBytes;
}

uint16_t Proceeding::fillPosting(ifstream *in,Posting* p) {
    uint16_t totalBytes = 0;
    uint32_t df = 0;
    uint16_t l  = 0;

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&df,sizeof(df));
    totalBytes += sizeof(df);
    //cout << "df : " << df ;cin.get();

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)&l,sizeof(l));
    totalBytes += sizeof(l);
    //cout << "l size : " << l ;cin.get();

    uint8_t* temp = new uint8_t[l];
    memset(temp,0,l);

    //cout << "cp : " << in->tellg() << endl;
    in->read((char*)temp,l);
    for (int i = 0; i < l; ++i) {
        p->code.push_back(temp[i]);
    }
    totalBytes += l;
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
