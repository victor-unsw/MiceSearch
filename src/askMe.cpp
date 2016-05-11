//
// Created by Demon on 10/05/16.
//

#include "../inc/askMe.h"


vector<int> QueryEngine::SvS(vector<string> query) {
    int i=0;
    vector<int> r;

    while (i < query.size() && !dictionary->exist(query[i]))
        i++;

    if (i == query.size())
        return r;

    Proceeding*   p = dictionary->get(query[i]);
    vector<uint8_t>* temp = p->getPostingList()->getList();
    vector<uint16_t>*  l = decode(temp);

    for (; i<query.size(); i++) {
        if (!dictionary->exist(query[i]))
            continue;

        p = dictionary->get(query[i]);
        temp = p->getPostingList()->getList();
        l = intersect(l, decode(temp));
    }

    for (int i=0; i<l->size(); i++) {
        uint16_t p = (*l)[i];
        r.push_back(p);
    }

    return r;

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