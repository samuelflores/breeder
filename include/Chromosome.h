// Chromosome.h
#include "BiopolymerClass.h"
//#include <cstdlib>
//#include <boost/lexical_cast.hpp>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include "BiopolymerClass.h"
//#include "Mutation.h"    
//#include "SQLite.h"
#include "DBManager.h"
//#include <locale>
#include <stdlib.h>
#include <ctype.h>
#ifndef Chromosome_H
#define Chromosome_H
class Chromosome {
private:
	std::string wildTypeSequence ;
	//std::string sequence;
	std::string sequences;
	std::string chainID1;
	int firstResidue;
	int lastResidue;
	std::vector<int> mutablePositions; 
        //std::vector<Mutation> mutations;
	float mutationRate;
	int maxMutations ;
     
	std::vector <std::string> aminoAcidTypes;//("A" , "V", "I",    "L", "M", "P" ,    "F" ,"W",      "C", "N","Q", "G","T", "Y", "S",      "D", "E"  , "K", "R", "H"   );
public:
        BiopolymerClassContainer biopolymerClassContainer;
	void initialize() ;
	void validateFirstResidue();
	void setMaxMutations(int myMaxMutations);
        void setWildTypeSequence (std::string myWildTypeSequence);
	//void setSequence (const std::string newSequence); 
	void setSequences (const std::string newSequences);
	void setMutationVectorFromString (const std::string mutationString);
	//std::string getWildTypeSequence ();
	//String getFormattedSequencesString() ;
	std::string getSequences ();
        void setFirstResidue (int myFirstResidue);
        void setBiopolymerClassContainer (BiopolymerClassContainer myBiopolymerClassContainer) {biopolymerClassContainer =myBiopolymerClassContainer; };
        BiopolymerClassContainer getBiopolymerClassContainer () {return biopolymerClassContainer ; };
        BiopolymerClassContainer & updBiopolymerClassContainer () {return biopolymerClassContainer ; };
        //int getFirstResidue ();
        //int getFirstResidue1 ();
	std::string crossOverAtPosition(std::string parentSequence1 , std::string parentSequence2, int crossOverPoint);
	//int numMutations(std::string inputSequence);
        
        int numMutations();
	bool validate( );
        //bool validate (std::string inputSequence);
	std::string crossOver(std::string parentSequence1 , std::string parentSequence2); //, bool success);
	std::string mutate(int mutationPosition);
	void mutate ();

	void loadMutations ();
	Chromosome();
	Chromosome(std::string parentSequence1 , std::string parentSequence2, int myMaxMutations /*, int myFirstResidueNumber*/, BiopolymerClassContainer myBiopolymerClassContainer);
	Chromosome(const std::string newSequences,  int myMaxMutations ,  BiopolymerClassContainer myBiopolymerClassContainer);
	Chromosome( std::string mutationString,  BiopolymerClassContainer myBiopolymerClassContainer);
        Chromosome(int myMaxMutations ,  BiopolymerClassContainer  myBiopolymerClassContainer );
	void submitJob();
	int countMutations (std::string string1 , std::string string2);
	std::string getSingleMutantID();
        std::string getMutationString();
        std::string getPdbNumberedMutationString(DBManager & mysqlConnection);
	//std::string getResidueIDAsString(int residueIndex) ;
        std::string getChainID1 () ;
        void prepareSingleMutant( DBManager & mysqlConnection , std::string singleMutantDirectory, bool computeIndividualWildType);
    	//void resetMutationsVectorFromWildTypeSequence();
	//Chromosome getWildTypeChromosome();
        void prepareSingleMutantDirectory(DBManager & mysqlConnection);
	void writeCommonPart(std::ofstream & output, DBManager & mysqlConnection);

	void writeChains(std::ofstream & output);
        /**
        ** \brief This writes commands to flexibilize around mutant. offset = number of residues to flexibilize on each side of mutant. radius  is to be used in the command: "applyMobilizersWithin Default <radius> <chain> <residue>"
        **/
	void writeMutationFlexibilizers(std::ofstream & output, const int offset, const double radius); 
	void writeWaterDroplets(std::ofstream & output, DBManager & mysqlConnection  ); 

	void writePhysicsZones(std::ofstream & output, const int offset);


	void writeSubstituteResidueCommands(std::ofstream & output);

	void writeCommandFile(std::ofstream & output, const int offset,DBManager & mysqlConnection);
	bool hasBeenComputed(DBManager & mysqlConnection)	;
	bool checkIndividualComputation(DBManager & mysqlConnection);
	bool allMutationsDifferFromWildType ();
	void revertToWildTypeEquivalent();
        void populateSequenceTable (DBManager & mysqlConnection);
};

#endif

