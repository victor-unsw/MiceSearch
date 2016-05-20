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
#include "array"
#include "dirent.h"

struct block{
    uint16_t blockID;
    uint32_t startPos;
    uint32_t size;
    block(uint16_t b = 0,uint32_t s = 0):blockID(b),startPos(s),size(0){}
};

class Storage{
private:
    vector<char> flatDictionary;
public:
    Storage(){}

    void shrink(){
        flatDictionary.shrink_to_fit();
    }

    uint32_t getCapacity(){
        return flatDictionary.capacity();
    }
    size_t getSize(){
        return flatDictionary.size();
    }

    inline uint32_t put(const char* str,uint8_t l){
        flatDictionary.push_back(l);
        uint32_t loc = (uint32_t)(flatDictionary.size()-1);
        for (uint8_t i = 0; i < l; ++i) {
            flatDictionary.push_back(str[i]);
        }
        return loc;
    }

    inline string get(uint32_t pt){
        uint8_t length = uint8_t(flatDictionary[pt]);
        string s;
        for (uint8_t i = 1; i <= length; ++i)
            s.push_back(flatDictionary[pt+i]);
        return s;
    }

};

class location{
private:
    uint32_t         pt;
    uint32_t        pos;
public:
    location(Storage* store,const char* t,uint32_t p = 0,uint8_t l = 0):pos(p),pt(0){
        pt = store->put(t,l);
    }

    inline uint32_t getPos(){
        return pos;
    }
    inline uint32_t getPt(){
        return pt;
    }
};

class Indexer{

private:
    const uint32_t      block_size;
    const char*         input_folder;
    const char*         index_file;

    vector<string>*     files;
    vector<block>       blocks;

    Dictionary*         dictionary;


    inline uint16_t     fill(Proceeding* p,ifstream* in);

    uint32_t            merge(uint16_t i,uint16_t j);

    vector<location>*   getDictionary(uint32_t size);

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

    vector<location>* SPIMI();

};

#endif //BAZINGA_INDEXER_H
