// SQLiteConnection.cpp
//#include "my_global.h" // Include this file first to avoid problems
#include <sqlite3.h>
#include <sys/stat.h>


#include <cstring>
#include "SQLite.h"          
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
	void SQLiteConnection::initialize (std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ) {
		runID.clear();
		server = (char*)myServer.c_str();//SERVER;     
		user = (char*)myUser.c_str(); //USER;   
		password = (char*)myPassword.c_str(); //PASSWORD; /* set me first */
		database = (char*)myDatabase.c_str(); //DATABASE;
		std::cout<<__FILE__<<":"<<__LINE__<<" user = "<<  user<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" database = "<<  database<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" password = "<<  password<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" server = "<<  server<<std::endl;
		int rc = sqlite3_open(database.c_str() , &db);
		if( rc ){
		      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		      //sqlite3_close(db); 
                      close();
		      exit(1);
		} else {
                      std::cout<<__FILE__<<":"<<__LINE__<<" Database opened successfully."<<std::endl;
                }

	}
        void SQLiteConnection::close() {
	   sqlite3_close(db);
	}
	//SQLiteConnection::SQLiteConnection(std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ){initialize(myServer,myPassword,myUser,myDatabase);}

        std::string SQLiteConnection::queryValueAsString (std::string queryString) {
		// send SQL query   
		std::cout<<__FILE__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" c_str() equivalent is : "<<queryString.c_str()<<std::endl;
                sqlite3_stmt *ppStmt; //const char **pzTail
                int result = sqlite3_prepare(db, queryString.c_str(),strlen(queryString.c_str())+1 ,  &ppStmt, NULL    );
                if (result != SQLITE_OK) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Failed to prepare database.  sqlite3_prepare returned error code: >"<<result<<"<"<<std::endl;
                        close();
			exit (1);
                }
                int stepReturnValue = sqlite3_step(ppStmt );
                if ((stepReturnValue == SQLITE_ROW) || (stepReturnValue == SQLITE_DONE ) ) {
                        std::string returnString;
			if ((stepReturnValue ) == SQLITE_ROW) {
			        returnString =  reinterpret_cast<const char*>(sqlite3_column_text  (ppStmt, 0))	;
				} else {
					std::cout<<__FILE__<<":"<<__LINE__<<" returned  SQLITE_DONE, not SQLITE_ROW.  Were you expecting a return value?"<<std::endl;
					returnString = "NO-RESULT-FROM-THIS-OPERATION-WERE-YOU-EXPECTING-ONE?";}
    				sqlite3_finalize(ppStmt);
				return returnString; 
                } else if ((stepReturnValue ) == SQLITE_BUSY ) { // If database is locked, try again:
                    std::cout<<__FILE__<<":"<<__LINE__<<" Pausing briefly to catch my breath ..."<<std::endl;
                    //sleep(2);
                    std::cout<<__FILE__<<":"<<__LINE__<<" Database is busy. Trying again.  If this continues for too long, the database is in some sense locked permanently."<<std::endl;
                    return queryValueAsString(queryString); // trying again, recursively.
                } else if (stepReturnValue == SQLITE_ERROR ){
                    std::cout<<__FILE__<<":"<<__LINE__<<" SQLITE_ERROR !  "<<std::endl;exit(1);
                } else if (stepReturnValue == SQLITE_MISUSE ){
                    std::cout<<__FILE__<<":"<<__LINE__<<" SQLITE_MISUSE ! "<<std::endl;exit(1);
                } else {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Some sort of error occurred.  No result found! "<<std::endl;exit(1);
                } 
	} ;

        std::string SQLiteConnection::runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType ) { 
                std::stringstream runJobStringStream;
                struct stat st; 
                std::string myJobID = getJobID();
                std::string databaseName = getDatabase() ;
                std::string myMMBExecutable = getMMBExecutable();
                std::string myFoldXScript =  getFoldXScript();
                std::string myFoldXExecutable = getFoldXExecutable     (); 
                std::string mySQLExecutable = getSQLExecutable     (); 
                if(stat(myMMBExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myMMBExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myMMBExecutable<<std::endl; exit(1);}
                if(stat(myFoldXScript .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myFoldXScript<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myFoldXScript<<std::endl; exit(1);}
                if(stat(myFoldXExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myFoldXExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myFoldXExecutable<<std::endl; exit(1);}
                if(stat(databaseName .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<databaseName<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<databaseName<<std::endl; exit(1);}
                if(stat(mySingleMutantDirectory .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<mySingleMutantDirectory<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<mySingleMutantDirectory<<std::endl; exit(1);}
                // TIMESTAMP for start of job
                runJobStringStream << "echo \"update results set startTime = (dateTime(\'now\',\'localtime\')) where jobName = \\\""<< myJobID  <<"\\\" and mutationString = \\\""<< mySingleMutantID  <<"\\\" ; \" | "<<mySQLExecutable <<" "<<databaseName<<"; " <<std::endl;;
                runJobStringStream << "echo \"update results set status = \\\"started\\\" where jobName = \\\""<< myJobID  <<"\\\" and mutationString = \\\""<< mySingleMutantID  <<"\\\" ; \" | "<<mySQLExecutable<<" "<<databaseName <<"; " <<std::endl;;
                runJobStringStream<<"cd "<<mySingleMutantDirectory<<std::endl;
                runJobStringStream<<myMMBExecutable<<" -c "<<mySingleMutantDirectory <<"/commands.dat &> "<<mySingleMutantDirectory<<"/mmb.out ;"<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<" About to write command to run "<<myFoldXScript<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<" with myFoldXExecutable =  "<<myFoldXExecutable<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<myFoldXScript<<" "<< myWorkingDirectory  <<" " <<  mySingleMutantID <<" "<< myJobID<<" "<< computeIndividualWildType<< " "<<myFoldXExecutable  <<   " "<<mySQLExecutable  <<  " "<<databaseName  <<  " &> "<<mySingleMutantDirectory<<"/foldx.out; "<<std::endl;
                runJobStringStream<<myFoldXScript<<" "<< myWorkingDirectory  <<" " <<  mySingleMutantID <<" "<< myJobID<<" "<< computeIndividualWildType<< " "<<myFoldXExecutable  <<   " "<<mySQLExecutable  <<  " "<<databaseName  <<  " &> "<<mySingleMutantDirectory<<"/foldx.out; "<<std::endl;
                runJobStringStream<<"rm trajectory.*.pdb.gz; gzip trajectory.*.pdb ;" <<std::endl;
                runJobStringStream << "echo \"update results set endTime = (dateTime(\'now\',\'localtime\')) where jobName = \\\""<< myJobID  <<"\\\" and mutationString = \\\""<< mySingleMutantID  <<"\\\" ; \" | "<<mySQLExecutable <<" "<<databaseName <<"; " <<std::endl;;
                return runJobStringStream.str();
        }   
