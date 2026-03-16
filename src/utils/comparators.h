#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "../data/dtos.h"

bool userEntryRatingComparator(UserEntry&,UserEntry&);
bool movieEntryRatingComparator(MovieEntry&,MovieEntry&);
bool neighborUserWeightComparator(NeighborUser&,NeighborUser&);
bool neighborMovieWeightComparator(NeighborMovie&,NeighborMovie&);
bool ulongComparator(unsigned long&,unsigned long&);

#endif
