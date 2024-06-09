// MysqlConnection.cpp
//#include "my_global.h" // Include this file first to avoid problems
#include "mysql.h" // MySQL Include File
#include <sys/stat.h>



#include "MysqlConnection.h"
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
	void MysqlConnection::initialize (std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ) {
		runID.clear();
                std::cout<<__FILE__<<":"<<__LINE__<<" Just cleared runID. Now contents are : >"<<runID<<"<."<<std::endl;
		server = (char*)myServer.c_str();//SERVER;     
		user = (char*)myUser.c_str(); //USER;   
		password = (char*)myPassword.c_str(); //PASSWORD; /* set me first */
                std::cout<<__FILE__<<":"<<__LINE__<<" Initializing with database = >"<<myDatabase<<"< "<<std::endl;
		database = (char*)myDatabase.c_str(); //DATABASE;
                std::cout<<__FILE__<<":"<<__LINE__<<" Initializing with database = >"<<database<<"< "<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" user = "<<  user<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" database = "<<  database<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" password = "<<  password<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" server = "<<  server<<std::endl;
		conn = mysql_init(NULL);
		if (!mysql_real_connect(conn, server.c_str(),
		   user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0)) {
		   fprintf(stderr, "%s\n", mysql_error(conn));
		   exit(1);
		}
	}

        void MysqlConnection::close() {
	   mysql_close(conn);
	}

	//MysqlConnection::MysqlConnection(std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ){initialize(myServer,myPassword,myUser,myDatabase);}

	std::string MysqlConnection::queryValueAsString (std::string queryString ) {
		// from : http://www.cyberciti.biz/tips/linux-unix-connect-mysql-c-api-program.html
		// Connect to database 
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		// send SQL query   
		std::cout<<__FILE__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" c_str() equivalent is : "<<queryString.c_str()<<std::endl;
		if (mysql_query(conn, queryString.c_str() )) { // If this returns non-zero, then there was an error.  Otherwise, query went OK.
		        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			fprintf(stderr, "%s\n", mysql_error(conn));
			exit(1);
		}
                unsigned int fieldCount = mysql_field_count(conn);
		std::string returnString;
                std::cout<<__FILE__<<":"<<__LINE__<<" Returned "<<fieldCount << " fields"<<std::endl;
                if ( fieldCount  == 0) {
			std::cout<<__FILE__<<":"<<__LINE__<<" Query returned nothing.  If this was an insert/update/delete operation, this should be OK. Otherwise something is probably wrong."<<std::endl;
			returnString = "NO-RESULT";
                } else if (fieldCount > 1) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Returned "<<fieldCount<<" fields.  Expected 0 or 1!"<<std::endl; exit(1);
                }
                else if (fieldCount == 1){
			MYSQL_RES * myResult ;
                        myResult = mysql_use_result(conn);
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			MYSQL_ROW returnRow ;
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                        if ((returnRow = mysql_fetch_row(myResult)) == NULL) {
                            std::cout<<__FILE__<<":"<<__LINE__<<" Result is NULL"<<std::endl;   
                            return std::string("");
                        } else {
			    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                            if (returnRow[0] == NULL){std::cout<<__FILE__<<":"<<__LINE__<<" Query : "<<queryString<<std::endl<<" appears to return a NULL. Using empty string \"\" instead."<<std::endl; returnString = "";}
                            else returnString =  std::string(returnRow[0]);    
                            //if (returnString == NULL){std::cout<<__FILE__<<":"<<__LINE__<<" Got a NULL value."<<std::endl;}
			    std::cout<<__FILE__<<":"<<__LINE__<<" returning >"<<returnString<<"< "<<std::endl;
                        };
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                        //returnString = std::string(returnRow[0]);    
                         
			std::cout<<__FILE__<<":"<<__LINE__<<"Query returned >"<<returnString<<"< "<<std::endl;
			while ((returnRow = mysql_fetch_row(myResult)) != NULL) { // have to flush rows
				std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
				std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
				printf("%s \n", returnRow[0]);
			}
                        //if (returnRow == NULL){std::cout<<__FILE__<<":"<<__LINE__<<" Got a NULL value."<<std::endl;}
			mysql_free_result(myResult);
                }
		std::cout<<__FILE__<<":"<<__LINE__<<" Returning : >"<<returnString<<"< "<<std::endl;
		return returnString;
			
	};
	std::string MysqlConnection::runGromacsSimulationString(std::string mySingleMutantID,  std::string mySingleMutantDirectory,  bool computeIndividualWildType ) {
                std::stringstream runGromacsSimulationStringStream;
                //  Put all GROMACS commands here :
                std::string gromacsCommandFileName = getSingleMutantFilesDirectory() + std::string("/gromacs-commands.txt");
		std::ifstream gromacsCommandFile(gromacsCommandFileName );
                runGromacsSimulationStringStream
                        <<"# From here down, This file is generated in " <<__FILE__<<":"<<__LINE__<<std::endl 
			//<< "module add gromacs/5.1.1 " <<std::endl 
			<<"cd "<<mySingleMutantDirectory<<std::endl
			<<"echo \" now working in : "<<mySingleMutantDirectory<<"\" "<<std::endl;

		// Copy over all the files GROMACS will need
		
		std::string singleMutantFilesDirectory = getSingleMutantFilesDirectory(); //t.ex. "/bubo/home/h17/samuelf/svn/breeder/singleMutantFiles";
		std::string ions_mdp_String   = singleMutantFilesDirectory + "/ions.mdp"   ;
		std::string md_mdp_String     = singleMutantFilesDirectory + "/md.mdp"   ;
		std::string mdout_mdp_String  = singleMutantFilesDirectory + "/mdout.mdp"   ;
		std::string minim_mdp_String  = singleMutantFilesDirectory + "/minim.mdp"   ;
		std::string npt_mdp_String    = singleMutantFilesDirectory + "/npt.mdp"   ;
		std::string nvt_mdp_String    = singleMutantFilesDirectory + "/nvt.mdp"   ;
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		struct stat st;
		if(stat(ions_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<ions_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<ions_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		if(stat(md_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<md_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<md_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		if(stat(mdout_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<mdout_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<mdout_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		if(stat(minim_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<minim_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<minim_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		if(stat(npt_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<npt_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<npt_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		if(stat(nvt_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<nvt_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<nvt_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
		runGromacsSimulationStringStream<<  "cp " + ions_mdp_String   + " "  +  mySingleMutantDirectory + " ; "<<std::endl ;
		runGromacsSimulationStringStream<<  "cp " + md_mdp_String     + " "  +  mySingleMutantDirectory + " ; "<<std::endl;
		runGromacsSimulationStringStream<<  "cp " + mdout_mdp_String  + " "  +  mySingleMutantDirectory + " ; "<<std::endl;
		runGromacsSimulationStringStream<<  "cp " + minim_mdp_String  + " "  +  mySingleMutantDirectory + " ; "<<std::endl;
		runGromacsSimulationStringStream<<  "cp " + npt_mdp_String    + " "  +  mySingleMutantDirectory + " ; "<<std::endl;
		runGromacsSimulationStringStream<<  "cp " + nvt_mdp_String    + " "  +  mySingleMutantDirectory + " ; "<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                

		if ( gromacsCommandFile )
	        {
                    runGromacsSimulationStringStream << "# Write cluster-specific configuration and module load commands.. "<<std::endl;
                    runGromacsSimulationStringStream << "# The following portion is being read from >"<<gromacsCommandFileName<<"< "<<std::endl;
		    runGromacsSimulationStringStream << gromacsCommandFile.rdbuf();
		    gromacsCommandFile.close();
                    runGromacsSimulationStringStream << "# End portion from >"<<gromacsCommandFileName<<"< "<<std::endl;
	        } 
                else {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Unable to open the gromacs command file : >"<<gromacsCommandFileName<<"< "<<std::endl;
                    exit(1);
                }
                    /*
                runGromacsSimulationStringStream  
			<<"echo 6 > temp.txt"<<std::endl
			<<"echo 1 >> temp.txt"<<std::endl
			<<""<<std::endl
			<<"cat temp.txt | srun -n 1 gmx_mpi pdb2gmx -f zemu-equilibrated-truncated.pdb -o zemu-equilibrated-truncated_processed.gro -ignh"<<std::endl
			<<"#6: Amber sb99 , 3 point TIP3P water model: force field was selected based on this benchmark article: http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2905107/"<<std::endl
			<<"srun  gmx_mpi editconf -f zemu-equilibrated-truncated_processed.gro -o zemu-equilibrated-truncated_newbox.gro -c -d 1.0 -bt cubic"<<std::endl
			<<"srun  -n 1 gmx_mpi solvate -cp zemu-equilibrated-truncated_newbox.gro -cs spc216.gro -o zemu-equilibrated-truncated_solv.gro -p topol.top"<<std::endl
			<<""<<std::endl
			<<"srun  gmx_mpi grompp -f ions.mdp -c zemu-equilibrated-truncated_solv.gro -p topol.top -o ions.tpr"<<std::endl
			<<"echo 13 | srun -n 1 gmx_mpi genion -s ions.tpr -o zemu-equilibrated-truncated_solv_ions.gro -p topol.top -pname NA -neutral " <<std::endl
			<<"srun  -n 1gmx_mpi grompp -f minim.mdp -c zemu-equilibrated-truncated_solv_ions.gro -p topol.top -o em.tpr"<<std::endl
			<<"srun  gmx_mpi mdrun -v -deffnm em"<<std::endl
			<<""<<std::endl
			<<"srun  -n 1 gmx_mpi grompp -f nvt.mdp -c em.gro -p topol.top -o nvt.tpr"<<std::endl
			<<"srun  gmx_mpi mdrun -deffnm nvt"<<std::endl
			<<"srun  -n 1 gmx_mpi grompp -f npt.mdp -c nvt.gro -t nvt.cpt -p topol.top -o npt.tpr"<<std::endl
			<<"srun  gmx_mpi mdrun -deffnm npt"<<std::endl
			<<"srun  -n 1 gmx_mpi grompp -f md.mdp -c npt.gro -t npt.cpt -p topol.top -o md_0_1.tpr"<<std::endl
			<<"srun  gmx_mpi mdrun -deffnm md_0_1"<<std::endl
			<<"# insert DSSP commands here"<<std::endl ;*/
                        // Comment out the above, and uncomment the below, for restart runs.
                        //<<"srun  gmx_mpi mdrun -deffnm md_0_1 -cpi md_0_1.cpt"<<std::endl ;

                runGromacsSimulationStringStream        <<"# End of GROMACS command generator." <<__FILE__<<":"<<__LINE__<<std::endl ;
         
                runGromacsSimulationStringStream <<"# Returning from " <<__FILE__<<":"<<__LINE__<<std::endl ;
                return runGromacsSimulationStringStream.str(); 
        };
        // This is called from Breed.cpp : spawn .. where the job file is opened.
	std::string MysqlConnection::runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType ) {
			std::stringstream runJobStringStream;
			struct stat st;
                std::string myJobID = getJobID();
                std::string databaseName = getDatabase() ;
                std::cout<<__FILE__<<":"<<__LINE__<<" database is >"<< getDatabase()<<"< "<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<" database is >"<<databaseName<<"< "<<std::endl;
                std::string myMMBExecutable = getMMBExecutable();
                std::string myFoldXScript =  getFoldXScript();
                std::string myFoldXExecutable = getFoldXExecutable     ();
                std::string mySQLExecutable = getSQLExecutable     ();
                std::string mySQLExecutableAndParameters = mySQLExecutable + std::string(" --defaults-extra-file=") +  getSingleMutantFilesDirectory() + std::string("/option-file.mysql");
                std::string myChainsInMutatedSubunit = getChainsInMutatedSubunit();
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<"getChainsInMutatedSubunit() = >"<<myChainsInMutatedSubunit<<"< "<<std::endl;
                int myLastStage = getLastStage();
                std::cout<<__FILE__<<":"<<__LINE__<<" getLastStage() = "<<myLastStage<<std::endl;
                if(stat(myMMBExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myMMBExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myMMBExecutable<<std::endl; exit(1);}
                if(stat(myFoldXScript .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myFoldXScript<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myFoldXScript<<std::endl; exit(1);}
                if(stat(myFoldXExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<myFoldXExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<myFoldXExecutable<<std::endl; exit(1);}
                if(stat(mySingleMutantDirectory .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<mySingleMutantDirectory<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<mySingleMutantDirectory<<std::endl; exit(1);}
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"# Start of runJobString call. This is written at "<<__FILE__<<":"<<__LINE__<<std::endl; 
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                // TIMESTAMP for start of job
                runJobStringStream<<"# "<<__FILE__<<":"<<__LINE__<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
               

                runJobStringStream<<"# "<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"echo \"update results set status = \\\"started\\\" where jobName = \\\""<< myJobID  <<"\\\" and pdbId = \'"<< getPdbId()  <<"\' and mutationString = \\\""<< mySingleMutantID  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"cd "<<mySingleMutantDirectory<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"# "<<__FILE__<<":"<<__LINE__<<std::endl;
                // remove # to restore normal function
                runJobStringStream<<myMMBExecutable<<" -c "<<mySingleMutantDirectory <<"/commands.dat > "<<mySingleMutantDirectory<<"/mmb.out ;"<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"# "<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<               myFoldXScript<<" "<< myWorkingDirectory  <<" " <<  mySingleMutantID <<" "<< myJobID<<" "<< computeIndividualWildType<< " "<<myFoldXExecutable  << " "<< mySQLExecutableAndParameters<<" "<<myChainsInMutatedSubunit<<" "<< myLastStage  << " "<< getPdbId() <<" "<< getTemperatureInK() << " &> "<<mySingleMutantDirectory<<"/run-foldx.out; "<<std::endl;//  ; " <<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"# "<<__FILE__<<":"<<__LINE__<<std::endl;
                // can put this back later after we verify all is OK :
                //runJobStringStream<<" GLOBIGNORE=raw_Repair_1.pdb ; rm -v * ; unset GLOBIGNORE ; " <<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                runJobStringStream<<"# End of runJobString call. This is written at "<<__FILE__<<":"<<__LINE__<<std::endl; 
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                
                return runJobStringStream.str();
        }


