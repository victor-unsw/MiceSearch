//
// Created by Demon on 22/05/16.
//
#include "../inc/search.h"

using namespace std;

int main(int argc,char** argv) {
    string input_folder_global;
    string index_file_global;

    vector<string> query;
    if (true) {
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
        input_folder_global = "/Users/victorchoudhary/Documents/Workspace/Data/simple/";
        index_file_global = "/Users/victorchoudhary/Documents/test.txt";
    }

    vector<string>* files = open(input_folder_global);

    float limit = 6;

    clock_t it = clock();
    Indexer* indexer = new Indexer(input_folder_global.c_str(),index_file_global.c_str(),files,limit*MB);
    Information* d1 = indexer->SPIMI();

    SearchInfo searchInfo(d1,index_file_global);

    //unordered_map<string,Proceeding*>* d = indexer->getDictionary(d1->pt,d1->pos,d1->s);
    //indexer->assertData(d,input_folder,index_file,limit);

    it = clock() - it;
    printf("%20s : %-4.2f sec.\n","TIME TAKEN",double(it)/CLOCKS_PER_SEC);


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
    cin.get();
    */


    QueryEngine engine(d1,&searchInfo,input_folder_global);

    vector<string> id = engine.SvS(query);

    vector<string> docs;
    for (string id : docs) {
        cout << id << endl;
    }

    return 0;
}