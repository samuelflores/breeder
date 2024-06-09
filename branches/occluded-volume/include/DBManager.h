// SQLite.h            
#include <sqlite3.h>
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#ifndef DBManager_H
#define DBManager_H
#define SQLCommand "/usr/bin/sqlite3"
#define SERVER "" //"mysql.uppmax.uu.se"
#define USER "" //samuelf"
#define PASSWORD "" //"oirfy235k4ys"
#define DATABASE "/home/samuelf/svn/breeder/build/breeder.sqlite3"
#define ACCOUNT ""


class DBManager {
 	protected:
		std::string runID;
                sqlite3 *db; 
                char *server ;
                char *user ;
                char *password ; /* set me first */
                char *database ;
                std::string account;
                std::string MMBExecutable;
                std::string FoldXScript;
                std::string FoldXExecutable;
                std::string SQLExecutable;
                std::string singleMutantFilesDirectory;
                std::string breederMainDirectory      ;
                double      mobilizerWithinRadius;
                double      maxTime;
	public:
		// this is a function which issues a mysql query "queryString" and returns an answer of type myType
		//template <class myType>
		void setJobID(std::string myJobID);
		std::string getJobID();
		//void setPassword(std::string myPassword)  {password = myPassword; };
		void setAccountName(std::string myAccount )  {account = myAccount; };
		//std::string getPassword() {return password;};
		std::string getAccountName();
		void setMobilizerWithinRadius(double myRadius)  {mobilizerWithinRadius = myRadius ; };
		double getMobilizerWithinRadius() {return mobilizerWithinRadius;}
		void setMaxTime(double myRadius)  {maxTime = myRadius ; };
		double getMaxTime() {return maxTime;}
		std::string getDatabase() {return database;} ;
		void setMMBExecutable( std::string myMMBExecutable) { MMBExecutable = myMMBExecutable;} ;
		std::string getMMBExecutable() {return MMBExecutable;} ;
		void setFoldXScript  (std::string myFoldXScript) { FoldXScript = myFoldXScript ;} ;
		std::string getFoldXScript  () {return FoldXScript  ;} ;
		void setSQLExecutable  (std::string mySQLExecutable) { SQLExecutable = mySQLExecutable ;} ;
		std::string getSQLExecutable  () { return SQLExecutable ;} ;
		void setFoldXExecutable  (std::string myFoldXExecutable) { FoldXExecutable = myFoldXExecutable ;} ;
		std::string getFoldXExecutable  () {return FoldXExecutable  ;} ;
		std::string getWildTypeStructureFileName ();
		std::string getFoldXRunFileName ();
		std::string getSingleMutantFilesDirectory();
		void setSingleMutantFilesDirectory(std::string mySingleMutantFilesDirectory ) {singleMutantFilesDirectory = mySingleMutantFilesDirectory;};
		void setBreederMainDirectory(std::string myBreederMainDirectory ) {  breederMainDirectory = myBreederMainDirectory ;};
		std::string  getBreederMainDirectory( ) {  return breederMainDirectory ;};
		std::string getWorkingDirectory();
		double getReportingInterval();
		int getNumReportingIntervals();
		int getLastStage();
		std::string getJobName() ;
		int  getMaxGenerations();
		int  getMaxMutations  ();
		int  getGenerationSize();
		double getDecayConstantsBetweenHighestAndLowest ();
		double getMaximumParentEnergy ();
 		bool  isNull(std::string variableName);
		//double queryValueAsDouble (std::string queryString);
		std::string getWildTypeSequence();
		virtual void initialize (std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		//void close() { mysql_close(conn );}
		virtual void close();
		void validate();
		DBManager(   ){}; // Default constructor more or less required for inheritance
		DBManager(std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		std::string getMysqlCommandString();
		void insertUpdateDelete(std::string queryString );
		void executeStatement ( std::string queryString); //, bool resultIsExpected = true);
		double queryValueAsDouble (std::string queryString);
		int    queryValueAsInt    (std::string queryString);
		virtual std::string queryValueAsString (std::string queryString);
                bool acceptanceTest (double energy);
		int getNumJobs(std::string status);
		std::string retrieveSequenceAtRandom();
		std::string retrieveSequenceInOrder(int & index, std::string orderField , std::string order);
                virtual std::string runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType) {std::cout<<__FILE__<<":"<<__LINE__<<" Never call this function in the parent!"<<std::endl; exit(1);}
};
#endif
