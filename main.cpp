#include <iostream>
#include "unistd.h"
#include "inc/askMe.h"
#include "dirent.h"

#define MB 1000000

#include "algorithm"

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

Storage* store;

int check(const void* t1,const void* t2){
    //cout << "checking now " << store->get(*((uint32_t*)t1)) << " and " << store->get(*((uint32_t*)t2));
    return string((char*)t1).compare(store->get(*((uint32_t*)t2)));
}

int main() {

    string input_folder = "/Users/victorchoudhary/Documents/Workspace/Data/ShakespearDOC/";
    string index_file = "/Users/victorchoudhary/Documents/test.txt";
    vector<string>* files = open(input_folder);

    float limit = 2;

    clock_t it = clock();
    Indexer* indexer = new Indexer(input_folder.c_str(),index_file.c_str(),files,limit*MB);
    vector<location>* d1 = indexer->SPIMI();
    store = indexer->store;

    cout << "store test : " << store->get((*d1)[2].getPt()) << endl;
    uint32_t ptArr[d1->size()];

    int index = 0;
    for (auto it=d1->begin();it!=d1->end();it++)
        ptArr[index++] = it->getPt();

    cout << "got d1 ["<< d1->size() << "]\n";

    //indexer->assertData(d,input_folder,index_file,limit);

    string s = "a'mercy";
    int * i = (int*)bsearch(s.c_str(),(void*)&ptArr,d1->size(),sizeof(uint32_t),check);
    cout << "index : " << *i << " : " << store->get(*i) << endl;

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