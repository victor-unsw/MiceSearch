//
// Created by Demon on 10/05/16.
//

#include "../inc/indexer.h"


Indexer::Indexer(const char *input_folder, vector<string>* f):in(NULL),files(NULL),dictionary(NULL) {
    this->input_folder  = input_folder;
    this->files = f;
}

Indexer::~Indexer() {
    input_folder    = NULL;
    dictionary      = NULL;
    files           = NULL;
}

Dictionary* Indexer::index() {
    if (!dictionary)
        dictionary = new Dictionary;

    string txt = ".txt";

    uint32_t pos    = 1;
    uint16_t i      = 1;
    for (auto it=files->begin(); it != files->end(); it++) {
        std::size_t found = (*it).find(txt);
        if (found==std::string::npos)
            continue;

        ifstream input(input_folder+*it);

        char c = 0;
        while (input.get(c)) {

            if (isspace(c))
                while (isspace(c = char(input.get())));
            if(!isalpha(c))
                continue;

            string token;
            token.push_back(char(tolower(c)));
            while (((c = input.get()) != EOF) && (isalpha(c) || (ispunct(c) && isalpha(input.peek()))))
                token.push_back(tolower(c));
            dictionary->insert(token, i,pos++);
        }

        input.close();
        i++;
        pos = 1;
    }

    return dictionary;
}