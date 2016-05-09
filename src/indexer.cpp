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

    string  TEMP_FILE = "/Users/victorchoudhary/Documents/Workspace/Data/Shakespear/DOC";
    int     TEMP_TOTAL= 54;

    uint32_t pos = 0;

    for (uint16_t i=1; i<=TEMP_TOTAL; i++) {
        string file(TEMP_FILE+to_string(i)+".txt");
        ifstream input(file);

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
    }

    cout << "total terms inserted : " << dictionary->getSize() << endl;

    return dictionary;
}