#ifndef SIM_UTILS
#define SIM_UTILS

#include <string>

double getStrSimilarity(unsigned int methodID,std::string& str1,std::string& str2);

bool strsSimilarWithinLimit(unsigned int methodID,std::string& str1,std::string& str2,double limit);

#endif
