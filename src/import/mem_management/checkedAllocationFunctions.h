#ifndef CHECKEDALLOC_H
#define CHECKEDALLOC_H

#include <cstdlib>
#include <iostream>


inline void errorMessageAndTermination(const char* message){

	std::cerr << "- - -\n";
	std::cerr << "An error occured while allocating memory for csv parsing.\n";
	std::cerr << "ERROR : ";
	std::cerr << message;
	std::cerr << "- - -\n";
	std::cerr << "\nThe program will now terminate.";
	std::cerr << "\n";
	std::cerr << "Press Enter to continue\n";
	std::cin.get();
	exit(0);

}
	

template <typename T>
void checkedAllocation(T** array,size_t size,bool initialize = false){
  
  if (size == 0){
	errorMessageAndTermination("invalid 1D size input on memory allocation.");
  }

  if(initialize)
	(*array) = (T*)calloc(size,sizeof(T));
  else
    (*array) = (T*)malloc(size*sizeof(T));

  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 1D memory allocation");
  }

}

template <typename T>
void checkedAllocation_2D(T*** array,size_t dim1,size_t dim2,bool initialize = false){

  if (dim1 == 0){
	  errorMessageAndTermination("invalid first dimension size input on 2D memory allocation.");
  }
  if (dim2 == 0){
	  errorMessageAndTermination("invalid second dimension size input on 2D memory allocation.");
  }

  (*array) = (T**)malloc(dim1*sizeof(T*));
  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 2D memory allocation");
  }
  for(size_t i=0;i<dim1;i++)
    checkedAllocation<T>(&((*array)[i]),dim2,initialize);
}

template <typename T>
void checkedAllocation_3D(T**** array,size_t dim1,size_t dim2,size_t dim3,bool initialize = false){

  if (dim1 == 0){
	  errorMessageAndTermination("invalid first dimension size input on 3D memory allocation.");
  }
  if (dim2 == 0){
	  errorMessageAndTermination("invalid second dimension size input on 3D memory allocation.");
  }
  if (dim3 == 0){
	  errorMessageAndTermination("invalid third dimension size input on 3D memory allocation.");
  }

  (*array) = (T***)malloc(dim1*sizeof(T**));
  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 3D memory allocation");
  }
  for(size_t i=0;i<dim1;i++)
    checkedAllocation_2D<T>(&((*array)[i]),dim2,dim3,initialize);

}

#endif
