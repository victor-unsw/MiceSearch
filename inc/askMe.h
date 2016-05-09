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

    vector<Posting*>* intersect(vector<Posting*> *l1, vector<Posting*> *l2);

};


#endif //BAZINGA_ASKME_H
