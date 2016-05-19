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

    while (pdir = readdir(dir)) {
        files->push_back(pdir->d_name);
    }
    return files;
}

int main() {
    string input_folder = "/Users/victorchoudhary/Documents/Workspace/Data/books200M/";
    string index_file = "/Users/victorchoudhary/Documents/test.txt";
    vector<string>* files = open(input_folder);

    uint8_t limit = 10;

    clock_t it = clock();
    Indexer* indexer = new Indexer(input_folder.c_str(),index_file.c_str(),files,limit*MB);
    indexer->SPIMI();
    delete indexer;
    it = clock() - it;
    printf("%20s : %-4.2f sec.\n","TIME TAKEN",double(it)/CLOCKS_PER_SEC);
    cin.get();
/*
    ofstream out("/Users/victorchoudhary/Documents/test.txt",ios_base::binary|ios_base::out);
    cout << "Enter to flush dictionary : ";cin.get();
    uint32_t bytesWritten = d->flush(&out);
    out.close();

    int postingSpace = d->getTotalPostingSpace();
    printf("%20s : %d\n","dictionary size",d->getSize());
    printf("%20s : %d\n","Total postings space",postingSpace);
    printf("%20s : %d\n","Total Postings",TOTAL_POSTINGS);
    printf("%20s : %-4.2f MB\n","Posting Space",(TOTAL_POSTINGS)/(1000000*1.0));
    printf("%20s : %d or %-5.2f KB\n","TOTAL BYTES WRITTEN",bytesWritten,(bytesWritten)/(1000*1.0)) ;
    cout << "Enter to delete dictionary : ";cin.get();
    delete(d);

    cout << "Enter to fill dictionary : ";cin.get();
    ifstream in("/Users/victorchoudhary/Documents/test.txt",ios_base::binary|ios_base::in);
    std::streampos begin,end;
    begin       = in.tellg();   in.seekg(0,std::ios::end);
    end         = in.tellg();   in.seekg(0,std::ios::beg);
    size_t FILE_SIZE   = (unsigned) (end-begin);
    printf("%20s : %ld or %-5.2f KB\n","FILE SIZE ",FILE_SIZE,(FILE_SIZE)/(1000*1.0)) ;
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