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
    vector<Posting*>*  l = p->getPostingList()->getList();

    for (; i<query.size(); i++) {
        if (!dictionary->exist(query[i]))
            continue;

        p = dictionary->get(query[i]);
        l = intersect(l, p->getPostingList()->getList());
    }

    for (int i=0; i<l->size(); i++) {
        Posting* p = (*l)[i];
        r.push_back(p->getDocID());
    }

    return r;

}

vector<Posting*>* QueryEngine::intersect(vector<Posting*> *l1, vector<Posting*> *l2) {
    vector<Posting*>* result = new vector<Posting*>;
    uint16_t    s1 = uint16_t(l1->size()),
                s2 = uint16_t(l2->size());

    int p1 = 0,p2 = 0;
    Posting* pt_1 = NULL;                            // pointer to l1
    Posting* pt_2 = NULL;                            // pointer to l2

    while (s1 && s2) {
        pt_1 = (*l1)[p1];
        pt_2 = (*l2)[p2];

        if (pt_1->getDocID() == pt_2->getDocID()) {                 // if intersect

            result->push_back(pt_1);                                // add result
            s1--;s2--;                                              // decrement both counters
            p1++;p2++;                                              // increment both pointers

        }else if (pt_1->getDocID() < pt_2->getDocID()){

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