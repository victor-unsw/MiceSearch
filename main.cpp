#include <iostream>

#include "inc/askMe.h"

using namespace std;

int main() {

    Indexer indexer("","");
    Dictionary* d = indexer.index();

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