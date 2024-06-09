// SQLite.h            
#include "DBManager.h"
#include <sqlite3.h>
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#ifndef SQLite_H
#define SQLite_H

#define SQLCommand "/usr/bin/sqlite3"
#define SERVER "" //"mysql.uppmax.uu.se"
#define USER "" //samuelf"
#define PASSWORD "" //"oirfy235k4ys"
#define DATABASE "/home/samuelf/svn/breeder/build/breeder.sqlite3"
#define ACCOUNT ""


class SQLiteConnection : public DBManager {
 	private:
                sqlite3 *db; 
                std::string SQLExecutable;
	public:
		void initialize (std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		void close();
		SQLiteConnection(std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  )  {initialize(myServer,myPassword,myUser,myDatabase);};
		std::string queryValueAsString (std::string queryString);
                std::string runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType );
};
#endif
