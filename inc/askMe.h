//
// Created by Demon on 10/05/16.
//

#ifndef BAZINGA_ASKME_H
#define BAZINGA_ASKME_H

#include "indexer.h"

class QueryEngine{

private:
    Dictionary  *   dictionary;

public:
    QueryEngine(Dictionary *d = NULL):dictionary(d) { }

    vector<int> SvS(vector<string> queryTerms);

    vector<uint16_t>* intersect(vector<uint16_t> *l1, vector<uint16_t> *l2);

    vector<uint16_t>* decode(vector<uint8_t> *l);

    vector<uint16_t>* getDocIDList(vector<Posting>* list);

    inline uint16_t decodePostingDOCID(vector<uint8_t>& postings,uint16_t lastDOC);

};


#endif //BAZINGA_ASKME_H
