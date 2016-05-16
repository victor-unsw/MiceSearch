//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_DICTIONARY_H
#define BAZINGA_DICTIONARY_H

#include "proceeding.h"
#include "unordered_map"
#include "map"
#include "string"

class Dictionary{

private:
    uint32_t                                        SIZE;
    std::unordered_map<std::string,Proceeding*>     map;


public:
    Dictionary();
    ~Dictionary();

    inline uint32_t getSize(){
        return map.size();
    }

    //===============================================================
    // MUTATOR Methods
    //===============================================================

    /*
     * insert(term).
     * - inserts the term into the dictionary.
     * - if proceeding already exist, then appends the docID to it.
     * - else creates a new instance of proceeding.
     */
    void insert(const string& term,uint16_t docID,uint32_t pos);

    /*
     * get(term).
     * - returns the proceeding for given term.
     */
    Proceeding* get(const string term);

    /*
     * returns false if term doesn't exist.
     */
    bool exist(const string term);

    uint32_t flush(ofstream* out);

    uint32_t fill(ifstream* in,uint32_t size);

    //===============================================================
    // Temp methods
    //===============================================================

    void show(){
        std::map<string,Proceeding*> ordered(map.begin(),map.end());
        auto it = ordered.begin();
        for (int i = 0; i < 100; ++i) {
            cout << it->first << endl;
            it++;
        }
    }

    int getTotalFrequency(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getTf();
        }
        return total;
    }

    int getTotalPostingSpace(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getPostingList()->getList()->size();
        }
        return total;
    }

    int totalTermSpace(){
        int total = 0;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            total += i->second->getTerm().length();
        }
        return total;
    }

    vector<Proceeding*> getAllProceeding(){
        vector<Proceeding*> p;
        cout << "returning p of size \t" << map.size() << endl;
        for(auto it=map.begin();it!=map.end();it++)
            p.push_back(it->second);
        return p;
    }

    vector<uint32_t> decodePosting(vector<uint8_t>& postings,uint16_t lastDOC){

        vector<uint32_t> decodeList;

        int     bitPosition     = 7;
        bool    readSelector    = true;

        uint8_t selector        = 0;
        uint32_t body           = 0;
        uint16_t index          = 0;
        // first decode document ID
        for (auto it=postings.begin(); it!=postings.end(); it++) {
            bool breakOutLoop = false;
            for (int j=bitPosition; j >= 0; j--) {
                int test = 0;test |= 1<< j;
                int bit = 0;
                if (*it & test)
                    bit = 1;

                if (readSelector) {
                    if (bit){
                        readSelector = false;
                        selector++;
                        //cout << "selector : " << unsigned(selector) << endl;
                        j++;        // increase bit position
                    }else{
                        //cout << "read 0 at bit pos " << j << endl;
                        selector++;
                    }
                }else{
                    if (bit) {
                        //cout << "marking 1 at " << unsigned(selector-1) << "bit, read at pos : " << j << "\n";
                        body |= 1 << (selector-1);
                        selector--;
                    }else{
                        //cout << "marking 0 at " << unsigned(selector) << "bit, read at pos : " << j << "\n";
                        selector--;
                    }
                    //cout << "s: " << unsigned(selector) << endl;
                    if (!selector) {
                        readSelector    = true;
                        uint32_t temp = lastDOC == 0 ? body : lastDOC+body;
                        //lastDOC == 0 ? decodeList.push_back(body) : decodeList.push_back(lastDOC+body);
                        decodeList.push_back(temp);
                        //cout << "DOC ID entered : " << temp << endl;
                        //decode.size() == 0 ? decode.push_back(body) : decode.push_back(decode.back()+body);
                        body            = 0;
                        breakOutLoop    = true;
                        break;
                    }
                }
                bitPosition--;
            }
            if (breakOutLoop)
                break;

            index++;
        }
        //cout << "index : " << index << "\t bit position : " << bitPosition << endl;cin.get();

        uint32_t lastPos = 0;
        // now decode position list
        for (auto it=postings.begin()+index; it!=postings.end(); it++) {

            for (int j=bitPosition; j >= 0; j--) {
                int test = 0;test |= 1<< j;
                int bit = 0;
                if (*it & test)
                    bit = 1;

                if (readSelector) {
                    if (bit){
                        readSelector = false;
                        selector++;
                        //cout << "selector : " << unsigned(selector) << endl;cin.get();
                        j++;        // increase bit position
                    }else{
                        //cout << "0 at bit pos " << j << endl;
                        selector++;
                    }
                }else{
                    if (bit) {
                        //cout << "marking 1 at " << unsigned(selector-1) << "bit, read at pos : " << j << "\n";
                        body |= 1 << (selector-1);
                        selector--;
                    }else{
                        //cout << "marking 0 at " << unsigned(selector) << "bit, read at pos : " << j << "\n";
                        selector--;
                    }
                    //cout << "s: " << unsigned(selector) << endl;
                    if (!selector) {
                        readSelector    = true;
                        //cout << "gap : " << body << endl;
                        uint32_t temp = lastPos == 0 ? body : lastPos + body;
                        //cout << "entering : " << temp << ", last pos : " << lastPos << endl;
                        //cout << "index : " << index << "\t bit position : " << bitPosition << endl;cin.get();
                        decodeList.push_back(temp);
                        lastPos = temp;
                        body            = 0;
                        //decode.size() == 0 ? decode.push_back(body) : decode.push_back(decode.back()+body);
                    }
                }
                //bitPosition--;
                //if (bitPosition < 0) {
                //    bitPosition = 7;
                //}
            }
            bitPosition = 7;
        }
        //cout << "returning size : " << decodeList.size() << endl; cin.get();
        return decodeList;
    }

    void  showDecoded(){
        uint16_t last = 0;
        vector<vector<uint32_t>> outputList;
        for (auto i = map.begin(); i != map.end() ; ++i) {
            vector<Posting>*list = i->second->getPostingList()->getList();
            printf("%20s :",i->second->getTerm().c_str());
            for (auto j = list->begin(); j != list->end() ; ++j) {
                vector<uint32_t> l = decodePosting(j->code,last);
                outputList.push_back(l);
                last = l[0];
                int z = 0;
                for (auto k = l.begin(); k != l.end(); ++k,z++) {
                    if (z == 0){
                        cout << "["<<*k<<"] ";
                    } else
                        cout << *k << " ";
                }
            }
            last = 0;
            cout << endl;
        }
    }

};

#endif //BAZINGA_DICTIONARY_H
