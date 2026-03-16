#ifndef MOV_GENRES
#define MOV_GENRES

#include <string>
#include <vector>

class MovieGenre{

    public:

            static const MovieGenre ACTION;
            static const MovieGenre ADVENTURE;
            static const MovieGenre ANIMATION;
            static const MovieGenre CHILDREN;
            static const MovieGenre COMEDY;
            static const MovieGenre CRIME;
            static const MovieGenre DOCUMENTARY;
            static const MovieGenre DRAMA;
            static const MovieGenre FANTASY;
            static const MovieGenre NOIR;
            static const MovieGenre HORROR;
            static const MovieGenre MUSICAL;
            static const MovieGenre MYSTERY;
            static const MovieGenre ROMANCE;
            static const MovieGenre SCI_FI;
            static const MovieGenre THRILLER;
            static const MovieGenre WAR;
            static const MovieGenre WESTERN;

            std::string str;

            MovieGenre(){
                str = "UNDEFINED";
            }

            MovieGenre(std::string genre) {
                this->str = genre;
            }

            bool equals(MovieGenre& other){
                return this->str == other.str;
            }

            bool equals(std::string& str){
                return this->str == str;
            }
};

extern const std::vector<std::string> validGenresStrVec;

#endif
