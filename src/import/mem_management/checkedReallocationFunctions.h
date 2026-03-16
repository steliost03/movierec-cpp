#ifndef CHECKEDREALLOC_H
#define CHECKEDREALLOC_H

#include "checkedAllocationFunctions.h"

template <typename T>
void checkedReallocation(T** array,size_t size){
  
  (*array) = (T*)realloc((*array),size*sizeof(T));
  
  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 1D memory reallocation");
  }
}

template <typename T>
void checkedReallocation_2D(T*** array,size_t dim1,size_t dim2,size_t old_dim1,bool initialize = false){
	
  (*array) = (T**)realloc((*array),dim1*sizeof(T*));
  
  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 2D memory reallocation");
  }
  
  for(size_t i=0;i<dim1;i++){
	
	  if (i < old_dim1){
		  checkedReallocation<T>(&((*array)[i]), dim2);
	  }
	else{
	
		checkedAllocation<T>(&((*array)[i]), dim2, initialize);
	}
	}
}

template <typename T>
void checkedReallocation_3D(T**** array,size_t dim1,size_t dim2,size_t dim3,size_t old_dim1,size_t old_dim2,bool initialize=false){
	
  (*array) = (T***)realloc((*array),dim1*sizeof(T**));
 	
  if((*array) == NULL){
    errorMessageAndTermination("array NULL after 3D memory reallocation");
  }
  
  for(size_t i=0;i<dim1;i++){
	if(i<old_dim1)
		checkedReallocation_2D<T>(&((*array)[i]),dim2,dim3,old_dim2);
	else
		checkedAllocation_2D<T>(&((*array)[i]),dim2,dim3,initialize);
	}

}

#endif
