//
// Created by Demon on 10/05/16.
//

#include "../inc/indexer.h"
#include "unistd.h"


Indexer::Indexer(const char *input_folder,const char* iFile, vector<string>* f, float limit):files(NULL),dictionary(NULL),block_size(uint32_t(limit)){
    this->input_folder  = input_folder;
    this->index_file    = iFile;
    this->files = f;
}

Indexer::~Indexer() {
    input_folder    = NULL;
    dictionary      = NULL;
    delete files;
    files           = NULL;
}

Dictionary* Indexer::directIndex() {
        if (!dictionary)
            dictionary = new Dictionary;

        //string txt = ".txt";

        uint32_t pos    = 1;
        uint16_t i      = 1;
        for (auto it=files->begin(); it != files->end(); it++) {
            //cout << i << " for " << *it << endl;
            //std::size_t found = (*it).find(txt);
            //if (found==std::string::npos)
              //  continue;

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

uint32_t Indexer::index(ifstream *f, uint16_t docID,uint32_t& startPos,bool& partial,bool& flushDictionary) {
    uint32_t pos = 0;
    f->seekg(startPos,ios_base::beg);

    char c = 0;


    string token;

    while (f->get(c)) {


        if(isalpha(c))
            token.push_back(char(tolower(c)));
        else if(isspace(c) || ispunct(c) || isdigit(c)){
            if (token.size() != 0){
                dictionary->insert(token, docID,pos);
                token.clear();
            }
        }


        /*
        if (isspace(c)) {
            while (isspace(c = char(f->get())));
        }

        if(!isalpha(c))
            continue;

        string token;
        token.push_back(char(tolower(c)));
        //while (((c = f->get()) != EOF) && (isalpha(c) || (ispunct(c) && isalpha(f->peek())))) {
        while(((c = f->get()) != EOF) && (isalpha(c))){// || (ispunct(c) && isalpha(f->peek())))){
            token.push_back(tolower(c));
        }

        dictionary->insert(token, docID,pos);
         */

        if(dictionary->getSize() > 30000) {
            flushDictionary = true;
            streamoff current = f->tellg();

            if (f->eof())
                f->clear();
            std::streampos begin, end;
            f->seekg(startPos, ios_base::beg);
            begin = f->tellg();
            end = f->tellg();
            uint32_t FILE_SIZE = (unsigned) (end - begin);

            if (current != FILE_SIZE)
                partial = true;

            startPos = uint32_t(current < 0 ? 0 : current);
            //cout << "chunk : " << CHUNK_SIZE << "\t read : " << read << endl;cin.get();
            return current;
        }
    }
    // update bytes_read
    if (f->eof())
        f->clear();
    std::streampos begin,end;
    f->seekg(startPos,ios_base::beg);
    begin       = f->tellg();
    f->seekg(0,std::ios::end);
    end         = f->tellg();
    uint32_t FILE_SIZE   = (unsigned) (end-begin);

    startPos = 0;

    return FILE_SIZE;
}


Information* Indexer::SPIMI() {

    ofstream out(index_file,ios_base::binary|ios_base::out);

    if (!dictionary)
        dictionary = new Dictionary;

    string txt = ".txt";
    uint32_t last       = 0;
    uint16_t i          = 1;
    uint16_t blockID    = 0;

    bool same           = false;
    uint32_t startPos   = 0;

    ifstream f;
    for (auto it=files->begin(); it != files->end(); same ? it : it++) {
        same = false;

        f.open(input_folder + *it);
        bool partial = false;
        bool flushDictionary = false;

        index(&f,i,startPos,partial,flushDictionary);

        if (flushDictionary){

            // create new dictionary

            blocks.push_back({blockID++,last});
            uint32_t bytesWriten = dictionary->flush(&out);
            blocks.back().size = bytesWriten;

            cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten+last << endl;
            last += bytesWriten;
            delete(dictionary);
            dictionary = new Dictionary;
        }

        if (!partial) {
            i++;
        } else{
            same = true;
        }

        f.close();

    }

    if (dictionary != NULL && dictionary->getSize() != 0) {
        blocks.push_back({blockID++, last});
        uint32_t bytesWriten = dictionary->flush(&out);
        blocks.back().size = bytesWriten;
        cout << "\tbytes written : " << bytesWriten << ", next : " << bytesWriten + last << endl;
        delete dictionary;
    }
    out.flush();

    std::streampos begin,end;
    out.seekp(0,ios_base::beg);
    begin       = out.tellp();
    out.seekp(0,std::ios::end);
    end         = out.tellp();
    uint32_t FILE_SIZE   = (unsigned) (end-begin);
    cout << "file size : " << FILE_SIZE << FILE_SIZE/(1000000*1.0) << "MB"<<  endl;
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
        uint32_t s1 = blocks[blockIndex].size,
                 s2 = blocks[blockIndex+1].size;

        uint32_t written = merge(blockIndex,blockIndex+1);

        blockIndex++;
    }

    Information* d = getDictionary(blocks.back().size);

    return d;
}

uint32_t Indexer::merge(uint16_t i,uint16_t j){

    bool flag = true;
    bool show = false;

    uint32_t st1    = blocks[i].startPos;
    int s1          = blocks[i].size;
    uint32_t st2    = blocks[j].startPos;
    int s2          = blocks[j].size;
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
        if (s1 <= 0 && s2 <= 0){

            uint32_t received = 0;
            uint16_t FLAG = NONE;

            while ((received = write(pb1,pb2,&o,&FLAG))){
                writenBytes += received;
                switch (FLAG){
                    case FIRST  :{pb1 = NULL; }     break;
                    case SECOND :{pb2 = NULL;}      break;
                    case BOTH   :{pb1 = pb2 = NULL;}break;
                    default:                        break;
                }
            }

            break;

        } else if (s1 <= 0){
            while (s2 > 0){
                if (pb2 == NULL){
                    pb2 = new Proceeding;
                    s2 -= pb2->fill(&b2);
                }

                uint16_t FLAG = NONE;
                writenBytes += write(pb1,pb2,&o,&FLAG);
                switch (FLAG){
                    case FIRST  :{pb1 = NULL; }     break;
                    case SECOND :{pb2 = NULL;}      break;
                    case BOTH   :{pb1 = pb2 = NULL;}break;
                    default:                        break;
                }
            }
        } else if (s2 <= 0){
            while (s1 > 0){
                if (pb1 == NULL){
                    pb1 = new Proceeding;
                    s1 -= pb1->fill(&b1);
                }
                uint16_t FLAG = NONE;
                writenBytes += write(pb1,pb2,&o,&FLAG);
                switch (FLAG){
                    case FIRST  :{pb1 = NULL;}      break;
                    case SECOND :{pb2 = NULL;}      break;
                    case BOTH   :{pb1 = pb2 = NULL;}break;
                    default:                        break;
                }
            }
        } else{
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

            uint16_t FLAG = NONE;
            writenBytes += write(pb1,pb2,&o,&FLAG);
            switch (FLAG){
                case FIRST  :{pb1 = NULL; }     break;
                case SECOND :{pb2 = NULL;}      break;
                case BOTH   :{pb1 = pb2 = NULL;}break;
                default:                        break;
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

    b2.close();

    o.seekp(writePos,fstream::beg);
    b1.seekg(st3,ios_base::beg);
    char c = 0;
    while (b1.get(c))
        o.write(&c,1);

    b1.close();
    o.flush();
    o.close();

    uint32_t finalLength = blocks.back().startPos+blocks.back().size;
    truncate(index_file,finalLength);
    return writenBytes;
}

inline uint32_t Indexer::write(Proceeding *p1, Proceeding *p2,fstream* o,uint16_t* FLAG) {
    uint32_t bytes = 0;

    if (p1 != NULL && p2 != NULL){
        int comp = p1->getTerm().compare(p2->getTerm());

        if (comp == 0){
            Proceeding* p = p1->merge(p1,p2);
            bytes        += p->flush(o);
            delete p;
            delete p1; p1 = NULL;
            delete p2; p2 = NULL;
            *FLAG = BOTH;
        } else if (comp < 0){
            bytes       += p1->flush(o);
            delete p1;p1 = NULL;
            *FLAG = FIRST;
        } else if (comp > 0){
            bytes       += p2->flush(o);
            delete p2;p2 = NULL;
            *FLAG = SECOND;
        }

    } else if (p1 != NULL){
        bytes   = p1->flush(o);
        delete  p1;
        p1 = NULL;
        *FLAG   = FIRST;

    } else if (p2 != NULL){
        bytes   = p2->flush(o);
        delete  p2;
        p2 = NULL;
        *FLAG   = SECOND;
    }

    return bytes;
}

Information* Indexer::getDictionary(uint32_t size) {

    ifstream*    input = new ifstream(index_file,ios_base::binary|ios_base::in|ios_base::beg);

    //vector<location>* dict      = new vector<location>;
    vector<uint32_t>* pt        = new vector<uint32_t>;
    vector<uint32_t>* pos       = new vector<uint32_t>;
    Storage* store              = new Storage;

    uint32_t    stringLength    = 0;
    uint32_t    bytesRead       = 0;
    uint32_t    startPos        = 0;

    while (bytesRead != size && !input->eof()) {
        Proceeding p;
        bytesRead      += p.fill(input);
        stringLength   += p.getTerm().length();
        pt->push_back(store->put(p.getTerm().c_str(),p.getTerm().length()));
        pos->push_back(startPos);
        startPos        = bytesRead;
    }

    store->shrink();
    pt->shrink_to_fit();pos->shrink_to_fit();

    Information* info = new Information(store,pt,pos);

    input->close();
    delete input;

    return info;
}


unordered_map<string,Proceeding*>* Indexer::getDictionary(vector<uint32_t> *dict,vector<uint32_t>* pos,Storage* store) {
    ifstream*    input = new ifstream(index_file,ios_base::binary|ios_base::in|ios_base::beg);
    unordered_map<string,Proceeding*>* d = new unordered_map<string,Proceeding*>;
    int j = 0;
    for (auto i = dict->begin(); i != dict->end() ; ++i,j++) {
        //cout << "-> " << store->get(i->getPt()) << "\t :: " << index++ << endl;
        Proceeding* p = new Proceeding;
        input->seekg((*pos)[j],ios_base::beg);
        p->fill(input);
        (*d)[store->get(*i)] = p;
    }
    return d;
}