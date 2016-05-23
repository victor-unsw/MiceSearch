//
// Created by Demon on 22/05/16.
//
#include "../inc/search.h"

using namespace std;

int main(int argc,char** argv) {
    string input_folder_global;
    string index_file_global;

    vector<string> query;
    if (argc > 3) {
        if (argc < 4) {
            cout << "wrong input parameters\n";
            exit(1);
        } else {
            input_folder_global = argv[1];
            index_file_global = argv[2];
            int next = 3;
            if (!strcmp(argv[next], "-s"))
                next = 5;
            for (int i = next; i < argc; ++i)
                query.push_back(argv[i]);
        }
    } else{
        input_folder_global = "/Users/victorchoudhary/Documents/Workspace/Data/books200M/";
        index_file_global = "/Users/victorchoudhary/Documents/test.txt";
        query.push_back("god");
    }

    vector<string>* files = open(input_folder_global);

    float limit = 6;

    Indexer* indexer = new Indexer(input_folder_global.c_str(),index_file_global.c_str(),files,limit*MB);
    Information* d1 = indexer->SPIMI();

    SearchInfo searchInfo(d1,index_file_global);

    //unordered_map<string,Proceeding*>* d = indexer->getDictionary(d1->pt,d1->pos,d1->s);
    //indexer->assertData(d,input_folder,index_file,limit);

    delete indexer;

    QueryEngine engine(d1,&searchInfo,input_folder_global);

    vector<string> id = engine.SvS(query);

    for (string i : id)
        cout << i << endl;

    delete d1;
    return 0;
}