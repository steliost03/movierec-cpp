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

#include "DEMO_DEPENDENCIES.h"
#include "DEMO_CONFIG.h"

char filemenu();
uint multithreadmenu();
char menu();
char write_to_file_menu();

void show_usrentr(float** userEntries,uint rowcount);
void show_movies(char*** movies,uint rowcount);
void show_gnrentr(char*** genreEntries, uint rowcount);
void show_tags(char** tags,uint rowcount);

void show_metrics(double times[13]);

void writeToFile_usrentr(float** userEntries,uint rowcount,FILE* file);
void writeToFile_movies(char*** movies,uint rowcount,FILE* file);
void writeToFile_gnrentr(char*** genreEntries,uint rowcount,FILE* file);
void writeToFile_tags(char** tags,uint rowcount,FILE* file);


int main() {
	
	bool multithread_parse_ratings_enabled;
	bool multithread_parse_other_enabled;
	bool multithread_search_enabled;
	
	char filechoice = filemenu();
	string moviesfile_path;
	string ratingsfile_path;
	string tagsfile_path;
	string genometagsfile_path;
	switch(filechoice){
		case '1':
			moviesfile_path = MOCK_MOVIESFILE_PATH;
			ratingsfile_path = MOCK_RATINGSFILE_PATH;
			tagsfile_path = MOCK_TAGSFILE_PATH;
			genometagsfile_path = MOCK_GENOMETAGSFILE_PATH;
			if(AUTOMATIC_MULTITHREAD){
				multithread_parse_ratings_enabled = false;
				multithread_parse_other_enabled = false;
				multithread_search_enabled = false;
			}
			break;
		case '2':
			moviesfile_path = SMALL_MOVIESFILE_PATH;
			ratingsfile_path = SMALL_RATINGSFILE_PATH;
			tagsfile_path = SMALL_TAGSFILE_PATH;
			genometagsfile_path = SMALL_GENOMETAGSFILE_PATH;
			if(AUTOMATIC_MULTITHREAD){
				multithread_parse_ratings_enabled = true;
				multithread_parse_other_enabled = false;
				multithread_search_enabled = true;
			}
			break;
		case '3':
			moviesfile_path = LARGE_MOVIESFILE_PATH;
			ratingsfile_path = LARGE_RATINGSFILE_PATH;
			tagsfile_path = LARGE_TAGSFILE_PATH;
			genometagsfile_path = LARGE_GENOMETAGSFILE_PATH;
			if(AUTOMATIC_MULTITHREAD){
				multithread_parse_ratings_enabled = true;
				multithread_parse_other_enabled = true;
				multithread_search_enabled = true;
			}
			break;
		case '4':
			moviesfile_path = VERY_LARGE_MOVIESFILE_PATH;
			ratingsfile_path = VERY_LARGE_RATINGSFILE_PATH;
			tagsfile_path = VERY_LARGE_TAGSFILE_PATH;
			genometagsfile_path = VERY_LARGE_GENOMETAGSFILE_PATH;
			if(AUTOMATIC_MULTITHREAD){
				multithread_parse_ratings_enabled = true;
				multithread_parse_other_enabled = true;
				multithread_search_enabled = true;
			}
			break;
		default: 
			exit(0);
	}
	
	if(!AUTOMATIC_MULTITHREAD){
		cout<<"Automatic multithread is disabled."<<endl;
		multithread_parse_ratings_enabled = MULTITHREAD_PARSE_RATINGS_ENABLED;
		multithread_parse_other_enabled = MULTITHREAD_PARSE_OTHER_ENABLED;
		multithread_search_enabled = MULTITHREAD_SEARCH_ENABLED;
	}
	
	uint threadcount = DEFAULT_THREADCOUNT;
	
	if(multithread_parse_ratings_enabled || multithread_parse_other_enabled || multithread_search_enabled){
		threadcount = multithreadmenu();
		if(threadcount == 0){
			cout<<"Multithreading will not be used."<<endl;
			multithread_parse_ratings_enabled = false;
			multithread_parse_other_enabled = false;
			multithread_search_enabled = false;
		}	
	}

	
	//timer
	CustomTimer timer;
	
	//times
	double fileprocess_elapsed = 0; 
	double initalloc_elapsed = 0; 
	double parse_elapsed = 0; 
	double temptags_search_elapsed = 0;
	double temptags_copy_elapsed = 0;
	double tagIDs_realloc_elapsed = 0;
	double tags_search_elapsed = 0;
	double tags_realloc_elapsed = 0;
	double tags_copy_elapsed = 0;
	double usr_search_elapsed = 0;
	double usr_realloc_elapsed = 0;
	double usr_copy_elapsed = 0;
	double total_elapsed = 0;
	
	//files
	file moviesfile;
	file ratingsfile;
	file tagsfile;
	file genometagsfile;
	
	cout<<"-processing files..."<<endl;
	timer.markStartPoint();
	openAndProcessFile(moviesfile_path.c_str(),moviesfile,1);
	openAndProcessFile(ratingsfile_path.c_str(),ratingsfile,1);
	openAndProcessFile(tagsfile_path.c_str(),tagsfile,1);
	openAndProcessFile(genometagsfile_path.c_str(),genometagsfile,1);
	fileprocess_elapsed += timer.markEndPointAndGetTimeElapsed();
	cout<<"-file processing complete."<<endl;
	
	//matrices
	float** userEntries;
	char*** movies;
	char** tags;
	char*** genreEntries;
	//temporary matrix
	char*** temp_tags;
	
	cout<<"-allocating memory for matrices..."<<endl;
	timer.markStartPoint();
	checkedAllocation_2D<float>(&userEntries,ratingsfile.linecount,3+MAXTAGS_PERUSER,true);
	checkedAllocation_3D<char>(&movies,moviesfile.linecount,2,MOVTITLE_LEN);
	checkedAllocation_2D<char>(&tags,genometagsfile.linecount,TAGNAME_LEN);
	checkedAllocation_3D<char>(&genreEntries,moviesfile.linecount,MAXGENRECOUNT,GENRENAME_LEN,true);
	//temporary matrix
	checkedAllocation_3D<char>(&temp_tags,tagsfile.linecount,3,TAGNAME_LEN);
	initalloc_elapsed += timer.markEndPointAndGetTimeElapsed();
	
	//tokenlist allocations for files
	char*** tokenlist_moviesfile;
	char*** tokenlist_ratingsfile;
	char*** tokenlist_genometagsfile;
	char*** tokenlist_tagsfile;
	
	cout<<"-allocating memory for tokenlists..."<<endl;
	timer.markStartPoint();
	checkedAllocation_3D<char>(&tokenlist_moviesfile,moviesfile.linecount,3+MAXGENRECOUNT,MAXTOKENLEN,true); //moviesfile
	checkedAllocation_3D<char>(&tokenlist_ratingsfile,ratingsfile.linecount,4,MAXTOKENLEN); //ratingsfile
	checkedAllocation_3D<char>(&tokenlist_genometagsfile,genometagsfile.linecount,2,MAXTOKENLEN); //genometagsfile
	checkedAllocation_3D<char>(&tokenlist_tagsfile,tagsfile.linecount,4,MAXTOKENLEN); //tagsfile
	initalloc_elapsed += timer.markEndPointAndGetTimeElapsed();
	
	
	cout<<"-allocations complete."<<endl;
	
	//parsing
	cout<<"-parsing begin :"<<endl;
	cout<<"-populating 'movies' and 'genreEntries' matrices..."<<endl;
	

	if(multithread_parse_other_enabled){
		timer.markStartPoint();
		splitAndParallelParseTextBlock_stringLists(threadcount,moviesfile.filetext,moviesfile.newlinePositions,movies,genreEntries,3,tokenlist_moviesfile,",|",2+MAXGENRECOUNT,moviesfile.linecount,false,-1,false);
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	else{
		timer.markStartPoint();
		parseTextBlock_stringLists(moviesfile.filetext,movies,genreEntries,3,tokenlist_moviesfile,",|",2+MAXGENRECOUNT,moviesfile.linecount,false); //moviesfile
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	
	cout<<"-populating 'userEntries' matrix..."<<endl;

	if(multithread_parse_ratings_enabled){
		timer.markStartPoint();
		splitAndParallelParseTextBlock_float(threadcount,ratingsfile.filetext,ratingsfile.newlinePositions,userEntries,tokenlist_ratingsfile,",",3,ratingsfile.linecount,-1,false);
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	else{
		timer.markStartPoint();
		parseTextBlock_float(ratingsfile.filetext,userEntries,tokenlist_ratingsfile,",",3,ratingsfile.linecount); //ratingsfile
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	

	cout<<"-populating 'tags' matrix..."<<endl;

	if(multithread_parse_other_enabled){
		timer.markStartPoint();
		splitAndParallelParseTextBlock_singleString(threadcount,genometagsfile.filetext,genometagsfile.newlinePositions,tags,tokenlist_genometagsfile,",",2,genometagsfile.linecount,true,1,false);
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	else{
		timer.markStartPoint();
		parseTextBlock_singleString(genometagsfile.filetext,tags,tokenlist_genometagsfile,",",2,genometagsfile.linecount,true,1); //genometagsfile
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	
	cout<<"-populating 'temp_tags' matrix..."<<endl;

	if(multithread_parse_other_enabled){
		timer.markStartPoint();
		splitAndParallelParseTextBlock_strings(threadcount,tagsfile.filetext,tagsfile.newlinePositions,temp_tags,tokenlist_tagsfile,",",3,tagsfile.linecount,true,-1,false);
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	else{
		timer.markStartPoint();
		parseTextBlock_strings(tagsfile.filetext,temp_tags,tokenlist_tagsfile,",",3,tagsfile.linecount,true);	//tagsfile(populating TEMP matrix)
		parse_elapsed += timer.markEndPointAndGetTimeElapsed();
	}
	
	cout<<"-done parsing."<<endl;
	cout<<"--deallocating memory for tokenlists...";
	arrcleanup_3D<char>(&tokenlist_moviesfile,moviesfile.linecount,3+MAXGENRECOUNT);
	arrcleanup_3D<char>(&tokenlist_ratingsfile,ratingsfile.linecount,4);
	arrcleanup_3D<char>(&tokenlist_genometagsfile,genometagsfile.linecount,2);
	arrcleanup_3D<char>(&tokenlist_tagsfile,tagsfile.linecount,4);
	cout<<"done."<<endl;
	cout<<"--deallocating memory for file contents...";
	arrcleanup<char>(&(moviesfile.filetext));
	arrcleanup<char>(&(ratingsfile.filetext));
	arrcleanup<char>(&(tagsfile.filetext));
	arrcleanup<char>(&(genometagsfile.filetext));
	//
	arrcleanup<ulong>(&(moviesfile.newlinePositions));
	arrcleanup<ulong>(&(ratingsfile.newlinePositions));
	arrcleanup<ulong>(&(tagsfile.newlinePositions));
	arrcleanup<ulong>(&(genometagsfile.newlinePositions));
	cout<<"done."<<endl;
	cout << "-closing files...";
	filecleanup(&moviesfile.filepointer);
	filecleanup(&ratingsfile.filepointer);
	filecleanup(&tagsfile.filepointer);
	filecleanup(&genometagsfile.filepointer);
	cout << "done." << endl;
	
	//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	//*** fill columns of userEntries (or append new rows if necessary) ***
	//*** append new rows to tags (if necessary) ***

	cout << "-using 'temp_tags' matrix to fill missing columns/rows of 'userEntries', and any missing rows of 'tags'... " << endl;

	//for every row of temp_tags, perform local search for userID and movieID combination, and obtain the tags.
	
	char** keys;
	checkedAllocation_2D<char>(&keys,2,TAGNAME_LEN);
	uint keyPositions[2] = {1,2};
	long indexrange[2];
	char** collected_tags;
	checkedAllocation_2D<char>(&collected_tags,1,TAGNAME_LEN);
	int* collected_tagids;
	checkedAllocation<int>(&collected_tagids,1);
	float* collected_tagids_final;
	checkedAllocation<float>(&collected_tagids_final,1);
	uint old_dim_tags = 1;
	
	uint tagmat_addition_count = 0;

	uint usrentr_addition_count = 0;
	uint old_dim_usrentr = ratingsfile.linecount;
	uint old_dim_tagmat = genometagsfile.linecount;
	
	long target_usrentr_idx;

	for(int r=0;r<tagsfile.linecount;r++){

		strcpy(keys[0],temp_tags[r][0]);
		strcpy(keys[1],temp_tags[r][1]);

		timer.markStartPoint();
		indexRangeLocalSearchArrayByMultipleColKeysText(temp_tags,keys,keyPositions,2,tagsfile.linecount,indexrange);
		temptags_search_elapsed += timer.markEndPointAndGetTimeElapsed();

		timer.markStartPoint();
		checkedReallocation_2D<char>(&collected_tags,indexrange[1]-indexrange[0]+1,TAGNAME_LEN,old_dim_tags);
		checkedReallocation<int>(&collected_tagids,indexrange[1]-indexrange[0]+1);
		checkedReallocation<float>(&collected_tagids_final,indexrange[1]-indexrange[0]+1);
		tagIDs_realloc_elapsed += timer.markEndPointAndGetTimeElapsed();

		timer.markStartPoint();
		partialArrayCopyText_flatOutput(collected_tags,temp_tags,indexrange,3);
		temptags_copy_elapsed += timer.markEndPointAndGetTimeElapsed();

		old_dim_tags = indexrange[1]-indexrange[0]+1;
		r = indexrange[1]; //the for loop will also increase it by 1.

		//transform the collected tags into tagids,check if they exist in the tags matrix (and if not add them) in the process.
		for(int i=0;i<(indexrange[1]-indexrange[0]+1);i++){
		
			timer.markStartPoint();
			collected_tagids[i] = getIndexGlobalSearchArrayByColKeyText(NULL,tags,collected_tags[i],3,genometagsfile.linecount);
			tags_search_elapsed += timer.markEndPointAndGetTimeElapsed();
			
			if(collected_tagids[i] < 0){
				tagmat_addition_count++;
				
				if (genometagsfile.linecount + tagmat_addition_count > old_dim_tagmat){
					
					timer.markStartPoint();
					uint new_dim_tagmat = ((uint(old_dim_tagmat*TAGS_REALLOC_FACTOR) - old_dim_tagmat) >= 1) ? uint(old_dim_tagmat*TAGS_REALLOC_FACTOR) : old_dim_tagmat+1;

					//checkedReallocation_2D<char>(&tags, genometagsfile.linecount + tagmat_addition_count, TAGNAME_LEN, genometagsfile.linecount + tagmat_addition_count - 1);
					checkedReallocation_2D<char>(&tags,new_dim_tagmat,TAGNAME_LEN,old_dim_tagmat);
					old_dim_tagmat = new_dim_tagmat;
					tags_realloc_elapsed += timer.markEndPointAndGetTimeElapsed();
					
				}

				timer.markStartPoint();
				strcpy(tags[genometagsfile.linecount+tagmat_addition_count-1],collected_tags[i]);
				strip(&tags[genometagsfile.linecount+tagmat_addition_count-1],'~');
				collected_tagids[i] = genometagsfile.linecount+tagmat_addition_count-1;
				tags_copy_elapsed += timer.markEndPointAndGetTimeElapsed();
				
			}
			
		}

		//collected_tagids (int) -> collected_tagids_final (float)
		//collected_tagids_final are increased by 1 (they represent position,not index).
		for (int i = 0; i < (indexrange[1] - indexrange[0] + 1); i++){
			collected_tagids_final[i] = (float)collected_tagids[i] +1;
		}
		
		//search for userID-movieID combo in userEntries, modify it accordingly.
		float usrentr_keys[2];
		usrentr_keys[0] = strtod(keys[0], NULL);
		usrentr_keys[1] = strtod(keys[1], NULL);
		uint usrentr_keypositions[2] = { 1, 2 };
		
		if(multithread_search_enabled){
			timer.markStartPoint();
			target_usrentr_idx = getIndexGlobalSearchArrayByMultipleColKeysRealNum_parallel(threadcount,userEntries,usrentr_keys,usrentr_keypositions,2,ratingsfile.linecount);
			usr_search_elapsed += timer.markEndPointAndGetTimeElapsed();
		}
		else{
			timer.markStartPoint();
			target_usrentr_idx = getIndexGlobalSearchArrayByMultipleColKeysRealNum(userEntries, usrentr_keys, usrentr_keypositions, 2, ratingsfile.linecount);
			usr_search_elapsed += timer.markEndPointAndGetTimeElapsed();
		}
		

		if (target_usrentr_idx<0){
			//if combo does not exist,append new row in userEntries
			usrentr_addition_count++;
			target_usrentr_idx = ratingsfile.linecount + usrentr_addition_count - 1;
			if (ratingsfile.linecount + usrentr_addition_count > old_dim_usrentr){
				//checkedReallocation_2D<float>(&userEntries, ratingsfile.linecount + usrentr_realloc_count, 3 + MAXTAGS_PERUSER, ratingsfile.linecount + usrentr_realloc_count - 1,true);
				uint new_dim_usrentr = ((uint(old_dim_usrentr*USRENTR_REALLOC_FACTOR) - old_dim_usrentr) >= 1) ? uint(old_dim_usrentr*USRENTR_REALLOC_FACTOR) : old_dim_usrentr+1;
				timer.markStartPoint();
				checkedReallocation_2D<float>(&userEntries, new_dim_usrentr, 3 + MAXTAGS_PERUSER, old_dim_usrentr, true);
				usr_realloc_elapsed = timer.markEndPointAndGetTimeElapsed();
				old_dim_usrentr = new_dim_usrentr;
			}
			
			timer.markStartPoint();
			userEntries[ratingsfile.linecount + usrentr_addition_count - 1][0] = usrentr_keys[0];
			userEntries[ratingsfile.linecount + usrentr_addition_count - 1][1] = usrentr_keys[1];
			userEntries[ratingsfile.linecount + usrentr_addition_count - 1][2] = 0;
			partialArrayCopyRealNum_flatInput(userEntries, collected_tagids_final, target_usrentr_idx + 1, 4, indexrange[1] - indexrange[0] + 1);
			usr_copy_elapsed += timer.markEndPointAndGetTimeElapsed();
		
		}
		else{
			//else,add to columns of target row

			timer.markStartPoint();
			partialArrayCopyRealNum_flatInput(userEntries, collected_tagids_final, target_usrentr_idx + 1, 4, indexrange[1] - indexrange[0] + 1);
			usr_copy_elapsed = timer.markEndPointAndGetTimeElapsed();

		}
		
	}
	
	timer.markStartPoint();
	checkedReallocation_2D<float>(&userEntries, ratingsfile.linecount + usrentr_addition_count, 3 + MAXTAGS_PERUSER, old_dim_usrentr, true);
	usr_realloc_elapsed += timer.markEndPointAndGetTimeElapsed();
	timer.markStartPoint();
	checkedReallocation_2D<char>(&tags,genometagsfile.linecount+tagmat_addition_count,TAGNAME_LEN,old_dim_tagmat);
	tags_realloc_elapsed += timer.markEndPointAndGetTimeElapsed();
	
	cout << "-done." << endl;
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
	cout << "Finished." << endl;
	
	uint userentries_mat_rowsize = ratingsfile.linecount+usrentr_addition_count;
	uint movies_mat_rowsize = moviesfile.linecount;
	uint tags_mat_rowsize = genometagsfile.linecount+tagmat_addition_count;
	uint genentries_mat_rowsize = moviesfile.linecount;
	
	//* * * * *
	//SHOW MENU
	cout << "Press Enter to proceed to the menu." << endl;
	cin.get();

	char choice = '0';
	while (choice != '7'){
		choice = menu();
		if (choice == '1')
			show_usrentr(userEntries, userentries_mat_rowsize);
		else if(choice ==  '2')
			show_movies(movies, movies_mat_rowsize);
		else if(choice ==  '3')
			show_gnrentr(genreEntries, genentries_mat_rowsize);
		else if (choice == '4')
			show_tags(tags, tags_mat_rowsize);
		else if (choice == '5'){
			
			char choice_file = write_to_file_menu();

			FILE* usrentrFile_out;
			FILE* moviesFile_out;
			FILE* gnrentrFile_out;
			FILE* tagsFile_out;
			string filename;
			string path_usrentr;
			string path_mov;
			string path_gnrentr;
			string path_tag;
			
			filename = "userEntries.txt";
			path_usrentr = OUTPUT_FILE_PATH_BEGIN + filename;
			const char* cpath_usrentr = path_usrentr.c_str();
			filename = "movies.txt";
			path_mov = OUTPUT_FILE_PATH_BEGIN + filename;
			const char* cpath_mov = path_mov.c_str();
			filename = "genreEntries.txt";
			path_gnrentr = OUTPUT_FILE_PATH_BEGIN + filename;
			const char* cpath_gnrentr = path_gnrentr.c_str();
			filename = "tags.txt";
			path_tag = OUTPUT_FILE_PATH_BEGIN + filename;
			const char* cpath_tag = path_tag.c_str();

			switch (choice_file){

			case '1':
				filecheckedopen(cpath_usrentr,"w",&usrentrFile_out);
				writeToFile_usrentr(userEntries,userentries_mat_rowsize,usrentrFile_out);
				fflush(usrentrFile_out);
				fclose(usrentrFile_out);
				break;
			case '2':
				filecheckedopen(cpath_mov,"w",&moviesFile_out);
				writeToFile_movies(movies,movies_mat_rowsize,moviesFile_out);
				fflush(moviesFile_out);
				fclose(moviesFile_out);
				break;
			case '3':
				filecheckedopen(cpath_gnrentr,"w",&gnrentrFile_out);
				writeToFile_gnrentr(genreEntries,genentries_mat_rowsize,gnrentrFile_out);
				fflush(gnrentrFile_out);
				fclose(gnrentrFile_out);
				break;
			case '4':
				filecheckedopen(cpath_tag,"w",&tagsFile_out);
				writeToFile_tags(tags,tags_mat_rowsize,tagsFile_out);
				fflush(tagsFile_out);
				fclose(tagsFile_out);
				break;

			case '5':
				filecheckedopen(cpath_usrentr, "w", &usrentrFile_out);
				filecheckedopen(cpath_mov, "w", &moviesFile_out);
				filecheckedopen(cpath_gnrentr, "w", &gnrentrFile_out);
				filecheckedopen(cpath_tag, "w", &tagsFile_out);
				writeToFile_usrentr(userEntries, userentries_mat_rowsize,usrentrFile_out);
				writeToFile_movies(movies, movies_mat_rowsize,moviesFile_out);
				writeToFile_gnrentr(genreEntries, genentries_mat_rowsize,gnrentrFile_out);
				writeToFile_tags(tags, tags_mat_rowsize,tagsFile_out);
				fflush(usrentrFile_out);
				fflush(moviesFile_out);
				fflush(gnrentrFile_out);
				fflush(tagsFile_out);
				fclose(usrentrFile_out);
				fclose(moviesFile_out);
				fclose(gnrentrFile_out);
				fclose(tagsFile_out);
				break;


			}
			cout << "Press Enter to continue" << endl;
			cin.get();
		}
		else if (choice == '6'){
			
			double total_elapsed = 0;
			double times[13] = { fileprocess_elapsed, initalloc_elapsed, parse_elapsed, 
				              temptags_search_elapsed, temptags_copy_elapsed, tagIDs_realloc_elapsed, tags_search_elapsed,
							  tags_realloc_elapsed, tags_copy_elapsed, usr_search_elapsed, usr_realloc_elapsed, usr_copy_elapsed, 
							  total_elapsed };

			for (int i = 0; i < 11; i++){
				times[12] = times[12] + times[i];
			}

			show_metrics(times);
		}
		else{
			break;
		}

	}
	
	//final cleanup,for testing purposes (testing memory integrity)
	arrcleanup_2D<float>(&userEntries,userentries_mat_rowsize);
	arrcleanup_3D<char>(&movies,movies_mat_rowsize,2);
	arrcleanup_2D<char>(&tags,tags_mat_rowsize);
	arrcleanup_3D<char>(&genreEntries,genentries_mat_rowsize,MAXGENRECOUNT);
}

char filemenu(){
	
	string choice_str = "";
	do{
		cout<<"- - -"<<endl;
		cout<<"Choose input file size/type"<<endl;
		cout<<"1.Mock"<<endl;
		cout<<"2.Small (100k)"<<endl;
		cout<<"3.Large (10m)"<<endl;
		cout<<"4.Very Large(20m)"<<endl;
		getline(cin,choice_str);
	}while((choice_str != "1") && (choice_str != "2") && (choice_str != "3") && (choice_str != "4"));
	
	char choice = choice_str.at(0);

	return choice;
	
}

uint multithreadmenu(){
	
	string choice_str = "";
	do{
	cout<<"- - -"<<endl;
	cout<<"Multithreading options:"<<endl;
	cout<<"1.Use maximum amount of threads. (Maximum for this machine = "<<getMaximumThreads()<<")"<<endl;
	cout<<"2.Use default amount of threads. (Current default = "<<DEFAULT_THREADCOUNT<<")"<<endl;
	cout<<"3.Manually insert amount of threads"<<endl;
	cout<<"4.Do not use multithreading."<<endl;
	getline(cin,choice_str);
	}while((choice_str != "1") && (choice_str != "2") && (choice_str != "3") && (choice_str != "4"));
	
	char choice = choice_str.at(0);
	
	switch(choice){
		case '1':
			return getMaximumThreads();
		case '2':
			return DEFAULT_THREADCOUNT;
		case '3':
			{
				uint maxthreads = getMaximumThreads();
				long manuallyChosenThreadcount = 0;
				cout<<endl<<"--"<<endl;
				do{
					cout<<"Insert amount of threads (maximum = "<<maxthreads<<")"<<endl;
					cin>>manuallyChosenThreadcount;
				}while((manuallyChosenThreadcount<1)||(manuallyChosenThreadcount>maxthreads));
				cout<<"You have chosen: "<<manuallyChosenThreadcount<<" threads."<<endl;
				return uint(manuallyChosenThreadcount);
			}
		case '4':
			return 0;
		default:
			return 0;
	}	
}

char menu(){

	string choice_str = "";
	do{
		cout << "- - -" << endl;
		cout << "1.Show 'User Entries'" << endl;
		cout << "2.Show 'Movies'" << endl;
		cout << "3.Show 'Genre Entries'" << endl;
		cout << "4.Show 'Tags'" << endl;
		cout << "- - -" << endl;
		cout << "5.Write to file" << endl;
		cout << "6.View Metrics" << endl;
		cout << "7.Exit" << endl;
		getline(cin,choice_str);

	} while ((choice_str != "1") && (choice_str != "2") && (choice_str != "3") && (choice_str != "4") && (choice_str != "5") && (choice_str != "6") && (choice_str != "7"));

	char choice = choice_str.at(0);

	return choice;
}


void show_usrentr(float** userEntries,uint rowcount){
	
	cout << "\nUSER ENTRIES :" << endl;
	cout << "Press Enter" << endl;
	cin.get();
	
	//column names
	cout << "userID\tmovieID\trating\ttagIDs" << endl;
	cout << "--" << endl;

	for (uint r = 0; r < rowcount; r++){
		for (uint c = 0; c < (3+MAXTAGS_PERUSER); c++){
			if (userEntries[r][c] == 0){
				if (c == 2){
					cout << "-\t";
					continue;
				}
				else{
					break;
				}
			}
			cout << userEntries[r][c] << "\t";
		}
		cout << endl;
	}

	cout << "--" << endl;
	cout << "end of 'userEntries' matrix." << endl << endl;
}

void show_movies(char*** movies, uint rowcount){

	cout << "\nMOVIES :" << endl;
	cout << "Press Enter" << endl;
	cin.get();

	//column names
	cout << "movieID\ttitle" << endl;
	cout << "--" << endl;

	for (uint r = 0; r < rowcount; r++){
		for (uint c = 0; c < 2; c++){
			cout << movies[r][c] << "\t";
		}
		cout << endl;
	}

	cout << "--" << endl;
	cout << "end of 'movies' matrix." << endl << endl;
}

void show_gnrentr(char*** genreEntries, uint rowsize){

	cout << "\nGENRE ENTRIES :" << endl;
	cout << "Press Enter" << endl;
	cin.get();

	//column names
	cout << "genres" << endl;
	cout << "--" << endl;

	for (uint r = 0; r < rowsize; r++){
		for (uint c = 0; c < MAXGENRECOUNT; c++){
			if (genreEntries[r][c][0] == 0)
				break;
			cout << genreEntries[r][c] << ",";
		}
		cout << endl;
	}

	cout << "--" << endl;
	cout << "end of 'genreEntries' matrix." << endl << endl;
}

void show_tags(char** tags, uint rowsize){

	cout << "\nTAGS :" << endl;
	cout << "Press Enter" << endl;
	cin.get();

	//column names
	cout << "tag" << endl;
	cout << "--" << endl;

	for (uint r = 0; r < rowsize; r++){
			cout << tags[r] << endl;
	
	}

	cout << "--" << endl;
	cout << "end of 'tags' matrix." << endl << endl;

}

char write_to_file_menu(){

	string choice_str = "";
	char choice;
	do{

		cout << "- - -" << endl;
		cout << "Write to file" << endl;
		cout << "- - -" << endl;
		cout << "Select matrix:" << endl << endl;

		cout << "1.User Entries" << endl;
		cout << "2.Movies" << endl;
		cout << "3.Genre Entries" << endl;
		cout << "4.Tags" << endl;
		cout << "5.ALL" << endl;
		getline(cin,choice_str);

		choice = choice_str.at(0);

	} while ((choice_str != "1") && (choice_str != "2") && (choice_str != "3") && (choice_str != "4") && (choice_str != "5"));

	return choice;
}

void writeToFile_usrentr(float** userEntries, uint rowcount,FILE* file){

	cout << "Writing userEntries matrix to file..." << endl;

	fprintf(file,"-- USER ENTRIES MATRIX --\n");
	fprintf(file,"[userID],[movieID],[rating],[tagIDs]\n");

	for (uint r = 0; r < rowcount; r++){
		for (uint c = 0; c < (3 + MAXTAGS_PERUSER); c++){
			if (userEntries[r][c] == 0){
				if (c == 2){
					fprintf(file, "-");
					fprintf(file,",");
					continue;
				}
				else{
					break;
				}
			}
			if (c != 2){
				fprintf(file, "%0.f", userEntries[r][c]);
			}
			else{
				fprintf(file, "%0.1f", userEntries[r][c]);
			}
			if (c > 2){
				fprintf(file, "|");
			}
			else{
				fprintf(file, ",");
			}
		}
		fprintf(file, "\n");

	}

	cout << "done." << endl;
}

void writeToFile_movies(char*** movies, uint rowcount,FILE* file){

	cout << "Writing movies matrix to file..." << endl;

	fprintf(file, "-- MOVIES MATRIX --\n");
	fprintf(file, "[movieID],[title]\n");

	for (uint r = 0; r < rowcount; r++){

		for (uint c = 0; c < 2; c++){
			fprintf(file,"%s",movies[r][c]);
			fprintf(file,",");
		}
		fprintf(file,"\n");

	}

	cout << "done." << endl;
}

void writeToFile_gnrentr(char*** genreEntries, uint rowcount,FILE* file){

	cout << "Writing genreEntries matrix to file..." << endl;

	fprintf(file, "-- GENRE ENTRIES MATRIX --\n");
	fprintf(file, "[genres]\n");

	for (uint r = 0; r < rowcount; r++){

		for (uint c = 0; c < MAXGENRECOUNT; c++){
			if (genreEntries[r][c][0] == 0)
				break;
			fprintf(file,"%s",genreEntries[r][c]);
			fprintf(file,"|");
		
		}
		fprintf(file, "\n");

	}

	cout << "done." << endl;
}

void writeToFile_tags(char** tags, uint rowcount,FILE* file){

	cout << "Writing tags matrix to file..." << endl;

	fprintf(file, "-- TAGS MATRIX --\n");
	fprintf(file, "[tag]\n");

	for (uint r = 0; r < rowcount; r++){
		fprintf(file,"%s",tags[r]);
		fprintf(file,"\n");
	}

	cout << "done." << endl;
}
 
void show_metrics(double times[13]){

	cout <<endl<< "- - -" << endl;
	cout << "( all times are in seconds )" << endl;

	cout << "--Initialization--" << endl;
	cout << "File processing :" << times[0] << endl;
	cout << "Initial memory allocations :" << times[1] << endl;
	cout << "File parsing :" << times[2] << endl;

	cout << "--temp_tags--" << endl;
	cout << "temp_tags searches :" << times[3] << endl;
	cout << "temp_tags copies :" << times[4] << endl;
	cout << "tagIDs reallocations :" << times[5] << endl;

	cout << "--tags--" << endl;
	cout << "tags searches :" << times[6] << endl;
	cout << "tags reallocations :" << times[7] << endl;
	cout << "tags copies :" << times[8] << endl;

	cout << "--userEntries--" << endl;
	cout << "userEntries searches :" << times[9] << endl;
	cout << "userEntries reallocations :" << times[10] << endl;
	cout << "userEntries copies:" << times[11] << endl;


	cout << endl<< "TOTAL : " << times[12] << endl;

	cout << "Press Enter to continue." << endl;
	cin.get();
}
