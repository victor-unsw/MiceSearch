#include <iostream>

#include "inc/askMe.h"
#include "dirent.h"


using namespace std;


int main() {

    clock_t it = clock();
    Indexer* indexer = new Indexer("/Users/victorchoudhary/Documents/Workspace/Data/Shakespear/","");
    Dictionary* d = indexer->index();
    delete indexer;
    it = clock() - it;
    printf("%15s : %-4.2f sec.\n","TIME TAKEN",double(it)/CLOCKS_PER_SEC);

    int postingSpace = d->getTotalPostingSpace();
    cout << "Total postings : " << postingSpace << endl;
    cout << "TOTAL POSTING BYTES : " << postingSpace << endl;
    cout << "MB : " << (TOTAL_POSTINGS)/(1000000*1.0) << endl;


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
        vector<int> docs = engine.SvS(queries);

        cout << "DOCS [ ";
        for (int d : docs)
            cout << d << " ";
        cout << "]\n\n";
    }

    return 0;
}