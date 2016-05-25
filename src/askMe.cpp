//
// Created by Demon on 10/05/16.
//

#include <sstream>
#include "../inc/askMe.h"
#include "string.h"

void QueryEngine::updateDocumentID(vector<uint16_t> *source, vector<uint16_t> *target) {

    for(auto it=source->begin();it!=source->end();it++)
        target->push_back(*it);

    std::sort(target->begin(),target->end());

    target->erase(unique(target->begin(),target->end()),target->end());
}

//
// simply increments the freq.
// using given proceeding data and doc ID
void QueryEngine::incrementFrequency(pair<Proceeding*,vector<uint16_t>*> list, double_t *freq) {
    vector<uint16_t>* pList = list.second;
    for (auto i = pList->begin(); i != pList->end() ; ++i)
        freq[*i] += list.first->getPostingList()->getFreq(*i);
}

void QueryEngine::incrementFrequency(Proceeding *p, vector<uint16_t>* r, double_t * freq) {
    for(auto it=r->begin();it!=r->end();it++)
        freq[*it] += p->getPostingList()->getFreq(*it);
}

vector<int> QueryEngine::getSortedID(vector<int> id, double_t *freq) {
    vector<std::pair<int,double_t>> pairs;
    for (auto i = id.begin(); i != id.end(); ++i)
        pairs.push_back(make_pair(*i,freq[*i]));

    std::sort(pairs.begin(),pairs.end(),pairCompare);

    vector<int> result;
    for (auto j = pairs.begin(); j != pairs.end(); ++j)
        result.push_back(j->first);

    return result;
}

vector<string> QueryEngine::filesSorted(vector<pair<string,int>> files,vector<int> id, double_t *freq) {
    vector<string> results;
    vector<std::pair<double_t,string>> pairs;

    for (auto i = files.begin(); i != files.end(); ++i)
        pairs.push_back(make_pair(freq[i->second], i->first));

    std::sort(pairs.begin(),pairs.end(),pairStringCompare);

    vector<string> temp;
    double_t last = 0;
    for (auto j = pairs.begin(); j != pairs.end(); ++j) {
        if (j->first != last){
            sort(temp.begin(),temp.end());
            for (auto it = temp.begin(); it != temp.end() ; ++it) {
                results.push_back(*it);
            }
            temp.erase(temp.begin(),temp.end());
        }
        temp.push_back(j->second);
        last = j->first;
    }

    sort(temp.begin(),temp.end());
    for (auto it = temp.begin(); it != temp.end() ; ++it) {
        results.push_back(*it);
    }

    return results;
}

