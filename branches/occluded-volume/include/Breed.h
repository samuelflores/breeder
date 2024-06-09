// Breed.h
#include "BiopolymerClass.h"
#include "Chromosome.h"
//#include "SQLite.h"           
#include "DBManager.h"
#include <fstream>
#include <sstream>
#include <string>
//#include <boost/lexical_cast.hpp>
//#define _OPEN_SYS

//#include <sys/types.h>
//#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef Breed_H
#define Breed_H

	enum ListTypeEnum {
		sequencesString,// The user will provide a list of mutant sequences.  We are not using this much anymore.
		mutationString, // This is for when an explicit list of mutants will be provided
		residues,	// This is intended for a MultiScanning run, in which only residue numbers will be provided, and all possible mutants generated.
		unassigned
	};
class Breed {

private :
	int generationSize ;

public:
	
	void spawn (Chromosome chromosome, DBManager & mysqlConnection); 	
	std::string selectRandomParent(DBManager & mysqlConnection ) ;
        void setGenerationSize(int myGenerationSize);	
        int getNumSequencesInDatabase (std::string sequence, DBManager & mysqlConnection);  
        int getNumSequencesInDatabase (Chromosome myChromosome, DBManager & mysqlConnection);  
	void spawnNewGeneration (DBManager & mysqlConnection );
        // This procedure spawns jobs from a specified list.  No search algorithm of any sort is employed.
	void spawnFromList    (DBManager & mysqlConnection , const char * listFileName, ListTypeEnum  myListTypeEnum) ;
};
#endif
