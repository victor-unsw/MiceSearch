#include <iostream>

#include "inc/askMe.h"
#include "dirent.h"


using namespace std;

std::vector<std::string>* open(string input_folder) {

    DIR*    dir;
    dirent* pdir;
    std::vector<std::string>* files = new vector<string>;

    dir = opendir(input_folder.c_str());

    while (pdir = readdir(dir)) {
        files->push_back(pdir->d_name);
    }
    return files;
}

int main() {
    string input_folder = "/Users/victorchoudhary/Documents/Workspace/Data/ShakespearDOC/";
    //string input_folder = "/Users/victorchoudhary/Documents/test/";
    vector<string>* files = open(input_folder);

    clock_t it = clock();
    Indexer* indexer = new Indexer(input_folder.c_str(),files);
    Dictionary* d = indexer->index();
    delete indexer;
    it = clock() - it;
    printf("%20s : %-4.2f sec.\n","TIME TAKEN",double(it)/CLOCKS_PER_SEC);
    //d->showDecoded();


    ofstream out("/Users/victorchoudhary/Documents/test.txt",ios_base::binary|ios_base::out);

    int postingSpace = d->getTotalPostingSpace();
    printf("%20s : %d\n","dictionary size",d->getSize());
    printf("%20s : %d\n","Total postings",postingSpace);
    printf("%20s : %-4.2f MB\n","Posting Space",(TOTAL_POSTINGS)/(1000000*1.0));

    uint32_t bytesWritten = d->flush(&out);
    printf("%20s : %d or %-5.2f KB\n","TOTAL BYTES WRITTEN",bytesWritten,(bytesWritten)/(1000*1.0)) ;
    cin.get();
    delete(d);
    out.close();
    cout << "deleted dictionary\n";

    cin.get();
    ifstream in("/Users/victorchoudhary/Documents/test.txt",ios_base::binary|ios_base::in);
    d = new Dictionary;
    uint32_t bytesRead = d->fill(&in,bytesWritten);
    printf("%20s : %d\n","dictionary size",d->getSize());
    printf("%20s : %d or %-5.2f KB\n","TOTAL BYTES READ",bytesRead,(bytesRead)/(1000*1.0)) ;



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
        for (int i : id) {
            docs.push_back((*files)[(i + 2) - 1]);
        }
        cout << "DOCS :-\n";
        sort(docs.begin(),docs.end());
        for (string i : docs)
            cout << i << "\n";
    }

    return 0;
}