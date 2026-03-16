#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <cmath>

using namespace std;

inline bool realNumEqualWithinError(double x1,double x2,double errorMargin) {
    if(fabs(x1-x2) <= errorMargin){
        return true;
    } else{
        return false;
    }
}

//unsigned types only.
inline size_t min(size_t x1 , size_t x2){

	return (x1 >= x2 ? x2 : x1);
}

//(x + constant)^2
inline double calculateSquaredSum(double* values, size_t valueCount, double constant){

    double sum = 0;

    for (size_t i = 0; i < valueCount; i++){
		sum = sum + (values[i] + constant)*(values[i] + constant);
	}

    return sum;
}


//mfactors*(x - constants)^2
inline double calculateSquaredSum2(double* values, size_t valueCount, double* constants,double* mfactors){
	
    double sum = 0;
	
    for(size_t i=0;i<valueCount;i++){
        sum = sum + mfactors[i]*(values[i]-constants[i])*(values[i] - constants[i]);
    }

    return sum;
}

//mfactors*(x + constant)^2
inline double calculateSquaredSum3(double* values, size_t valueCount, double constant,double* mfactors){

    double sum = 0;

    for (size_t i = 0; i < valueCount; i++){
        sum = sum + mfactors[i]*(values[i] + constant)*(values[i] + constant);
    }

    return sum;
}

//sum(x*y)
inline double calculateSumMultiplicative(double* values1,double* values2,size_t valueCount){
    double sum = 0;
    for(size_t i=0;i<valueCount;i++){
        sum = sum + values1[i]*values2[i];
    }
    return sum;
}

//sum(|x|)
inline double calculateSumOfAbsolutes(double* values,size_t valueCount){
    double sum = 0;
    for(size_t i=0;i<valueCount;i++){
        sum = sum + fabs(values[i]);
    }
    return sum;
}

//mfactors*(x - constant1)*(y-constant2)
inline double calculateComplexSum(double* values1, double* values2, size_t valueCount, double constant1, double constant2,double* multiplicationFactors){

    double sum = 0;

    for (size_t i = 0; i < valueCount; i++){
        sum = sum + multiplicationFactors[i]*(values1[i] - constant1)*(values2[i] - constant2);
    }

    return sum;
}


//mfactors*(x - constants)*(y - constants)
inline double calculateComplexSum2(double* values1 , double* values2, size_t valueCount, double* constants, double* multiplicationFactors){

    double sum = 0;
	
    for(size_t i=0;i<valueCount;i++){
        sum = sum + multiplicationFactors[i] * (values1[i] - constants[i])*(values2[i] - constants[i]);
    }
	
    return sum;
}

inline double calculateMean(double* values, size_t valueCount){

    if(valueCount == 0){
        return 0;
    }

    double sum = 0;
    for (size_t i = 0; i < valueCount; i++){
		sum = sum + values[i];
	}
	return sum / double(valueCount);
}

inline double calculateNewMean(double oldMean,size_t oldTotalValueCount, double newValuesSum , size_t newValueCount){
	
	double newMean = (oldTotalValueCount*oldMean + newValuesSum) / (oldTotalValueCount+newValueCount);
	
	return newMean;
}


//TODO: population or sample standard deviation? currently using: population standard deviation.
inline double calculateStandardDeviation(double* values,size_t valueCount){

    if(valueCount == 0){
        return 0;
    }

    double mean = calculateMean(values,valueCount);
    double inner = calculateSquaredSum(values,valueCount,-mean);
    //inner = inner / static_cast<double>(valueCount - 1); //sample.
    inner = inner / static_cast<double>(valueCount); //population.
    double squareRoot = sqrt(inner);
    return squareRoot;
}

//deprecated?
inline double calculateZScoreNormalization(double targetValue, double* allValues, unsigned long valueCount){

	return (targetValue - calculateMean(allValues,valueCount)) / calculateStandardDeviation(allValues,valueCount);
}


//frequency-weighted pearson corellation
inline double calculatePearsonCorrelation(double* values1, double* values2, size_t valueCount, double valuesMean1, double valuesMean2,double* mfactors){

    if(valueCount == 0){
        return 0;
    }
    double numerator = calculateComplexSum(values1,values2,valueCount,valuesMean1,valuesMean2,mfactors);
    double denominator_inner = calculateSquaredSum3(values1, valueCount, -valuesMean1,mfactors)*calculateSquaredSum3(values2, valueCount, -valuesMean2,mfactors);
    double denominator = sqrt(denominator_inner);

    if(realNumEqualWithinError(denominator,0,0.000001)){
        return 0;
    }

    return numerator / denominator;
}

inline double calculateAdjustedCosineSimilarity(double* values1, double* values2, size_t valueCount , double* valuesMeans , double* mfactors){
	
    if(valueCount == 0){
        return 0;
    }

    double numerator = calculateComplexSum2(values1,values2,valueCount,valuesMeans,mfactors); //TODO
    double denominator_inner = calculateSquaredSum2(values1,valueCount,valuesMeans,mfactors)*calculateSquaredSum2(values2,valueCount,valuesMeans,mfactors);
    double denominator = sqrt(denominator_inner);

    if(denominator == 0){
        return 0;
    }
	
    return numerator / denominator;
}


inline double calculateIDF(size_t totalDocumentCount, size_t targetDocumentCount){

    //log(totalDocumentCount/targetDocumentCount)

    double inner = double(totalDocumentCount) / double(targetDocumentCount);

    return log10(inner);
}

inline double modifyCorrelationBySignificance(double initialCorrelation,unsigned long significanceFactor,size_t size) {
	
    size_t numerator = min(static_cast<size_t>(significanceFactor),size);
    unsigned long denominator = significanceFactor;
	
    double fraction = static_cast<double>(numerator)/static_cast<double>(denominator);
	
    double result = fraction*initialCorrelation;
	
    return result;
}
#endif
