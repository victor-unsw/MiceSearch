//
// Created by Demon on 10/05/16.
//

#include "../inc/indexer.h"
#include "unistd.h"


Indexer::Indexer(const char *input_folder,const char* iFile, vector<string>* f,uint32_t limit):files(NULL),dictionary(NULL),block_size(limit){
    this->input_folder  = input_folder;
    this->index_file    = iFile;
    this->files = f;
}

Indexer::~Indexer() {
    input_folder    = NULL;
    dictionary      = NULL;
    files           = NULL;
}

Dictionary* Indexer::directIndex() {
        if (!dictionary)
            dictionary = new Dictionary;

        string txt = ".txt";

        uint32_t pos    = 1;
        uint16_t i      = 1;
        for (auto it=files->begin(); it != files->end(); it++) {
            std::size_t found = (*it).find(txt);
            if (found==std::string::npos)
                continue;

            ifstream input(input_folder+*it);

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
            i++;
            pos = 1;
        }

        return dictionary;
}

/*
 * index(f,s)
 * - indexes given file from start position.
 */
void Indexer::index(ifstream *f,uint16_t docID) {
    uint32_t pos    = 0;

    char c = 0;
    while (f->get(c)) {

        if (isspace(c))
            while (isspace(c = char(f->get())));

        if(!isalpha(c))
            continue;

        string token;
        token.push_back(char(tolower(c)));
        while (((c = f->get()) != EOF) && (isalpha(c) || (ispunct(c) && isalpha(f->peek())))) {
            token.push_back(tolower(c));
        }

        dictionary->insert(token, docID,pos);
    }
}

vector<location>* Indexer::SPIMI() {

    ofstream out(index_file,ios_base::binary|ios_base::out);

    if (!dictionary)
        dictionary = new Dictionary;

    string txt = ".txt";
    uint32_t bytesRead  = 0;
    uint32_t last       = 0;
    uint16_t i          = 1;
    uint16_t blockID    = 0;

    for (auto it=files->begin(); it != files->end(); it++) {
        std::size_t found = (*it).find(txt);
        if (found == std::string::npos)
            continue;

        ifstream f(input_folder + *it);
        std::streampos begin,end;
        begin       = f.tellg();   f.seekg(0,std::ios::end);
        end         = f.tellg();   f.seekg(0,std::ios::beg);
        uint32_t FILE_SIZE   = (unsigned) (end-begin);

        if (bytesRead + FILE_SIZE > block_size){
            // new dictionary

            blocks.push_back({blockID++,last});
            uint32_t bytesWriten = dictionary->flush(&out);
            blocks.back().size = bytesWriten;
            cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten+last << endl;
            last += bytesWriten;
            delete(dictionary);
            //cin.get();
            dictionary = new Dictionary;
            bytesRead = 0;
        }

        index(&f,i);
        bytesRead += FILE_SIZE;

        i++;
        f.close();
    }
    blocks.push_back({blockID++,last});
    uint32_t bytesWriten = dictionary->flush(&out);
    blocks.back().size = bytesWriten;
    cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten+last << endl;
    delete dictionary;
    out.close();

    for (auto j = blocks.begin(); j != blocks.end() ; ++j) {
        cout << j->blockID << "  :  " << j->startPos << " : " << j->size <<endl;
    }

    // ------------------------------------------------------------------------------------------------------

    uint16_t blockIndex = 0;
    while (blocks.size() != 1){
        size_t lastIndex = blocks.size() - 1;

        if (blockIndex >= lastIndex) {
            blockIndex = 0;
            continue;
        }
        cout << "\ntotal blocks : " << blocks.size() << endl;
        //cout << "merging block " << blockIndex << " and " << blockIndex+1 << endl;
        uint32_t s1 = blocks[blockIndex].size,
                 s2 = blocks[blockIndex+1].size;

        uint32_t written = merge(blockIndex,blockIndex+1);

        //cout << s1 << " and " << s2 << " ---> " << written << endl;
        //cout << "merged block placed at index " << blockIndex << " size " << blocks[blockIndex].size << endl;
        blockIndex++;
        //cin.get();
    }


    dictionary = new Dictionary;
    ifstream b(index_file,ios_base::binary|ios_base::in);
    b.seekg(blocks[0].startPos,ios_base::beg);
    cout << "size sent : " << blocks[0].size << endl;
    std::map<string,Proceeding*>* ord = dictionary->fillOrdered(&b,blocks[0].size);
    cout << "size of dict : " << ord->size() << endl;cin.get();


    cout << "Get dictionary :- ";
    cout << "size sent : " << blocks.back().size << endl;
    vector<location>* d = getDictionary(blocks.back().size);

    return d;
}

inline uint16_t Indexer::fill(Proceeding *p, ifstream *in) {
    uint16_t bytesRead = p->fill(in);
    return bytesRead;
}

