#ifndef EXPORT_CSV
#define EXPORT_CSV

#include "../data/dtos.h"
#include <vector>

void exportToCsvFromMemory(std::vector<User>& allUsers, std::vector<Movie>& allMovies);

void exportToCsvFromDB();

#endif
