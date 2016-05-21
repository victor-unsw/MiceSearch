#include <iostream>
#include "unistd.h"
#include "inc/askMe.h"
#include "dirent.h"

#define MB 1000000

using namespace std;

std::vector<std::string>* open(string input_folder) {

    DIR*    dir;
    dirent* pdir;
    std::vector<std::string>* files = new vector<string>;

    dir = opendir(input_folder.c_str());

    while ((pdir = readdir(dir)))
        files->push_back(pdir->d_name);

    return files;
}

int main() {


    string input_folder = "/Users/victorchoudhary/Documents/Workspace/Data/Shakespear/";
    string index_file = "/Users/victorchoudhary/Documents/test.txt";
    vector<string>* files = open(input_folder);

    float limit = 5;

    clock_t it = clock();
    Indexer* indexer = new Indexer(input_folder.c_str(),index_file.c_str(),files,limit*MB);
    vector<location>* d1 = indexer->SPIMI();
    unordered_map<string,Proceeding*>* d = indexer->getDictionary(d1);

    cout << "got d1 ["<< d1->size() << "]\n";
    indexer->assertData(d,input_folder,index_file,limit);
    delete d1;
    delete indexer;

    /*Dictionary* d = indexer->directIndex();
    //d->show();
    cout << "size : " << d->getSize() << endl;

    fstream out(index_file,fstream::beg|fstream::out|fstream::binary);
    uint32_t wrote = d->flushPosting(&out);
    out.close();

    cout << "now reading\n";
    ifstream in(index_file,ios_base::beg|ios_base::in|ios_base::binary);
    uint32_t read = d->fillPosting(&in,wrote);

    delete indexer;
    it = clock() - it;
    printf("%20s : %-4.2f sec.\n","TIME TAKEN",double(it)/CLOCKS_PER_SEC);
    cin.get();
    */

    /*
    QueryEngine engine(d);

    while (true) {
        cout << "Enter you query : ";
        vector<string> queries;
        string query;
        while (cin >> query) {
            if (query == "proceed")
                break;
            queries.push_back(query);
        }
        if (query == "quit" || query == "exit")
            break;
        vector<int> id = engine.SvS(queries);
        vector<string> docs;
        for (int i : id)
            docs.push_back((*files)[(i+3)-1]);

        cout << "DOCS :-\n";
        sort(docs.begin(),docs.end());
        for (string i : docs)
            cout << i << "\n";
    }
    */

    return 0;
}