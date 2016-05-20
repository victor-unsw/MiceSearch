//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_INDEXER_H
#define BAZINGA_INDEXER_H

#include "dictionary.h"
#include "fstream"
#include "locale"
#include "string"
#include "ctype.h"
#include "dirent.h"

struct block{
    uint16_t blockID;
    uint32_t startPos;
    uint32_t size;
    block(uint16_t b = 0,uint32_t s = 0):blockID(b),startPos(s),size(0){}
};

class Indexer{

private:
    const uint32_t      block_size;
    const char*         input_folder;
    const char*         index_file;

    vector<string>*     files;
    vector<block>       blocks;

    ifstream*           in;
    Dictionary*         dictionary;


    inline uint16_t fill(Proceeding* p,ifstream* in);

    uint32_t merge(uint16_t i,uint16_t j);

public:
    Indexer(const char* input_folder,const char* iFile,vector<string>* f,uint32_t limit = 0);

    ~Indexer();

    /*
     * index(f,s)
     * - indexes given file from start position.
     * - returns the number of bytes read.
     * - stops when block limit exceeds.
     */
    void index(ifstream* f,uint16_t docID);

    Dictionary* directIndex();

    void SPIMI();

};

#endif //BAZINGA_INDEXER_H
