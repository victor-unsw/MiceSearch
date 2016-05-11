//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_INDEXER_H
#define BAZINGA_INDEXER_H

#include "dictionary.h"
#include "fstream"
#include "locale"
#include "ctype.h"
#include "dirent.h"

class Indexer{

private:
    const char*         input_folder;
    vector<string>*     files;

    ifstream*       in;
    Dictionary*     dictionary;

public:
    Indexer(const char* input_folder,vector<string>*    f);
    ~Indexer();

    Dictionary* index();


};

#endif //BAZINGA_INDEXER_H
