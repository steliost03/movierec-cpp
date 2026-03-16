/*
Copyright (C) 2018 Stylianos Tsiakalos
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef HEADER_ALL
#define HEADER_ALL

#include <iostream>
#include <string>
using namespace std;

#include <ctime>

#include "csvparsing/csvparsing.h"
#include "csvparsing/stringutils/stringUtils.h"
#include "csvparsing/multithread/csvparsing_parallel.h"

#include "mem_management/checkedAllocationFunctions.h"
#include "mem_management/checkedReallocationFunctions.h"
#include "mem_management/cleanupFunctions.h"

#include "arrayutils/arrayCopyFunctions.h"
#include "arrayutils/searchArrayFunctions.h"

#include "arrayutils_par/parallelSearchArrayFunctions.h"

#include "multithreadcommons/multithreadcommons.h"

//#include "boostutils/boostUtils.h"

#include "timeutils/timeUtils.h"

#endif