vector<string> QueryEngine::SvS(vector<string> query) {

    int i=0;
    vector<int> r;
    double_t* freq = new double_t[2001];

    for (int j = 0; j < 2001; ++j)
        freq[j] = 0;


    bool phraseQuery = false;
    /*string phrase;

    if (query.size() == 1 & query[0].find(" ") != string::npos) {
        phraseQuery = true;
        phrase = query[0];
        istringstream iss(query[0]);
        copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter(query));
        query.erase(query.begin());
    }*/

    // STEP 1
    // - fill in data for each query term
    // - proceedings, & decoded docIDs

    unordered_map<string,pair<Proceeding*,vector<uint16_t>*>> master;

    for(int it=0;it<query.size();it++) {
        string term = query[it];
        if (searchInfo->exist(term)) {
            master[query[it]] = make_pair(searchInfo->getProceeding(term),
                                          decode(searchInfo->getProceeding(term)->getPostingList()->getList()));
        }
        else
            master[query[it]] = make_pair(new Proceeding,new vector<uint16_t>);
    }

    // increment frequencies from master
    for(auto it=master.begin();it!=master.end();it++)
        incrementFrequency(it->second,freq);

    // delete all proceeding data from master
    for(auto it=master.begin();it!=master.end();it++)
        delete it->second.first;

    // STEP 2
    // - get substring for each query term

    vector<string> substrings;

    bool        more        = true;
    uint32_t    start       = 0;
    uint32_t    increment   = 4000;

    //cout << "starting getting substring data";cin.get();

    while(more && !phraseQuery) {
        vector<char *> *subject = searchInfo->getShortTerms(start, start + increment, more, increment);
        start += increment;

        for (auto m = subject->begin(); m != subject->end(); ++m) {

            for (auto q = query.begin(); q != query.end(); ++q) {

                if ((strstr(*m, q->c_str()) != NULL) && strcmp(*m, q->c_str())) {
                    Proceeding *mP = searchInfo->getProceeding(*m);
                    vector<uint16_t> *docIDs = decode(mP->getPostingList()->getList());
                    incrementFrequency(make_pair(mP, docIDs), freq);

                    updateDocumentID(docIDs, master[*q].second);
                    substrings.push_back(*m);
                    delete mP;
                    delete docIDs;
                }
            }
            delete[] *m;
        }
        delete(subject);
    }

    //cout << "got substring data";cin.get();


    for (auto q = substrings.begin(); q != substrings.end(); ++q) {
        query.push_back(*q);
    }


    while (i < query.size() && !searchInfo->exist(query[i])) {
        i++;
    }

    delete searchInfo->info;


    // STEP 3
    // - intersection of results

    unordered_map<string,pair<Proceeding*,vector<uint16_t>*>>::iterator it = master.begin();

    vector<uint16_t>*  l = it->second.second;

    bool first = true;
    for (; it!=master.end(); it++) {
        l = intersect(l, it->second.second);
    }

    for (int i=0; i<l->size(); i++) {
        uint16_t p = (*l)[i];
        r.push_back(p);
    }


    DIR*    dir;
    dirent* pdir;
    std::vector<std::string>* files = new vector<string>;

    dir = opendir(inputFolder.c_str());
    while ((pdir = readdir(dir))) {
        if (pdir->d_name[0] != '.') {
            files->push_back(pdir->d_name);
        }
    }

    /*
    cout << "finding phrase : " << phrase << endl;
    vector<uint16_t>*  pL = new vector<uint16_t>;
    for (int j = 0; j < 2001; ++j)
        freq[j] = 0;

    ifstream f;
    locale loc;

    for (int i=0; i<r.size(); i++) {

        f.open(inputFolder+(*files)[r[i]-1],ios_base::binary|ios_base::in);
        f.seekg (0, std::ios::end);
        long length = f.tellg();
        f.seekg (0, std::ios::beg);

        char buffer[length];

        f.read (&buffer[0],length);
        string str(buffer);
        for(auto it = str.begin();it!=str.end();it++)
            *it = tolower(*it,loc);
        int count = countPattern(str,phrase);
        if (count > 0){
            freq[r[i]] += count;
            pL->push_back(r[i]);
        }
        f.close();

        //cout << "done with file " << (*files)[r[i]-1];
        //cout << "count : " << count << endl;
    }

    l = intersect(l, pL);
    r.erase(r.begin(),r.end());
    for (int i=0; i<l->size(); i++) {
        uint16_t p = (*l)[i];
        r.push_back(p);
    }*/

    vector<pair<string,int>> results;
    for (int i = 0; i < r.size(); ++i) {
        results.push_back(make_pair((*files)[r[i]-1], r[i]));
    }

    vector<string> output = filesSorted(results,r,freq);

    //r = getSortedID(r,freq);

    for (auto k = master.begin(); k != master.end() ; ++k) {
        delete(k->second.second);
    }


    delete files;
    delete [] freq;
    return output;
}

vector<uint16_t>* QueryEngine::intersect(vector<uint16_t> *l1, vector<uint16_t> *l2) {
    vector<uint16_t>* result = new vector<uint16_t>;
    uint16_t    s1 = uint16_t(l1->size()),
                s2 = uint16_t(l2->size());

    int p1 = 0,p2 = 0;
    uint16_t pt_1 = 0;                            // pointer to l1
    uint16_t pt_2 = 0;                            // pointer to l2

    while (s1 && s2) {
        pt_1 = (*l1)[p1];
        pt_2 = (*l2)[p2];

        if (pt_1 == pt_2) {                 // if intersect

            result->push_back(pt_1);                                // add result
            s1--;s2--;                                              // decrement both counters
            p1++;p2++;                                              // increment both pointers

        }else if (pt_1 < pt_2){

            p1++;                                                   // if l1 element is smaller
            s1--;                                                   // then shift the pointer to l1

        }
        else {

            p2++;                                                   // other shift the l2 pointer
            s2--;

        }
    }

    return result;
}

vector<uint16_t>* QueryEngine::decode(vector<uint8_t> *l){
    vector<uint16_t>* result = new vector<uint16_t>;

    int     bitPosition     = 7;
    bool    readSelector    = true;

    uint8_t selector        = 0;
    uint16_t body           = 0;

    for (auto it=l->begin(); it!=l->end(); it++) {

        for (int j=bitPosition; j >= 0; j--) {
            int test = 0;test |= 1<< j;
            int bit = 0;
            if (*it & test)
                bit = 1;

            if (readSelector) {
                if (bit){
                    readSelector = false;
                    selector++;
                    j++;        // increase bit position
                }else{
                    selector++;
                }
            }else{
                if (bit) {
                    body |= 1 << (selector-1);
                    selector--;
                }else{
                    selector--;
                }
                if (!selector) {
                    readSelector    = true;
                    result->size() == 0 ? result->push_back(body) : result->push_back(result->back()+body);
                    body            = 0;
                }
            }
        }

    }
    return result;
}

int QueryEngine::countPattern(const std::string &str, const std::string &sub) {
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
         offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}