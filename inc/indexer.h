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
    const char*     input_folder;
    const char*     output_file;

    ifstream*       in;
    ofstream*       out;
    Dictionary*     dictionary;

    std::vector<std::string> open();

public:
    Indexer(const char* input_folder,const char* output_file);
    ~Indexer();

    Dictionary* index();


};

#endif //BAZINGA_INDEXER_H
