// MysqlConnection.h
#include "DBManager.h"
#include "mysql.h" // MySQL Include File
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#ifndef MysqlConnection_H
#define MysqlConnection_H




class MysqlConnection : public DBManager {
 	private:
                MYSQL *conn;
                MYSQL_RES *res;
                MYSQL_ROW row; //std::string SQLExecutable;
	public:
		void initialize (std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		//void close() { mysql_close(conn );}
		void close();
                MysqlConnection(std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  )  {initialize(myServer,myPassword,myUser,myDatabase);};
                std::string queryValueAsString (std::string queryString);
                std::string runGromacsSimulationString(std::string mySingleMutantID,  std::string mySingleMutantDirectory,  bool computeIndividualWildType );
                std::string runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType );
};
#endif
