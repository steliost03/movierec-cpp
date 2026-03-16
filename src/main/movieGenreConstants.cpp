#ifndef MOV_GENRE_CONSTANTS
#define MOV_GENRE_CONSTANTS

#include "movieGenres.h"
#include <vector>

const MovieGenre MovieGenre::ACTION = MovieGenre("Action");
const MovieGenre MovieGenre::ADVENTURE = MovieGenre("Adventure");
const MovieGenre MovieGenre::ANIMATION = MovieGenre("Animation");
const MovieGenre MovieGenre::CHILDREN = MovieGenre("Children");
const MovieGenre MovieGenre::COMEDY = MovieGenre("Comedy");
const MovieGenre MovieGenre::CRIME = MovieGenre("Crime");
const MovieGenre MovieGenre::DOCUMENTARY = MovieGenre("Documentary");
const MovieGenre MovieGenre::DRAMA = MovieGenre("Drama");
const MovieGenre MovieGenre::FANTASY = MovieGenre("Fantasy");
const MovieGenre MovieGenre::NOIR = MovieGenre("Film - Noir");
const MovieGenre MovieGenre::HORROR = MovieGenre("Horror");
const MovieGenre MovieGenre::MUSICAL = MovieGenre("Musical");
const MovieGenre MovieGenre::MYSTERY = MovieGenre("Mystery");
const MovieGenre MovieGenre::ROMANCE = MovieGenre("Romance");
const MovieGenre MovieGenre::SCI_FI = MovieGenre("Sci - Fi");
const MovieGenre MovieGenre::THRILLER = MovieGenre("Thriller");
const MovieGenre MovieGenre::WAR = MovieGenre("War");
const MovieGenre MovieGenre::WESTERN = MovieGenre("Western");

const std::vector<std::string> validGenresStrVec = {MovieGenre::ACTION.str,MovieGenre::ADVENTURE.str
                                                        ,MovieGenre::ANIMATION.str,MovieGenre::CHILDREN.str,
                                                        MovieGenre::COMEDY.str,MovieGenre::CRIME.str,
                                                        MovieGenre::DOCUMENTARY.str,MovieGenre::DRAMA.str,
                                                        MovieGenre::FANTASY.str,MovieGenre::NOIR.str,
                                                        MovieGenre::HORROR.str,MovieGenre::MUSICAL.str,
                                                        MovieGenre::MYSTERY.str,MovieGenre::ROMANCE.str,
                                                        MovieGenre::SCI_FI.str,MovieGenre::THRILLER.str,
                                                        MovieGenre::WAR.str,MovieGenre::WESTERN.str};

#endif
