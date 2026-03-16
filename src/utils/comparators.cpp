#include "comparators.h"

typedef unsigned long ulong;

bool userEntryRatingComparator(UserEntry& a,UserEntry& b){
    return a.rating > b.rating; //descending order.
}

bool movieEntryRatingComparator(MovieEntry& a,MovieEntry& b){
    return a.rating > b.rating; //descending order.
}

bool neighborUserWeightComparator(NeighborUser& a,NeighborUser& b){
    return  a.weight > b.weight; //descending order.
}

bool neighborMovieWeightComparator(NeighborMovie& a, NeighborMovie& b){
    return a.weight > b.weight; //descending order.
}

bool ulongComparator(ulong& a, ulong& b){
    return a < b; //ascending order.
}
