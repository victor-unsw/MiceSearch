//
// Created by Demon on 22/05/16.
//
#include "../inc/search.h"
#include "locale"

using namespace std;

int main(int argc,char** argv) {


    string input_folder_global;
    string index_file_global;
    locale loc;

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
            for (int i = next; i < argc; ++i) {
                string s(argv[i]);
                for(auto it = s.begin();it!=s.end();it++)
                    *it = tolower(*it,loc);
                query.push_back(s);
            }
        }
        input_folder_global.push_back('/');
    } else{
        input_folder_global = "/Users/victorchoudhary/Documents/Workspace/Data/books200M/";
        index_file_global = "/Users/victorchoudhary/Documents/books.indx";
        //query.push_back("australian");
        //query.push_back("limited");
        query.push_back("the");
        //query.push_back("limited");
    }


    vector<string>* files = open(input_folder_global);

    uint16 limit = 0;
    uint32 total_size = 0;
    bool isXML = false;
    bool check = true;

    for (auto it=files->begin(); it != files->end(); it++) {
        if (check){
            isXML = it->find(".xml") != string::npos;
            check = false;
        }
        ifstream input(input_folder_global+*it);
        std::streampos begin,end;
        input.seekg(0,ios_base::beg);
        begin       = input.tellg();
        input.seekg(0,std::ios::end);
        end         = input.tellg();
        uint32_t FILE_SIZE   = (unsigned) (end-begin);
        total_size += FILE_SIZE;
    }

    if (isXML){
        if (total_size >= 100*MB)
            limit = 25000;
        else
            limit = 20000;
    } else{
        if (total_size >= 100*MB)
            limit = 30000;
        else
            limit = 20000;
    }


    Indexer* indexer = new Indexer(input_folder_global.c_str(),index_file_global.c_str(),files,limit);
    Information* d1 = indexer->SPIMI();

    SearchInfo searchInfo(d1,index_file_global);

    delete indexer;

    QueryEngine engine(d1,&searchInfo,input_folder_global);

    vector<string> id = engine.SvS(query);

    for (string i : id)
        cout << i << endl;

    return 0;
}