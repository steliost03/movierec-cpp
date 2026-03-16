#include "importCsv.h"
#include "../../logging/recommenderLogger.h"
#include "../../utils/utils.h"
#include <cstdio>
using namespace std;

MatrixSizesWrapper loadFromFiles(Configuration* config,string moviesFilePath,string ratingsFilePath,string tagsFilePath,string genomeTagsFilePath,
                   float*** userEntries,char**** movies,char*** tags,char**** genreEntries,ElapsedTimes& elapsedTimes,bool terminateOnError){

    CustomSteadyTimer parseTimer;
    CustomSteadyTimer parsePostProcessingTimer;

    parseTimer.markStartPoint();

    RecommenderLogger LOG;
    LOG.init("ImportCsv:LoadFromFiles");
    LOG.info("START: loadFromFiles.");

    //temporary matrix
    char*** temp_tags;

    //
    file moviesfile;
    file ratingsfile;
    file tagsfile; //tag entries file.
    file genometagsfile; //tag ids file.

    bool tagIdsFileIsUsed = (genomeTagsFilePath != "0");
    if(tagIdsFileIsUsed){
        LOG.debug("Tag ids file is enabled.");
        cout<<"Tag ids file is enabled."<<endl;
    }else{
        LOG.debug("No tag ids file will be used");
        cout<<"No tag ids file will be used."<<endl;
    }

    //
    LOG.debug("Processing input files.");
    cout<<"-processing files..."<<endl;
    LOG.trace("Processing movies file: "+moviesFilePath);
    bool moviesFileSuccess = openAndProcessFile(moviesFilePath.c_str(),moviesfile,terminateOnError,1);
    if(!moviesFileSuccess){
        throw RecCsvParsingException("Could not open and process movies file.");
    }
    LOG.trace("Processing ratings file: "+ratingsFilePath);
    bool ratingsFileSuccess = openAndProcessFile(ratingsFilePath.c_str(),ratingsfile,terminateOnError,1);
    if(!ratingsFileSuccess){
        throw RecCsvParsingException("Could not open and process ratings file.");
    }
    LOG.trace("Processing tag entries file: "+tagsFilePath);
    bool tagsFileSuccess = openAndProcessFile(tagsFilePath.c_str(),tagsfile,terminateOnError,1);
    if(!tagsFileSuccess){
        throw RecCsvParsingException("Could not open and process tag entries file.");
    }
    LOG.trace("Processing tag ids file: "+genomeTagsFilePath);
    if(tagIdsFileIsUsed){
        bool genometagsFileSuccess = openAndProcessFile(genomeTagsFilePath.c_str(),genometagsfile,terminateOnError,1);
        if(!genometagsFileSuccess){
            throw RecCsvParsingException("Could not open and process tag ids file.");
        }
    } else{
        genometagsfile.linecount = 0;
    }
    cout<<"-file processing complete."<<endl;

    uint tagsMatInitialRowCount = tagIdsFileIsUsed? genometagsfile.linecount : 10;

    //
    LOG.debug("Allocating memory for matrices.");
    cout<<"-allocating memory for matrices..."<<endl;
    LOG.trace("Allocating memory for userEntries matrix.");
    checkedAllocation_2D<float>(userEntries,ratingsfile.linecount,3+config->maxTagsPerUser,true);
    LOG.trace("Allocating memory for movies matrix.");
    checkedAllocation_3D<char>(movies,moviesfile.linecount,2,config->movieNameMaxLength);
    LOG.trace("Allocating memory for tags matrix.");
    if(tagIdsFileIsUsed){
        checkedAllocation_2D<char>(tags,genometagsfile.linecount,config->tagNameMaxLength);
    } else{
        checkedAllocation_2D<char>(tags,tagsMatInitialRowCount,config->tagNameMaxLength); //10 is hardcoded,maybe make configurable?
    }
    LOG.trace("Allocating memory for genreEntries matrix.");
    checkedAllocation_3D<char>(genreEntries,moviesfile.linecount,config->maxGenreCount,config->genreNameMaxLength,true);
    //temporary matrix
    LOG.trace("Allocating memory for temp_tags matrix.");
    checkedAllocation_3D<char>(&temp_tags,tagsfile.linecount,3,config->tagNameMaxLength);

    //tokenlist allocations for files
    char*** tokenlist_moviesfile;
    char*** tokenlist_ratingsfile;
    char*** tokenlist_genometagsfile;
    char*** tokenlist_tagsfile;

    //
    LOG.debug("Allocating memory for tokenlists");
    cout<<"-allocating memory for tokenlists..."<<endl;
    checkedAllocation_3D<char>(&tokenlist_moviesfile,moviesfile.linecount,3+config->maxGenreCount,MAXTOKENLEN,true); //moviesfile
    checkedAllocation_3D<char>(&tokenlist_ratingsfile,ratingsfile.linecount,4,MAXTOKENLEN); //ratingsfile
    if(tagIdsFileIsUsed){
        checkedAllocation_3D<char>(&tokenlist_genometagsfile,genometagsfile.linecount,2,MAXTOKENLEN); //genometagsfile
    }
    checkedAllocation_3D<char>(&tokenlist_tagsfile,tagsfile.linecount,4,MAXTOKENLEN); //tagsfile
    cout<<"-allocations complete."<<endl;

    //parsing
    LOG.debug("Beginning parsing.");
    cout<<"-parsing begin :"<<endl;

    //populate 'movies' matrix.
    LOG.trace("Populating movies matrix.");
    cout<<"-populating 'movies' and 'genreEntries' matrices..."<<endl;

    if(config->multithreadParseOtherEnabled){
        splitAndParallelParseTextBlock_stringLists(config->defaultThreadcount,moviesfile.filetext,moviesfile.newlinePositions,*movies,*genreEntries,3,tokenlist_moviesfile,",|",2+config->maxGenreCount,moviesfile.linecount,false,-1,false);
    }
    else{
        parseTextBlock_stringLists(moviesfile.filetext,*movies,*genreEntries,3,tokenlist_moviesfile,",|",2+config->maxGenreCount,moviesfile.linecount,false); //moviesfile
    }

    //populate 'userEntries' matrix.
    LOG.trace("Populating userEntries matrix.");
    cout<<"-populating 'userEntries' matrix..."<<endl;

    if(config->multithreadParseRatingsEnabled){
        splitAndParallelParseTextBlock_float(config->defaultThreadcount, ratingsfile.filetext, ratingsfile.newlinePositions, *userEntries, tokenlist_ratingsfile, ",", 3, ratingsfile.linecount, -1, false);
    }
    else{
        parseTextBlock_float(ratingsfile.filetext,*userEntries,tokenlist_ratingsfile,",",3,ratingsfile.linecount); //ratingsfile
    }

    //populate 'tags' matrix.
    LOG.trace("Populating tags matrix.");
    cout<<"-populating 'tags' matrix..."<<endl;

    if(tagIdsFileIsUsed){
        if(config->multithreadParseOtherEnabled){
            splitAndParallelParseTextBlock_singleString(config->defaultThreadcount, genometagsfile.filetext, genometagsfile.newlinePositions, *tags, tokenlist_genometagsfile, ",", 2, genometagsfile.linecount, true, 1, false);
        }
        else{
            parseTextBlock_singleString(genometagsfile.filetext,*tags,tokenlist_genometagsfile,",",2,genometagsfile.linecount,true,1); //genometagsfile
        }
    }

    //populate 'temp_tags' matrix.
    LOG.trace("Populating temp_tags matrix.");
    cout<<"-populating 'temp_tags' matrix..."<<endl;

    if(config->multithreadParseOtherEnabled){
        splitAndParallelParseTextBlock_strings(config->defaultThreadcount, tagsfile.filetext, tagsfile.newlinePositions, temp_tags, tokenlist_tagsfile, ",", 3, tagsfile.linecount, true, -1, false);
    }
    else{
        parseTextBlock_strings(tagsfile.filetext,temp_tags,tokenlist_tagsfile,",",3,tagsfile.linecount,true);	//tagsfile(populating TEMP matrix)
    }

    cout<<"-done parsing."<<endl;
    LOG.debug("Parsing finished.");

    //deallocate memory for tokenlists and filetexts, close files.
    cout<<"--deallocating memory for tokenlists...";
    arrcleanup_3D<char>(&tokenlist_moviesfile,moviesfile.linecount,3+config->maxGenreCount);
    arrcleanup_3D<char>(&tokenlist_ratingsfile,ratingsfile.linecount,4);
    if(tagIdsFileIsUsed){
        arrcleanup_3D<char>(&tokenlist_genometagsfile,genometagsfile.linecount,2);
    }
    arrcleanup_3D<char>(&tokenlist_tagsfile,tagsfile.linecount,4);
    cout<<"done."<<endl;

    cout<<"--deallocating memory for file contents...";
    arrcleanup<char>(&(moviesfile.filetext));
    arrcleanup<char>(&(ratingsfile.filetext));
    arrcleanup<char>(&(tagsfile.filetext));
    if(tagIdsFileIsUsed){
        arrcleanup<char>(&(genometagsfile.filetext));
    }

    arrcleanup<ulong>(&(moviesfile.newlinePositions));
    arrcleanup<ulong>(&(ratingsfile.newlinePositions));
    arrcleanup<ulong>(&(tagsfile.newlinePositions));
    if(tagIdsFileIsUsed){
        arrcleanup<ulong>(&(genometagsfile.newlinePositions));
    }

    cout<<"done."<<endl;

    cout << "-closing files...";
    filecleanup(&moviesfile.filepointer);
    filecleanup(&ratingsfile.filepointer);
    filecleanup(&tagsfile.filepointer);
    if(tagIdsFileIsUsed){
        filecleanup(&genometagsfile.filepointer);
    }
    cout << "done." << endl;

    elapsedTimes.setParseElapsed(parseTimer.markEndPointAndGetTimeElapsed());

    parsePostProcessingTimer.markStartPoint();

    //* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    //*** fill columns of userEntries (or append new rows if necessary) ***
    //*** append new rows to tags (if necessary) ***

    LOG.debug("Beginning parsing post-processing.");
    cout << "-PARSING POST PROCESSING: using 'temp_tags' matrix to fill missing columns/rows of 'userEntries', and any missing rows of 'tags'... " << endl;

    //for every row of temp_tags, perform local search for userID and movieID combination, and obtain the tags.
    LOG.trace("For every row of temp_tags: performing local search for user,movieIDs and obtaining tags.");

    char** keys;
    checkedAllocation_2D<char>(&keys,2,config->tagNameMaxLength);
    uint keyPositions[2] = {1,2};
    long indexrange[2];
    //collected_tags: the obtained tags after the userID-movieID search
    //collected_tagIds: the tagIDs corresponding to the found tags.
    char** collected_tags;
    checkedAllocation_2D<char>(&collected_tags,1,config->tagNameMaxLength);
    int* collected_tagids;
    checkedAllocation<int>(&collected_tagids,1);
    float* collected_tagids_final;
    checkedAllocation<float>(&collected_tagids_final,1);
    uint old_dim_tags = 1;

    uint tagmat_addition_count = 0;

    uint usrentr_addition_count = 0;
    uint old_dim_usrentr = ratingsfile.linecount;
    uint old_dim_tagmat;
    if(tagIdsFileIsUsed){
         old_dim_tagmat = genometagsfile.linecount;
    }else{
         old_dim_tagmat = tagsMatInitialRowCount;
         genometagsfile.linecount = 0;
    }

    long target_usrentr_idx;

    LOG.trace("Start loop on temp_tags rows.");
    for(size_t r=0;r<tagsfile.linecount;r++){

        //extracting userId-movieId combination.
        strcpy(keys[0],temp_tags[r][0]);
        strcpy(keys[1],temp_tags[r][1]);

        LOG.trace("Local search on temp_tags matrix. searchKeys: "+string(keys[0])+","+string(keys[1]));
        indexRangeLocalSearchArrayByMultipleColKeysText(temp_tags,keys,keyPositions,2,tagsfile.linecount,indexrange);

        checkedReallocation_2D<char>(&collected_tags,indexrange[1]-indexrange[0]+1,config->tagNameMaxLength,old_dim_tags);
        checkedReallocation<int>(&collected_tagids,indexrange[1]-indexrange[0]+1);
        checkedReallocation<float>(&collected_tagids_final,indexrange[1]-indexrange[0]+1);

        LOG.trace("Copying found values within rowrange: "+to_string(indexrange[0])+","+to_string(indexrange[1]));
        partialArrayCopyText_flatOutput(collected_tags,temp_tags,indexrange,3);

        old_dim_tags = indexrange[1]-indexrange[0]+1;
        r = indexrange[1]; //the for loop will also increase it by 1.

        LOG.trace("Starting inner loop: check if values exist in tags matrix: if not, add them.");
        //transform the collected tags into tagids,check if they exist in the tags matrix (and if not add them) in the process.
        for(int i=0;i<(indexrange[1]-indexrange[0]+1);i++){

            LOG.trace("Inner loop: search tags matrix for found value: "+string(collected_tags[i]));
            collected_tagids[i] = getIndexGlobalSearchArrayByColKeyText(NULL,*tags,collected_tags[i],3,genometagsfile.linecount+tagmat_addition_count);
            //result=-1 : tags were not found in matrix.

            if(collected_tagids[i] < 0){
                LOG.trace("tags matrix did not contain found value. Adding it.");
                tagmat_addition_count++;

                if (genometagsfile.linecount + tagmat_addition_count > old_dim_tagmat){

                    uint new_dim_tagmat = ((uint(old_dim_tagmat*config->tagsReallocFactor) - old_dim_tagmat) >= 1) ? uint(old_dim_tagmat*config->tagsReallocFactor) : old_dim_tagmat+1;

                    //checkedReallocation_2D<char>(&tags, genometagsfile.linecount + tagmat_addition_count, TAGNAME_LEN, genometagsfile.linecount + tagmat_addition_count - 1);
                    checkedReallocation_2D<char>(tags,new_dim_tagmat,config->tagNameMaxLength,old_dim_tagmat);
                    old_dim_tagmat = new_dim_tagmat;
                }

                //transfer to new row of 'tags' matrix.
                strcpy((*tags)[genometagsfile.linecount+tagmat_addition_count-1],collected_tags[i]); //TODO: untested
                strip(&((*tags)[genometagsfile.linecount+tagmat_addition_count-1]),'~'); //TODO: untested
                collected_tagids[i] = genometagsfile.linecount+tagmat_addition_count-1;
            } else{
                LOG.trace("tags matrix DID contain the found value. No need to add.");
            }
        }

        //collected_tagids (int) -> collected_tagids_final (float)
        //collected_tagids_final are increased by 1 (they represent position,not index).
        LOG.trace("Start searching for userID-movieID combo in userEntries matrix.");
        for (int i = 0; i < (indexrange[1] - indexrange[0] + 1); i++){
            collected_tagids_final[i] = static_cast<float>(collected_tagids[i]) +1;
        }

        //search for userID-movieID combo in userEntries, modify it accordingly.(keys=userID,movieID)
        float usrentr_keys[2];
        usrentr_keys[0] = strtof(keys[0], NULL);
        usrentr_keys[1] = strtof(keys[1], NULL);
        uint usrentr_keypositions[2] = { 1, 2 };
        LOG.trace("UserID-movieID combo to search: userID:"+to_string(usrentr_keys[0])+", movieID:"+to_string(usrentr_keys[1]));
        if(config->multithreadSearchEnabled){
            LOG.trace("Initiating multithreaded search. threadcount: "+to_string(config->defaultThreadcount));
            target_usrentr_idx = getIndexGlobalSearchArrayByMultipleColKeysRealNum_parallel(config->defaultThreadcount,*userEntries,usrentr_keys,usrentr_keypositions,2,ratingsfile.linecount);
        }
        else{
            LOG.trace("Initiating singlethreaded search.");
            target_usrentr_idx = getIndexGlobalSearchArrayByMultipleColKeysRealNum(*userEntries, usrentr_keys, usrentr_keypositions, 2, ratingsfile.linecount);
        }

        LOG.trace("Search complete.");
        if (target_usrentr_idx<0){
            //if combo does not exist,append new row in userEntries
            LOG.trace("Combo does NOT exist in userEntries, appending new row.");
            usrentr_addition_count++;
            target_usrentr_idx = ratingsfile.linecount + usrentr_addition_count - 1;
            if (ratingsfile.linecount + usrentr_addition_count > old_dim_usrentr){
                //checkedReallocation_2D<float>(&userEntries, ratingsfile.linecount + usrentr_realloc_count, 3 + config->maxTagsPerUser, ratingsfile.linecount + usrentr_realloc_count - 1,true);
                uint new_dim_usrentr = ((uint(old_dim_usrentr*config->userEntriesReallocFactor) - old_dim_usrentr) >= 1) ? uint(old_dim_usrentr*config->userEntriesReallocFactor) : old_dim_usrentr+1;
                checkedReallocation_2D<float>(userEntries, new_dim_usrentr, 3 + config->maxTagsPerUser, old_dim_usrentr, true);
                old_dim_usrentr = new_dim_usrentr;
            }

            (*userEntries)[ratingsfile.linecount + usrentr_addition_count - 1][0] = usrentr_keys[0];
            (*userEntries)[ratingsfile.linecount + usrentr_addition_count - 1][1] = usrentr_keys[1];
            (*userEntries)[ratingsfile.linecount + usrentr_addition_count - 1][2] = 0;
            partialArrayCopyRealNum_flatInput(*userEntries, collected_tagids_final, target_usrentr_idx + 1, 4, indexrange[1] - indexrange[0] + 1);

        }
        else{
            //else (if it does exist),add to columns of target row
            LOG.trace("Combo DOES exist in userEntries, adding tags to existing row.");
            partialArrayCopyRealNum_flatInput(*userEntries, collected_tagids_final, target_usrentr_idx + 1, 4, indexrange[1] - indexrange[0] + 1);
        }

    }
    LOG.trace("Loop on temp_tags matrix contents finished.");

    checkedReallocation_2D<float>(userEntries, ratingsfile.linecount + usrentr_addition_count, 3 + config->maxTagsPerUser, old_dim_usrentr, true);
    checkedReallocation_2D<char>(tags,tagsMatInitialRowCount+tagmat_addition_count,config->tagNameMaxLength,old_dim_tagmat);

    cout << "-done." << endl;
    LOG.debug("Parsing post-processing finished.");

    cout << "-deallocating 'temp_tags' matrix...";
    arrcleanup_3D<char>(&temp_tags, tagsfile.linecount, 3);
    cout<<"done."<<endl;

    cout<<"-cleaning up other temporary resources...";
    arrcleanup_2D<char>(&collected_tags,ulong(indexrange[1]-indexrange[0]+1));
    arrcleanup<int>(&collected_tagids);
    arrcleanup<float>(&collected_tagids_final);
    arrcleanup_2D<char>(&keys,2);
    cout << "done." << endl;
    cout << "---" << endl;

    cout<<"**Finished!**"<<endl;

    uint userentries_mat_rowsize = ratingsfile.linecount+usrentr_addition_count;
    uint movies_mat_rowsize = moviesfile.linecount;
    uint tags_mat_rowsize = tagsMatInitialRowCount+tagmat_addition_count;
    uint genentries_mat_rowsize = moviesfile.linecount;

    MatrixSizesWrapper result;
    result.userEntriesMatrixRowsize = userentries_mat_rowsize;
    result.moviesMatrixRowsize = movies_mat_rowsize;
    result.tagsMatrixRowsize = tags_mat_rowsize;
    result.genreEntriesMatrixRowsize = genentries_mat_rowsize;

    if(config->writeIntermediateMatricesToFile){
        FILE* userEntries_outputFile = fopen("MATRIX_OUTPUT/userEntries.txt","w");
        FILE* movies_outputFile = fopen("MATRIX_OUTPUT/movies.txt","w");
        FILE* tags_outputFile = fopen("MATRIX_OUTPUT/tags.txt","w");
        FILE* genreEntries_outputFile = fopen("MATRIX_OUTPUT/genreEntries.txt","w");
        writeToFile_UserEntries(config->maxTagsPerUser,*userEntries,result.userEntriesMatrixRowsize,userEntries_outputFile);
        writeToFile_Movies(*movies,result.moviesMatrixRowsize,movies_outputFile);
        writeToFile_Tags(*tags,result.tagsMatrixRowsize,tags_outputFile);
        writeToFile_GenreEntries(config->maxGenreCount,*genreEntries,result.genreEntriesMatrixRowsize,genreEntries_outputFile);
        fclose(userEntries_outputFile);
        fclose(movies_outputFile);
        fclose(tags_outputFile);
        fclose(genreEntries_outputFile);
    }

    elapsedTimes.setParsePostProcessElapsed(parsePostProcessingTimer.markEndPointAndGetTimeElapsed());

    LOG.info("loadFromFiles finished.");
    LOG.close();
    return result;

}

void cleanup(Configuration* config,MatrixSizesWrapper sizes,float*** userEntries,char**** movies,char*** tags,char**** genreEntries){

    RecommenderLogger LOG;
    LOG.init("ImportCsv:Cleanup");
    LOG.info("START: cleanup.");

    arrcleanup_2D<float>(userEntries,sizes.userEntriesMatrixRowsize);
    arrcleanup_3D<char>(movies,sizes.moviesMatrixRowsize,2);
    arrcleanup_2D<char>(tags,sizes.tagsMatrixRowsize);
    arrcleanup_3D<char>(genreEntries,sizes.genreEntriesMatrixRowsize,config->maxGenreCount);

    LOG.info("cleanup finished.");
    LOG.close();
    cout << "Deallocation finished successfully." << endl;
}
