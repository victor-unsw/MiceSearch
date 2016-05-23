//
// Created by Demon on 22/05/16.
//

#ifndef BAZINGA_SEARCH_H
#define BAZINGA_SEARCH_H

#include <iostream>
#include "unistd.h"
#include "dirent.h"
#include "algorithm"
#include "global.h"

#define MB 1000000

using namespace std;

#include "../inc/askMe.h"

std::vector<std::string>* open(string input_folder) {

    DIR*    dir;
    dirent* pdir;
    std::vector<std::string>* files = new vector<string>;

    dir = opendir(input_folder.c_str());

    while ((pdir = readdir(dir))) {
        if (pdir->d_name[0] != '.') {
            files->push_back(pdir->d_name);
        }
    }
    return files;
}

Storage* str = NULL;

static int check(const void* t1,const void* t2){
    return string((char*)t1).compare(str->get(*((uint32_t*)t2)));
}

SearchInfo::SearchInfo(Information *i, string file):info(i),store(i->s),pt(i->pt),pos(i->pos),index_file(file){
    cout << "got dictionary ["<< pt->size() << "]\n";
    str = store;
}

int SearchInfo::termIndex(string s) {
    void * i = bsearch(s.c_str(),(void*)&(*pt)[0],pt->size(),sizeof(uint32_t),check);

    if (i == NULL)
        return -1;

    return *(int*)i;
}

Proceeding* SearchInfo::getProceeding(string s) {
    Proceeding* p = new Proceeding;
    int index = termIndex(s);
    int i = 0;
    for (auto it=pt->begin();it!=pt->end();it++,i++){
        if (*it == index)
            break;
    }
    uint32_t loc = (*pos)[i];

    ifstream in(index_file,ios_base::binary|ios_base::in);
    in.seekg(loc,ios_base::beg);
    p->fill(&in);
    return p;
}

bool SearchInfo::exist(string s) {
    return termIndex(s) != -1;
}

#endif //BAZINGA_SEARCH_H