uint32_t Indexer::merge(uint16_t i,uint16_t j){
    //cout << "st1 : " << st1 << "\tst2 : " << st2 << endl;

    uint32_t st1    = blocks[i].startPos;
    int s1     = blocks[i].size;
    uint32_t st2    = blocks[j].startPos;
    int s2     = blocks[j].size;
    uint32_t st3    = blocks.back().startPos + blocks.back().size;

    fstream o(index_file,fstream::beg|fstream::in|fstream::out|fstream::binary);
    o.seekp(st3,ios_base::beg);

    ifstream b1(index_file,ios_base::binary|ios_base::in);
    b1.seekg(st1,ios_base::beg);

    ifstream b2(index_file,ios_base::binary|ios_base::in|ios_base::out);
    b2.seekg(st2,ios_base::beg);

    uint32_t writenBytes = 0;

    Proceeding* pb1 = NULL;
    Proceeding* pb2 = NULL;

    while (true){
        if (s1 <=0 && s2 <= 0){
            if (pb1 != NULL) {
                writenBytes += pb1->flush(&o);
                delete pb1;
                pb1 = NULL;
            }
            if (pb2 != NULL) {
                writenBytes += pb2->flush(&o);
                delete pb2;
                pb2 = NULL;
            }
            break;
        } else if(s1 <= 0){
            if (pb1 != NULL){
                writenBytes += pb1->flush(&o);
                delete(pb1);
                pb1 = NULL;
            }
            while (s2 > 0){
                if (pb2 != NULL){
                    writenBytes += pb2->flush(&o);
                    delete pb2;
                    pb2 = NULL;
                }
                pb2 = new Proceeding;
                s2 -= pb2->fill(&b2);
                writenBytes += pb2->flush(&o);
                pb2 = NULL;
                delete pb2;
            }
            //cout << "done s2 too : " << s2 << endl;cin.get();
        } else if(s2 <= 0){
            if (pb2 != NULL){
                writenBytes += pb2->flush(&o);
                delete pb2;
                pb2 = NULL;
            }
            while (s1 > 0){
                if (pb1 != NULL){
                    //cout << "pb1 already active\n";
                    writenBytes += pb1->flush(&o);
                    delete pb1;
                    pb1 = NULL;
                }
                pb1 = new Proceeding;
                s1 -= pb1->fill(&b1);
                writenBytes += pb1->flush(&o);
                delete pb1;
                pb1 = NULL;
            }
        } else{
            // none is empty

            // first we make sure both proceedings are filled
            if (!pb1 && !pb2){

                pb1 = new Proceeding;
                s1 -= pb1->fill(&b1);

                pb2 = new Proceeding;
                s2 -= pb2->fill(&b2);

            } else if(!pb1){
                pb1 = new Proceeding;
                s1 -= pb1->fill(&b1);
            } else if(!pb2){
                pb2 = new Proceeding;
                s2 -= pb2->fill(&b2);
            }

            int comp = pb1->getTerm().compare(pb2->getTerm());

            if (!comp){
                Proceeding* p = pb1->merge(pb1,pb2);
                writenBytes += p->flush(&o);
                delete p;
                delete pb1;
                delete pb2;
                pb1 = NULL;
                pb2 = NULL;
            } else if(comp < 0){
                writenBytes += pb1->flush(&o);
                delete pb1;
                pb1 = NULL;
            } else{
                writenBytes += pb2->flush(&o);
                delete pb2;
                pb2 = NULL;
            }
        }
    }

    o.flush();
    uint32_t writePos = 0;

    if (i != 0)
        writePos = blocks[i-1].startPos + blocks[i-1].size;

    blocks[i].startPos  = writePos;
    blocks[i].size      = writenBytes;
    blocks.erase(blocks.begin()+j);


    o.seekp(writePos,fstream::beg);
    b1.seekg(st3,ios_base::beg);
    char c = 0;
    while (b1.get(c)){
        o.write(&c,1);
    }

    uint32_t finalLength = blocks.back().startPos+blocks.back().size;
    //cout << "truncating till length : " << finalLength << endl;
    truncate(index_file,finalLength);

    //cin.get();
    return writenBytes;
}

vector<location>* Indexer::getDictionary(uint32_t size) {

    fstream    input(index_file,fstream::binary|fstream::in|fstream::out);

    vector<location>* dict  = new vector<location>;
    Storage* store          = new Storage;

    uint32_t    stringLength    = 0;
    uint32_t    bytesRead       = 0;
    uint32_t    startPos        = 0;
    uint32_t    added           = 0;

    while (bytesRead != size && !input.eof()) {
        Proceeding p;
        bytesRead      += p.fill(&input);
        //stringLength   += p.getTerm().length();
        //location l(store,p.getTerm().c_str(),startPos,uint8_t(p.getTerm().length()));
        //dict->push_back(l);
        //startPos        = bytesRead;
        added++;
    }
    cout << "bytes read : " << bytesRead << "\t size : " << size << endl;
    if (input.eof()){
        cout << "yes end of file\n";
    }
    store->shrink();

    /*
    cout << "bytesRead  : " << bytesRead << endl;
    cout << "bytesWrote : " << bytesWrote << endl;
    cout << "dict size  : " << (dict->size() * sizeof(location)) << " BYTES , " << (dict->size() * sizeof(location))/(1000000*1.0) << "MB.";
    cout << "string length : " << stringLength << endl;
    cout << "total space : " << ((store->getSize())+(dict->size() * 8))/(1000000*1.0) << " MB. \n";
    cout << "total space ::" << ((store->getCapacity())+(dict->size() * 8))/(1000000*1.0) << " MB. \n";
     */

    //for(auto it=dict->begin();it!=dict->end();it++){
    //    cout << store->get(it->getPt()) << endl;
    //}
    cout << "dictonary [" << dict->size() << "]" << endl;
    cout << "total added : " << added << endl;
    cin.get();
    delete store;
    cin.get();
    return dict;
}