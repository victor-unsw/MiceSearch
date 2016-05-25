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
#include "string.h"

#define NONE    0
#define FIRST   1
#define SECOND  2
#define BOTH    3

struct block{
    uint16_t blockID;
    uint32_t startPos;
    uint32_t size;
    block(uint16_t b = 0,uint32_t s = 0):blockID(b),startPos(s),size(0){}
};

class Storage{
private:
    //vector<char>            flatDictionary;


    char**                  blockDictionary;
    vector<vector<char>>    paddings;

    uint32_t                dictionarySize;
    uint32_t                blockSize;

public:
    Storage(uint32_t s = 500000):blockSize(s),dictionarySize(0),blockDictionary(NULL){
        blockDictionary = new char*[20];                    // initial size of 10 * blockSize
        for (int i = 0; i < 10; ++i)
            blockDictionary[i] = NULL;

        // initiate first block
        blockDictionary[0] = new char[blockSize];
        memset(blockDictionary[0],0,blockSize);

        paddings.resize(20);
    }

    inline uint32_t put(const char* str,uint8_t l){
        uint32_t result = dictionarySize;

        uint32_t targBlock  = dictionarySize/blockSize;

        uint32_t loc        = ((dictionarySize) % blockSize);

        if (blockDictionary[targBlock] == NULL){
            //cout << "creating new block " << targBlock << endl ;
            blockDictionary[targBlock] = new char[blockSize];
            memset(blockDictionary[targBlock],0,blockSize);
            loc = 0;
        }


        //cout << string(str) << " inserting at " << targBlock << " loc " << loc << " of length " << unsigned(l) <<endl;
        blockDictionary[targBlock][loc] = l;
        //cout << loc << " -> " << unsigned(blockDictionary[targBlock][loc]) << endl;
        if ((loc + l) >= blockSize){
            uint8_t i = 0;
            for (; i < l; ++i) {
                //cout << "loc " << loc+i+1;
                if (loc+i+1 >= blockSize) {
                    paddings[targBlock].push_back(str[i]);
                    //cout << "padding " << str[i] << endl;
                }
                else {
                    blockDictionary[targBlock][loc + i + 1] = str[i];
                    //cout << "inserting " << str[i] << endl;
                    dictionarySize++;
                }
            }
        } else{

            for (uint8_t i = 0; i < l; ++i) {
                blockDictionary[targBlock][loc + i + 1] = str[i];
                //cout << loc+i+1 << " -> " << blockDictionary[targBlock][loc + i + 1] << endl;
                dictionarySize++;
            }
        }


        dictionarySize++;
        //cout << "size : " << dictionarySize << endl;//cin.get();
        return result;
    }

    inline char* getCString(uint32_t pt){
        uint32_t block  = pt    /   blockSize;
        uint32_t loc    = pt    %   blockSize;

        uint8_t length = uint8_t(blockDictionary[block][loc]);
        char* value  = new char[length+1];

        if (loc + length >= blockSize){
            uint8_t i = 0;
            for (; i < length; ++i) {
                if (loc+i+1 >= blockSize) {
                    for (auto j = paddings[block].begin(); j != paddings[block].end() ; ++j)
                        value[i++] = *j;
                }
                else {
                    value[i] = blockDictionary[block][loc + i + 1];
                }
            }
        } else{
            for (uint8_t i = 1,j=0; i <= length; ++i,j++)
                value[j] = blockDictionary[block][loc+i];
        }
        value[length] = 0;
        return value;
    }

    inline string get(uint32_t pt){
        uint32_t block  = pt    /   blockSize;
        uint32_t loc    = pt    %   blockSize;

        uint8_t length = uint8_t(blockDictionary[block][loc]);
        string value;
        //cout << "loc : " << loc << "\t length : " << unsigned(length) << endl;
        if (loc + length >= blockSize){
            uint8_t i = 0;
            for (; i < length; ++i) {
                if (loc+i+1 >= blockSize) {
                    for (auto j = paddings[block].begin(); j != paddings[block].end() ; ++j,i++)
                        value.push_back(*j);
                        //value[i++] = *j;
                }
                else {
                    //cout << "loc : " << loc+i << " -> " << blockDictionary[block][loc + i] << endl;
                    value.push_back(blockDictionary[block][loc + i + 1]);
                    //value[i] = blockDictionary[block][loc + i + 1];
                }
            }
        } else{
            for (uint8_t i = 1; i <= length; ++i) {
                //cout << "loc : " << loc+i << " -> " << blockDictionary[block][loc + i] << endl;
                value.push_back(blockDictionary[block][loc + i]);
            }
        }
        return value;
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

class Information{
public:
    Storage*            s;
    vector<uint32_t>*   pt;
    vector<uint32_t>*   pos;
    Information(Storage* s1 = NULL,vector<uint32_t>*   pt2 = NULL,vector<uint32_t>*   pos2 = NULL):s(s1),pt(pt2),pos(pos2){}
    ~Information(){
        delete s;
        delete pt;
        delete pos;
    }
};

class Indexer{
private:
    const uint32_t      block_size;
    uint16_t            dictionary_limit;

    const char*         input_folder;
    const char*         index_file;

    vector<string>*     files;
    vector<block>       blocks;

    Dictionary*         dictionary;

    inline uint32_t     write(Proceeding* p1,Proceeding* p2,fstream* o,uint16_t* FLAG);
    uint32_t            merge(uint16_t i,uint16_t j);

    Information*        getDictionary(uint32_t size);

public:
    Indexer(const char* input_folder,const char* iFile,vector<string>* f, uint16_t limit = 20000);

    ~Indexer();

    /*
     * index(f,s)
     * - indexes given file from start position.
     * - returns the number of bytes read.
     * - stops when block limit exceeds.
     */
    uint32_t index(ifstream* f,uint16_t docID,uint32_t& startPos,bool& partial,bool& flushDictionary);

    Dictionary* directIndex();

    Information* SPIMI();


    //===============================================================
    // Temp methods
    //===============================================================

    void assertData(unordered_map<string,Proceeding*>* d1,string input_folder,string index_file,int limit){
        Indexer* indexer = new Indexer(input_folder.c_str(),index_file.c_str(),files,limit*1000000);
        Dictionary* d2 = indexer->directIndex();
        //cout << "got d2 ["<< d2->getSize() << "]\n";

        std::map<string,Proceeding*> ord1(d1->begin(),d1->end());
        std::map<string,Proceeding*> ord2(d2->getMap()->begin(),d2->getMap()->end());

        bool dictinaryCheck = true;
        for (auto it = ord1.begin(), jt = ord2.begin();it != ord1.end() && dictinaryCheck; it++,jt++){
            if (it->first.compare(jt->first)){
                cout << "not equal " << it->first << " and " << jt->first << endl;
                dictinaryCheck = false;
            }
        }

        if (dictinaryCheck){
            cout << "dictionary is good!\n";
        } else{
            exit(1);
        }

        for(auto it=d1->begin();it!=d1->end();it++){
            //cout << "d1 : " << it->first << "\t : " << it->second->getTerm() << endl;
            it->second->compare(it->second,d2->get(it->first));
        }

        cout << "posting is good!\n";

    }

    unordered_map<string,Proceeding*>* getDictionary(vector<uint32_t> *dict,vector<uint32_t>* pos,Storage* store);

};


#endif //BAZINGA_INDEXER_H
