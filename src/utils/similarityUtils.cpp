#include "similarityUtils.h"
#include "../exceptions/recommenderExceptions.h"
#include "functionalityNames.h"

#include <set>
using namespace std;

//Sørensen–Dice coefficient//
//note: identical bigrams that exist multiple times in a word are taken into account only once.
double getSorensenDiceCoefficient(string& string1,string& string2) {

    set<string> string1_bigrams;
    set<string> string2_bigrams;

    //base case
    if(string1.length() == 0 || string2.length() == 0)
    {
            return 0;
    }

     // extract character bigrams from string1
    for(size_t i = 0; i < (string1.length() - 1); i++) {
            string1_bigrams.insert(string1.substr(i, 2));
    }

    // extract character bigrams from string2
    for(size_t i = 0; i < (string2.length() - 1); i++) {
            string2_bigrams.insert(string2.substr(i, 2));
    }

    unsigned int intersection = 0;

    // find the intersection between the two sets

    for(set<string>::iterator IT = string2_bigrams.begin();
        IT != string2_bigrams.end();
        IT++)
    {
            intersection += string1_bigrams.count((*IT));
    }

    // calculate dice coefficient
    size_t total = string1_bigrams.size() + string2_bigrams.size();
    double dice = static_cast<double>(intersection * 2) / static_cast<double>(total);

    return dice;
}

//Cosine similarity, with vectors of trigrams//
double getCosineSimilarityForStr(string& string1,string& string2){

    //silence warnings for unused parameters.
    (void)string1;
    (void)string2;
    //

    throw NotYetImplementedException(FunctionalityNames::COS_SIMILARITY);
}

//assuming that both strings are both lowercase or both uppercase.
double getStrSimilarity(unsigned int methodID,string& string1, string& string2)
{
    //methodID:: 1->Sørensen–Dice , 2->Cosine similarity(TODO)
    switch(methodID){
        case 1:
            return getSorensenDiceCoefficient(string1,string2);
        case 2:
            return getCosineSimilarityForStr(string1,string2);
        default:
            return 0;
    }
}

//assuming that both strings are both lowercase or both uppercase.
bool strsSimilarWithinLimit(unsigned int methodID,string& string1,string& string2,double limit){

    if(limit > 1){
        throw InvalidStateException("Attempted to perform string similarity with invalid parameters. "
                                    "'limit' parameter cannot be higher than 1");
    }

    //if strings are identical, skip the similarity matching altogether.
    if(string1 == string2){
        return true;
    }
    double similarityMetric = getStrSimilarity(methodID,string1,string2);
    return similarityMetric >= limit;
}
