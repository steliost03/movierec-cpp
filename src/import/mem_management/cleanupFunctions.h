#ifndef CLEANUP_FUNCTIONS_H
#define CLEANUP_FUNCTIONS_H

#include <iostream>
#include <cstdlib>
#include <cstdio>

template <typename T>
void arrcleanup(T** array) {

  free(*array);
  *array = NULL;
 
}

template <typename T>
void arrcleanup_2D(T*** array,size_t dim){


   for(size_t i=0;i<dim;i++) {
     arrcleanup<T>(&((*array)[i]));
   }

   free(*array);
   *array = NULL;
}

template <typename T>
void arrcleanup_3D(T**** array,size_t dim1,size_t dim2){
	
  for(size_t i=0;i<dim1;i++)
    arrcleanup_2D<T>(&((*array)[i]),dim2);

  free(*array);
  *array = NULL;

}

inline void filecleanup(FILE** file){
	
  int r = fclose(*file);

  if(r == EOF)
    std::cout<<"WARNING: file failed to close at runtime."<<std::endl;
}

#endif
