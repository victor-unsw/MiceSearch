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

    const char*         input_folder;
    const char*         index_file;

    vector<string>*     files;
    vector<block>       blocks;

    Dictionary*         dictionary;

    inline uint32_t     write(Proceeding* p1,Proceeding* p2,fstream* o,uint16_t* FLAG);
    uint32_t            merge(uint16_t i,uint16_t j);

    Information*        getDictionary(uint32_t size);

public:
    Indexer(const char* input_folder,const char* iFile,vector<string>* f, float limit = 0);

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
        cout << "got d2 ["<< d2->getSize() << "]\n";

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
