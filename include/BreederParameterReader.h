//#include "/usr/local/MMB/include/simbody/SimTKcommon/internal/Subsystem.h"
#include <sys/types.h>
#include "DBManager.h"           
#include "SQLite.h"           
#include "MysqlConnection.h"
#include <sys/stat.h>
#include "Breed.h"
#include <iostream>
#include "Utils.h"
#include "ResidueStretchContainer.h"


#ifndef BreederParameterReader_H
#define BreederParameterReader_H


//#include "/home/samuelf/svn/RNAToolbox/trunk/include/ResidueStretchContainer.h"



class BreederParameterReader { 
    public:
        enum SQLTypes {SQLite , MySQL};
	DBManager *   dbConnection ; 
	std::string   pdbId ;
	bool          sequence;
	std::string   jobId ;
	std::string   listFileName ;
	std::string   msiScannedChains;
	std::string   msiPartnerChains  ;
	std::string   jobLibraryPath   ;
	std::string   workingDirectory;
	std::string   partition        ;
	std::string   chainsInMutatedSubunit ;//= "";   
        std::string   oneMutationString  ;//= "";
        std::string   wtRenumberedStructureFileName;//= "";
	double        msiRadius               ;//= -1111.0 ;         
	ListTypeEnum  listTypeEnum;
	//myListTypeEnum = unassigned;
	//myListTypeEnum = sequencesString;
	Breed breed;
	//breed.setGenerationSize( 3);
        SQLTypes sQLSystem ;//= SQLite; // Default
	std::string   server ;//= SERVER;
        std::string   user ;//= USER;
        std::string   database ;//= DATABASE;
        std::string   password ;//= PASSWORD; 
        std::string   account  ;//= ACCOUNT ; 
        std::string   foldXScript ;//= "";      
        std::string   SQLExecutable ;//= "";      
        std::string   SQLPassword   ;//= "";      
        std::string   foldXExecutable ;//= "";      
        std::string   breederMainDirectory ;//= "";      
        std::string   MMBExecutable ;//= ""  ; 
        std::string   emailAddress;//= ""  ; 
        std::string   singleMutantFilesDirectory ;//= "";
        std::string   pdbChains;                          
        double        mobilizerWithinRadius ;//= 0.0;
        double        waterRadius ;//= 0.0;
        double        maxTime ;//= 48;
        double        gromacsSimulationTime ;//= 0.0;
        double        reportingInterval     ;//= -1111.0;
        double        temperature;
        double        mutantAffinityInM;
        double        wildTypeAffinityInM;
        int           numReportingIntervals ;//= -1111;
        int           flexibilityWindowOffset;             
        int           lastStage;
        bool          ramp ;//= 0    ;
	//std::string arg ;//="";
        // std::string option ;//="";
        // homologyScanner parameters:
        std::string fastaExecutable = ""  ;    
        std::string fastaTempDirectory = ""  ;
        std::string breederExecutable;
        bool        singleHomologyScannerRun;
        std::string chainsInHomologComplex;
        std::string homologPdbId;

        void setDefaults ();
	void printUsage();
        void prepareMultiScanningInterfaceFile ();
        void readParameters (int myNumArguments, char * myArguments[]);
        int validate ();  // Limited validation.
        int validateForHomologyScanner (); // validation for parameter required by homologyScanner but not breeder.
        void updateDatabase();
        int  runBreeder();
};
#endif

