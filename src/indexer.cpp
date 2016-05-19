//
// Created by Demon on 10/05/16.
//

#include "../inc/indexer.h"


Indexer::Indexer(const char *input_folder,const char* iFile, vector<string>* f,uint32_t limit):in(NULL),files(NULL),dictionary(NULL),block_size(limit){
    this->input_folder  = input_folder;
    this->index_file    = iFile;
    this->files = f;
}

Indexer::~Indexer() {
    input_folder    = NULL;
    dictionary      = NULL;
    files           = NULL;
}

/*
 * index(f,s)
 * - indexes given file from start position.
 */
void Indexer::index(ifstream *f,uint16_t docID) {
    uint32_t pos    = 0;

    char c = 0;
    while (f->get(c)) {

        if (isspace(c))
            while (isspace(c = char(f->get())));

        if(!isalpha(c))
            continue;

        string token;
        token.push_back(char(tolower(c)));
        while (((c = f->get()) != EOF) && (isalpha(c) || (ispunct(c) && isalpha(f->peek())))) {
            token.push_back(tolower(c));
        }

        dictionary->insert(token, docID,pos);
    }
}

void Indexer::SPIMI() {

    ofstream out(index_file,ios_base::binary|ios_base::out);

    if (!dictionary)
        dictionary = new Dictionary;

    string txt = ".txt";
    uint32_t bytesRead  = 0;
    uint32_t last       = 0;
    uint16_t i          = 1;
    uint16_t blockID    = 0;

    for (auto it=files->begin(); it != files->end(); it++) {
        std::size_t found = (*it).find(txt);
        if (found == std::string::npos)
            continue;

        ifstream f(input_folder + *it);
        std::streampos begin,end;
        begin       = f.tellg();   f.seekg(0,std::ios::end);
        end         = f.tellg();   f.seekg(0,std::ios::beg);
        uint32_t FILE_SIZE   = (unsigned) (end-begin);

        if (bytesRead + FILE_SIZE > block_size){
            // new dictionary

            blocks.push_back({blockID++,last});
            uint32_t bytesWriten = dictionary->flush(&out);
            blocks.back().size = bytesWriten;
            cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten+last << endl;
            last += bytesWriten;
            delete(dictionary);
            //cin.get();
            dictionary = new Dictionary;
            bytesRead = 0;
        }

        index(&f,i);
        bytesRead += FILE_SIZE;

        //cout << "file [ " << *it << " ] number : " << i << " size : " << FILE_SIZE << " , bytes read : " << bytesRead << endl;
        i++;
        f.close();
    }

    blocks.push_back({blockID++,last});
    uint32_t bytesWriten = dictionary->flush(&out);
    blocks.back().size = bytesWriten;

    cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten+last << endl;
    delete dictionary;
    out.close();
}