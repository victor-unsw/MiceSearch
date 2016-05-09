//
// Created by Demon on 10/05/16.
//

#include "../inc/indexer.h"


Indexer::Indexer(const char *input_folder, const char *output_file):in(NULL),out(NULL),dictionary(NULL) {
    this->input_folder  = input_folder;
    this->output_file   = output_file;
}


Dictionary* Indexer::index() {
    if (!dictionary)
        dictionary = new Dictionary;

    std::vector<std::string> f;

    f = open();
    string txt = ".txt";

    uint32_t pos    = 0;
    uint16_t i      = 0;
    for (auto it=f.begin(); it != f.end(); it++) {
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
    }

    cout << "total terms inserted : " << dictionary->getSize() << endl;

    return dictionary;
}


std::vector<std::string> Indexer::open() {

    DIR*    dir;
    dirent* pdir;
    std::vector<std::string> files;

    dir = opendir(input_folder);

    while (pdir = readdir(dir)) {
        files.push_back(pdir->d_name);
    }
    return files;
}