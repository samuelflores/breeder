// ihromosome.cpp
#include <sys/stat.h>
#include "Chromosome.h"
//#include "SQLite.h"
#include "DBManager.h"
//#include <lexical_cast.hpp>
//#include <boost/lexical_cast.hpp>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
//#include "BreederUtils.h"
//#include <locale>
#include "Utils.h"
#include <stdlib.h>
#include <ctype.h>
	void Chromosome::initialize() {
		wildTypeSequence = "";
		//sequence = "";
		sequences = "";
		chainID1 = "";
		firstResidue = -1111;
		lastResidue = -1111;
		maxMutations= -1111;
		//mutations.clear();
	}
	void Chromosome::validateFirstResidue() {
		if (firstResidue < 0) {
			std::cout<<__FILE__<<":"<<__LINE__<<"  Bad first residue ID "<<std::endl; exit(1);
		} else {
			std::cout<<__FILE__<<":"<<__LINE__<<"  First residue = "<<firstResidue<<std::endl;
		}
	};
	void Chromosome::setMaxMutations(int myMaxMutations) {
		if (myMaxMutations < 0) {
			std::cout<<__FILE__<<":"<<__LINE__<<" You have specified maxMutations = "<<myMaxMutations<<".  But this cannot be negative!"<<std::endl; exit(1);
		}
		maxMutations = myMaxMutations;};
        void Chromosome::setWildTypeSequence (std::string myWildTypeSequence) {
		std::cout<<__FILE__<<":"<<__LINE__<<" This call is obsolete! this is controlled by the Chromosome's biopolymerClassContainer."<<std::endl; 
		exit(1);
		wildTypeSequence = myWildTypeSequence; } 
	/*void Chromosome::setSequence (const std::string newSequence) {
		std::cout<<__FILE__<<":"<<__LINE__<<" This call is obsolete! use setSequences, and use the standard format."<<std::endl; exit(1);
		sequence = newSequence;
	}*/
	void Chromosome::setSequences (const std::string newSequences) {
		//mySequences = newSequences;
		size_t colonPosition = newSequences.find(':');
		size_t dotPosition = newSequences.find('.');
		string myChainID = newSequences.substr(0,colonPosition);
		string mySequence = newSequences.substr(colonPosition + 1, (dotPosition - colonPosition - 1));
		std::cout<<__FILE__<<":"<<__LINE__<<" Parsing "<<newSequences<<". first chain = "<<myChainID<<" with sequence "<<mySequence<<std::endl;
		if (!(biopolymerClassContainer.hasChainID(myChainID))) {
			std::cout<<__FILE__<<":"<<__LINE__<<" Could not find a chain  "<<myChainID<<std::endl; exit(1);
		}
		biopolymerClassContainer.updBiopolymerClass(myChainID).changeSequence( mySequence);
		std::cout<<__FILE__<<":"<<__LINE__<<" newSequences, dotPosition, newSequences.length() = "<<newSequences<<", "<<dotPosition<<", "<<newSequences.length()<<std::endl;
                if  (dotPosition == string::npos) std::cout<<__FILE__<<":"<<__LINE__<<" dotPosition == string::npos "<<std::endl;
		if ((dotPosition < (sequences.length()-1))  && 
			(dotPosition != string::npos)) // dotPosition == string::npos indicates no dot was found
		{ // recursively send rest of string to the same setSequences function, to take care of remaining chains.
			std::cout<<__FILE__<<":"<<__LINE__<<" About to reissue setSequences iteratively with argument "<<newSequences.substr(dotPosition +1, 100000000)<<std::endl;
			setSequences(newSequences.substr(dotPosition +1, 100000000)) ; // a ridiculously large value takes everything to the right of dotPosition +1.
		}
	}


	void Chromosome::setMutationVectorFromString (const std::string mutationString) {
		std::cout<<__FILE__<<":"<<__LINE__<<" Parsing "<<mutationString<<std::endl;
		//size_t minorSeparatorPosition1 = mutationString.find(MUTATIONMINORSEPARATOR); // find the start position of residue ID
		//size_t minorSeparatorPosition2 = mutationString.find(MUTATIONMINORSEPARATOR, (minorSeparatorPosition1 + 1)); // find the end position of residue ID
		//size_t dotPosition = mutationString.find(MUTATIONMAJORSEPARATOR); // find the end position of the mutant record

                size_t dotPosition = mutationString.find(','); // If we find a comma, then this is a SKEMPI formatted mutation string.
                if (dotPosition == std::string::npos){dotPosition = mutationString.find(MUTATIONMAJORSEPARATOR); } // If there was no comma, then we are looking for a dot '.', because this could be a breeder-formatted mutation string. Or it could be a single-substitution mutant in SKEMPI format.

		//if (minorSeparatorPosition2 > dotPosition) {std::cout<<__FILE__<<":"<<__LINE__<<" Ill-formatted mutation string! Remember to use the format X"<<MUTATIONMINORSEPARATOR<<"NNI"<<MUTATIONMINORSEPARATOR<<"S .. where X is the chain ID, NNI is the residue ID and insertion code (if any) and S is the substituted residue type. Separate the mutants with " <<"."<< mutationString<<std::endl; exit(1);}  
		string mySingleMutationString = mutationString.substr(0,dotPosition);  
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		//ResidueID mutantResidueID = biopolymerClassContainer.updBiopolymerClass(myChainID).residueID( residueIDString ) ; // convert residueIDString to a real ResidueID
		Mutation myMutation;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                myMutation.setChainSubstitutionFromSingleMutationString(mySingleMutationString); // This method automatically detects whether we are using breeder or SKEMPI formatted mutation string, and parses accordingly.
                if (!(biopolymerClassContainer.hasChainID(myMutation.getChain()))) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" BiopolymerClass does not have a chain  "<<myMutation.getChain()<<std::endl; exit(1);
                }
                std::cout<<__FILE__<<":"<<__LINE__<<" adding the following mutation to the mutation vector of chain >"<<myMutation.getChain()<<"< :"<<std::endl;
                myMutation.print();
		biopolymerClassContainer.addMutationToVector(myMutation );
		std::cout<<__FILE__<<":"<<__LINE__<<" biopolymerClassContainer.getNumMutationVectorElements() "<< biopolymerClassContainer.getNumMutationVectorElements() << std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" biopolymerClassContainer.getFormattedMutationsString() "<< biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR )<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<" biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR) >"<< biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR)<<"< "<<std::endl;
		if (dotPosition == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" Bad format! "<<std::endl; exit(1); }
		if ((dotPosition < (mutationString.length()-1))  && 
			(dotPosition != string::npos)) // dotPosition == string::npos indicates no dot was found
		{ // recursively send rest of string to the same setMutationVectorFromString function to take care of remaining mutations.
			std::string newMutationString = mutationString.substr((dotPosition+1), string::npos);
			setMutationVectorFromString(newMutationString);
		}
	}


	std::string Chromosome::getSequences () {
                //std::cout<<__FILE__<<":"<<__LINE__<<" This call is obsolete!  Make calls to the biopolymerClassContainer."<<std::endl; exit(1);
                std::cout<<__FILE__<<":"<<__LINE__<<" biopolymerClassContainer.getFormattedSequencesString() = "<<biopolymerClassContainer.getFormattedSequencesString()<<std::endl;
		if ( biopolymerClassContainer.getFormattedSequencesString().length() ==0) {
			std::cout<<__FILE__<<":"<<__LINE__<<" The current sequences is of length 0!"<<std::endl; exit(1);}
		return biopolymerClassContainer.getFormattedSequencesString() ;
 	}
        std::string Chromosome::crossOverAtPosition(std::string parentSequence1 , std::string parentSequence2, int crossOverPoint) {
		if (parentSequence1.length() != parentSequence2.length()) {
			std::cout<<__FILE__<<":"<<__LINE__<<" the given sequence strings are different lengths!"<<std::endl; exit (1);
		}
		std::string childString = parentSequence1.substr(0,crossOverPoint) + parentSequence2.substr((crossOverPoint),std::string::npos); // recombine the two sequence strings
		if (parentSequence1.length() != childString.length()) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" parentSequence1 = >"<<parentSequence1<<"<"<<std::endl;
                        std::cout<<__FILE__<<":"<<__LINE__<<" childString     = >"<<childString<<"<"<<std::endl;
			std::cout<<__FILE__<<":"<<__LINE__<<" the child sequence string is of the wrong length!"<<std::endl; exit (1);
		}
		return childString;
	}
        int Chromosome::numMutations(){
		return biopolymerClassContainer.getNumMutationVectorElements();
		//return numMutations(getSequence());
        }
 
	bool Chromosome::validate( ) {

 		std::cout<<__FILE__<<":"<<__LINE__<<" Validating Chromosome .. " <<std::endl;
 		std::cout<<__FILE__<<":"<<__LINE__<<" Checking number of mutations: "<<numMutations()     <<std::endl;
 		std::cout<<__FILE__<<":"<<__LINE__<<" Checking mutation string: "<<biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR)<<std::endl; // don't use ::getSingleMutantID() because this kills the job if mutationString is of zero length. In this validate() procedure the Chromosome is allowed to be wild type.
		if (biopolymerClassContainer.getNumBiopolymers() < 1)	{
			std::cout<<__FILE__<<":"<<__LINE__<<" Got "<<biopolymerClassContainer.getNumBiopolymers()<<" biopolymers.  This can't be right!"<<std::endl;
			exit(1);
			return false;  // pointless right now, since we're killing when it's false
		}
		return true;
		//return validate(getSequence());
        };
	std::string Chromosome::crossOver(std::string parentSequence1 , std::string parentSequence2 /*bool success*/ ) {
                //success = false ;
		if (!(
			( parentSequence1.length()) ==
			( parentSequence2.length())
			)) {
                	std::cout<<__FILE__<<":"<<__LINE__<<" You are trying to use invalid parents:" << parentSequence1<<", "<<parentSequence2<<" ! These have different lengths"<<std::endl; exit(1);
		}
		if (parentSequence1.compare(parentSequence2) == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" parent sequences are identical!"<<std::endl; exit(1);}
                std::cout<<__FILE__<<":"<<__LINE__<<" About to do crossover operation .."<<std::endl;
                if (parentSequence1.length() != parentSequence2.length()) {std::cout<<__FILE__<<":"<<__LINE__<<" The supplied sequences are of lenghts "<<parentSequence1.length()<< " and "<<parentSequence2.length()<<std::endl; exit(1);}
		int firstSequenceDifferencePosition = 0;
		int lastSequenceDifferencePosition = 0;
                for (int i = 0 ; i < parentSequence1.length(); i++ ) {
                    if (parentSequence1[i] != parentSequence2[i]) {firstSequenceDifferencePosition = i ; break; }  
                }
                for (int i = 0 ; i < parentSequence1.length(); i++ ) {
                    if (parentSequence1[i] != parentSequence2[i]) { lastSequenceDifferencePosition = i; } 
                }
		std::cout<<__FILE__<<":"<<__LINE__<<" firstSequenceDifferencePosition = "<<firstSequenceDifferencePosition<<std::endl; 
		std::cout<<__FILE__<<":"<<__LINE__<<" lastSequenceDifferencePosition  = "<<lastSequenceDifferencePosition<<std::endl; 
                if (lastSequenceDifferencePosition == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" Error! no sequence difference found!"<<std::endl; exit(1);}
                if ((lastSequenceDifferencePosition - firstSequenceDifferencePosition) <= 0) {std::cout<<__FILE__<<":"<<__LINE__<<" Warning: sequences differ at one position only! Offspring will be identical to one parent."<<std::endl; }

		// Limit crossovers to region that is actually mutated:
		//    The 0.5 turns the floor to a 'round' operation.
                int randomResidue = floor( double(rand()) / double(RAND_MAX) * (lastSequenceDifferencePosition-firstSequenceDifferencePosition -1 ) +.5) + (firstSequenceDifferencePosition+1);
                std::cout<<__FILE__<<":"<<__LINE__<<" rand() RAND_MAX "<<rand()<<", "<< RAND_MAX<< ", rand() / RAND_MAX = "<<double(double(rand())/ double(RAND_MAX))<<  std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<" randomResidue                   = "<<randomResidue<<std::endl;
                std::cout<<__FILE__<<":"<<__LINE__<<" parentSequence1     = "<<parentSequence1<<std::endl; //" parentSequence2 = "<<parentSequence2<<" . Crossover will be at residue index = "<<randomResidue<<std::endl; 
                std::cout<<__FILE__<<":"<<__LINE__<<" parentSequence2     = "<<parentSequence2<<std::endl;//" parentSequence2 = "<<parentSequence2<<" . Crossover will be at residue index = "<<randomResidue<<std::endl; 
		std::string newSequences = crossOverAtPosition(parentSequence1, parentSequence2, randomResidue); 
                std::cout<<__FILE__<<":"<<__LINE__<<" Offspring sequences = "<<newSequences<<std::endl;
                setSequences(newSequences);
                biopolymerClassContainer.loadMutationVectorsFromSequence();
                return newSequences;
	}
	std::string Chromosome::mutate(int mutationPosition) {
                std::cout<<__FILE__<<":"<<__LINE__<<" Dont' use this yet .. it's not clear that it works with sequences"<<std::endl; exit(1); /*
		std::cout<<__FILE__<<":"<<__LINE__<<" sequence before mutation at position "<<mutationPosition<<" is: "<< sequences<<std::endl;
		sequence = sequence.substr(0, mutationPosition) + aminoAcidTypes[int(20*rand())] +  sequence.substr(mutationPosition +1 , std::string::npos);
		std::cout<<__FILE__<<":"<<__LINE__<<" sequence after mutation at position "<<mutationPosition<<" is: "<< sequence<<std::endl;*/
		return sequences;
	}
	void Chromosome::mutate (){
		for (unsigned int i = 0; i < mutablePositions.size(); i++) {
			if (rand() < mutationRate) {
				mutate(i);
			}
		}
	};

	void Chromosome::loadMutations ()  { // clears and loads mutations vector based on getSequence() and getWildTypeSequence().
		std::cout<<__FILE__<<":"<<__LINE__<<" This is an obsolete method!"<<std::endl; exit(1);
		/*int myNumMutations = 0;
		mutations.clear();
		
                for (unsigned int i = 0 ; i < getSequence().size() ; i ++) {
                        if (getSequence().substr(i,1).compare(getWildTypeSequence().substr(i,1)) != 0 ) { 
                                std::cout<<"Found a mutation at residue index "<<i<<std::endl; 
			Mutation myMutation;
			myMutation.setChainID("C");
			myMutation.setResidueNumber(i+getFirstResidue());
			myMutation.setSubstitutedResidueType(getSequence().substr(i,1));
			mutations.push_back(myMutation); 
                                myNumMutations++;
                        } else { // do nothing
                        }   

                }
		if   (myNumMutations != biopolymerClassContainer.getNumMutationVectorElements() ) {
			std::cout<<" The number of mutations found does not match an internal count! "<<std::endl; exit(1);
		}*/
	};
	Chromosome::Chromosome(std::string parentSequence1 , std::string parentSequence2,  int myMaxMutations /*, int myFirstResidueNumber*/, BiopolymerClassContainer myBiopolymerClassContainer){
                initialize();
		biopolymerClassContainer = myBiopolymerClassContainer;
		biopolymerClassContainer.setOriginalSequencesFromCurrentSequences();
		//setFirstResidue(myFirstResidueNumber);
		setMaxMutations(myMaxMutations);
                if (parentSequence1.compare(parentSequence2) == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" You have tried to cross two identical parents.  This is not possible!"<<std::endl; exit(1); } 
                
		crossOver(parentSequence1, parentSequence2);
		mutate();
		//loadMutations();
		validate();
	}
	Chromosome::Chromosome() {};
	Chromosome::Chromosome(const std::string newSequences,  int myMaxMutations ,  BiopolymerClassContainer myBiopolymerClassContainer ) {
		//std::cout<<__FILE__<<":"<<__LINE__<<" This is an obsolete method!"<<std::endl; exit(1);
		std::cout<<__FILE__<<":"<<__LINE__<<" newSequences = "<<newSequences<<std::endl; 
                initialize();
		biopolymerClassContainer = myBiopolymerClassContainer;
		biopolymerClassContainer.setOriginalSequencesFromCurrentSequences();
		setMaxMutations(myMaxMutations);
		setSequences(newSequences);
		biopolymerClassContainer.loadMutationVectorsFromSequence();
		//loadMutations();
		validate();
	}
	Chromosome::Chromosome( std::string mutationString, BiopolymerClassContainer myBiopolymerClassContainer ) {
		//std::cout<<__FILE__<<":"<<__LINE__<<" This is an obsolete method!"<<std::endl; exit(1);
		std::cout<<__FILE__<<":"<<__LINE__<<" mutationString = >"<<mutationString<<"<"<<std::endl; 
		if (mutationString.length() == 0) {
			std::cout<<__FILE__<<":"<<__LINE__<<" The mutation string is of length 0!"<<std::endl; exit(1);
		}
                initialize();
		biopolymerClassContainer = myBiopolymerClassContainer;
		biopolymerClassContainer.setOriginalSequencesFromCurrentSequences();
		setMutationVectorFromString(mutationString);
		vector<Mutation> myCompositeMutationVector = biopolymerClassContainer.getCompositeMutationVector();
                if (myCompositeMutationVector.size() !=  biopolymerClassContainer.getNumMutationVectorElements()) {
                        //for (int i =0; i < myCompositeMutationVector.size(); i++) {std::cout<<__FILE__<<":"<<__LINE__<<" Putting out myCompositeMutationVector["<<i<<"].ResidueID : "<<  myCompositeMutationVector[i].ResidueID.outString()<<endl;}
                        //std::cout<<__FILE__<<":"<<__LINE__<<"biopolymerClassContainer has formatted mutation string :"<<getFormattedMutationsString()<<std::endl;
			std::cout<<__FILE__<<":"<<__LINE__<<" Unexplained error!"<<std::endl; exit(1);
		}
		for (int i = 0 ; i < myCompositeMutationVector.size() ; i++) {
			// 						Mutation myMutation, bool safeParameters, bool matchPurineN1AtomLocations, bool proteinCapping
			biopolymerClassContainer.substituteResidue(myCompositeMutationVector[i],bool(true), bool(false), bool(false) );
		}
 		std::cout<<__FILE__<<":"<<__LINE__<<" Checking number of mutations: "<<numMutations()     <<std::endl;
		validate();
	}
	Chromosome::Chromosome(int myMaxMutations ,  BiopolymerClassContainer  myBiopolymerClassContainer ) {
		//std::cout<<__FILE__<<":"<<__LINE__<<" This is an obsolete method!"<<std::endl; exit(1);
                initialize();
		biopolymerClassContainer = myBiopolymerClassContainer;
		myBiopolymerClassContainer. setOriginalSequencesFromCurrentSequences();
		setMaxMutations(myMaxMutations);
		myBiopolymerClassContainer. loadMutationVectorsFromSequence();
		if (myBiopolymerClassContainer. getNumMutationVectorElements() > 0 ) {
			std::cout<<__FILE__<<":"<<__LINE__<<" Something went wrong! shouldn't have any mutations yet.."<<std::endl; exit(1);
		}
		//loadMutations();
		validate();
	}
	void Chromosome::submitJob(){
		
	}
	int Chromosome::countMutations (std::string string1 , std::string string2){return -1111;};


	std::string Chromosome::getSingleMutantID(){
		std::string mutationString =  biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR);
		if (mutationString.length() == 0) {
			std::cout<<__FILE__<<":"<<__LINE__<<" Something is wrong! the mutationString has zero length!"<<std::endl; exit(1); 
		}
		return mutationString;
	}
   
	std::string Chromosome::getMutationString() {
	        return biopolymerClassContainer.getFormattedMutationsString(MUTATIONMINORSEPARATOR);	
	};
	std::string Chromosome::getPdbNumberedMutationString(DBManager & mysqlConnection) {
            std:string sequencesString = "";     
            for (int i = 0 ; i < biopolymerClassContainer.getNumMutationVectorElements() ; i++) {
                sequencesString += biopolymerClassContainer.getMutationVector()[i].getChain();
                sequencesString += MUTATIONMINORSEPARATOR; // separates chain ID from residueID
                sequencesString +=  mysqlConnection.getPdbResidueIDFromRenumberedResidueNumber( biopolymerClassContainer.getMutationVector()[i].getChain(), biopolymerClassContainer.getMutationVector()[i].getResidue().getResidueNumber()); 
                sequencesString += MUTATIONMINORSEPARATOR; // separates residueID from mutation
                sequencesString += biopolymerClassContainer.updMutationVector()[i].getSubstitutedResidueType();
                if (i < (biopolymerClassContainer.getNumMutationVectorElements() -1)) {
                    sequencesString += MUTATIONMAJORSEPARATOR ; // connect mutants // was "."
                }
            }    
            return sequencesString;

	};
        /*
	std::string Chromosome::getResidueIDAsString(int residueIndex) {
		validateFirstResidue();
		int tempResidue = residueIndex + firstResidue;
		//char * residueStr;
		//std::string(itoa(tempResidue,residueStr ,10));
		std::string residueString = boost::lexical_cast<std::string>(tempResidue);//string(residueStr);
		return residueString;
	}*/

        std::string Chromosome::getChainID1 () {
		std::cout<<__FILE__<<":"<<__LINE__<<" This is an obsolete method!"<<std::endl; exit(1);
                if (chainID1.length() ==0) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" The current sequence is of length 0!"<<std::endl; exit(1);}
                return chainID1 ;
        }
	

	void Chromosome::prepareSingleMutant( DBManager & mysqlConnection , std::string singleMutantDirectory, bool computeIndividualWildType) { //, std::string jobName, std::string workingDirectory, std::string singleMutantDirectory  ) {
		
                std::string workingDirectory = mysqlConnection.getWorkingDirectory()  ;
                
                //std::string singleMutantDirectory = workingDirectory + "/" + chromosome.getSingleMutantID() + "/"  ;

                std::cout<<__FILE__<<":"<<__LINE__<<" mutationString = "<<getMutationString()<<std::endl;
                        
                struct stat st;
                if(stat(workingDirectory.c_str() ,&st) == 0) {
                    if(st.st_mode & S_IFDIR != 0) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Checking directory "<<workingDirectory<<" ... success!"<<std::endl;
                    }else {
                        
                        std::cout<<__FILE__<<":"<<__LINE__<<" Unable to find directory "<<workingDirectory<<" .. an unknown error ocurred"<<std::endl; exit(1);
                    }
                } else {
                        /*
			std::string makeWorkingDirectoryCommand = (std::string("mkdir ") + workingDirectory );
			std::cout<<__FILE__<<":"<<__LINE__<<" About to issue mkdir command : "<<makeWorkingDirectoryCommand<<std::endl;
			system(makeWorkingDirectoryCommand.c_str());
                        std::cout<<__FILE__<<":"<<__LINE__<<" Just created directory "<<workingDirectory<<" .. presumably for first time use"<<std::endl; 
                        */
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
                        myMkdir(workingDirectory);
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
                }

                /*std::string makeDirectoryCommand = (std::string("mkdir ") + singleMutantDirectory);
                std::cout<<__FILE__<<":"<<__LINE__<<" About to issue mkdir command : "<<makeDirectoryCommand<<std::endl;
                system(makeDirectoryCommand.c_str()); */
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
                myMkdir(singleMutantDirectory);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
                //exit(1);
		std::string    runCommand = "";
		std::string singleMutantFiles = mysqlConnection.getSingleMutantFilesDirectory(); //"/bubo/home/h17/samuelf/svn/breeder/singleMutantFiles";
                std::cout<<__FILE__<<":"<<__LINE__<<" retrieved singleMutantFiles = "<<singleMutantFiles<<std::endl;
		std::ifstream testFileStream(mysqlConnection.getRenumberedWildTypeStructureFileName().c_str());
		if (!(testFileStream.good())) {std::cout<<__FILE__<<":"<<__LINE__<<" Could not open "<<mysqlConnection.getRenumberedWildTypeStructureFileName()<<std::endl; exit(1); }
		testFileStream.close();

                //mmbCopyFile(mysqlConnection.getRenumberedWildTypeStructureFileName(),string(singleMutantDirectory + "/raw.pdb"));
                copyFile(mysqlConnection.getRenumberedWildTypeStructureFileName(),string(singleMutantDirectory + "/raw.pdb"));
               

                if (!computeIndividualWildType) {
                        std::string startJobString = "";
                        if (mysqlConnection.getNumSequenceTableEntries()) { // Only try to populate the pdbNumberedMutationString field if there are entries in the sequence table. Later, we will have to figure out why there are no such entries for the children.               
			    startJobString = std::string("insert into results (jobName, pdbId, batch_directory, sequence,mutationString,pdbNumberedMutationString, status,numMutations ) VALUES (\"")    +  mysqlConnection.getJobName()  + "\", \"" +   mysqlConnection.getPdbId()  +  "\",   \""    +  workingDirectory  + std::string("\", \"")  + getSequences() + std::string("\" , \"") + getMutationString() + std::string("\", \"") + getPdbNumberedMutationString(mysqlConnection) +std::string("\" , \"submitted\", ") + std::to_string(numMutations()) + std::string(");");
                        } else {
			    startJobString = std::string("insert into results (jobName, pdbId, batch_directory, sequence,mutationString, status,numMutations ) VALUES (\"")    +  mysqlConnection.getJobName()  + "\", \"" +   mysqlConnection.getPdbId()  +  "\",   \""    +  workingDirectory  + std::string("\", \"")  + getSequences() + std::string("\" , \"") + getMutationString() + std::string("\", \"submitted\", ") + std::to_string(numMutations()) + std::string(");");
                        }
			std::cout<<__FILE__<<":"<<__LINE__<<" About to issue : "<<startJobString<<std::endl;
                	mysqlConnection.insertUpdateDelete(startJobString);
                        mysqlConnection.setUserProvidedMutationStringInDB(getMutationString()); // sets userProvidedMutationString field in mutationStringEquivalent table
                }
                std::string parametersString  = singleMutantFiles + "/parameters.csv";
                std::string rotabaseString    = singleMutantFiles + "/rotabase.txt"   ;
                std::ifstream testParametersFileStream(parametersString.c_str());
                if (!(testParametersFileStream.good())) {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Could not open "<<parametersString<<" . "<<std::endl;
                    exit(1);
                } else {
                    std::cout<<__FILE__<<":"<<__LINE__<<" It is possible to open "<<parametersString<<" for reading. "<<std::endl;
                }  
                testParametersFileStream.close();
                /*if(stat(parametersString.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<parametersString<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<parametersString<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
                stat(parametersString.c_str() ,&st); 
                std::cout<<__FILE__<<":"<<__LINE__<<" st.st_mode = >"<<st.st_mode<<"< . "<<std::endl;
                if (st.st_mode & S_IRUSR) {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Confirmed that the OWNER (not necessarily you) has read permission for >"<<parametersString<<"< ."<<std::endl;
                } else {
                    std::cout<<__FILE__<<":"<<__LINE__<<" The OWNER (not necessarily you) has read permission for >"<<parametersString<<"< ."<<std::endl;
                    //exit(1);
                }*/
                std::ifstream testRotabaseFileStream(rotabaseString.c_str());
                if (!(testRotabaseFileStream.good())) {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Could not open "<<rotabaseString<<" . "<<std::endl;
                    exit(1);
                } else {
                    std::cout<<__FILE__<<":"<<__LINE__<<" It is possible to open "<<rotabaseString<<" for reading. "<<std::endl;
                }  
                testRotabaseFileStream.close();       
                //if(stat(rotabaseString.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<rotabaseString<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<rotabaseString<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}

	        runCommand += "cp " + parametersString  + " "  +  singleMutantDirectory + " ; ";
                //copyFile(parametersString ,string(singleMutantDirectory + "/"));

		runCommand += "cp " + rotabaseString    + " "  +  singleMutantDirectory + " ; ";
                std::cout<<__FILE__<<":"<<__LINE__<<endl;

                /*if (mysqlConnection.gromacsShouldRun()) {
			std::string ions_mdp_String   = singleMutantFiles + "/ions.mdp"   ;
			std::string md_mdp_String     = singleMutantFiles + "/md.mdp"   ;
			std::string mdout_mdp_String  = singleMutantFiles + "/mdout.mdp"   ;
			std::string minim_mdp_String  = singleMutantFiles + "/minim.mdp"   ;
			std::string npt_mdp_String    = singleMutantFiles + "/npt.mdp"   ;
			std::string nvt_mdp_String    = singleMutantFiles + "/nvt.mdp"   ;
                        std::cout<<__FILE__<<":"<<__LINE__<<endl;
			if(stat(ions_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<ions_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<ions_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			if(stat(md_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<md_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<md_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			if(stat(mdout_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<mdout_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<mdout_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			if(stat(minim_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<minim_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<minim_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			if(stat(npt_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<npt_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<npt_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			if(stat(nvt_mdp_String.c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<nvt_mdp_String<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<nvt_mdp_String<<". Check your -SINGLEMUTANTFILESDIRECTORY parameter."<<std::endl; exit(1);}
			runCommand += "cp " + ions_mdp_String   + " "  +  singleMutantDirectory + " ; ";
			runCommand += "cp " + md_mdp_String     + " "  +  singleMutantDirectory + " ; ";
			runCommand += "cp " + mdout_mdp_String  + " "  +  singleMutantDirectory + " ; ";
			runCommand += "cp " + minim_mdp_String  + " "  +  singleMutantDirectory + " ; ";
			runCommand += "cp " + npt_mdp_String    + " "  +  singleMutantDirectory + " ; ";
			runCommand += "cp " + nvt_mdp_String    + " "  +  singleMutantDirectory + " ; ";
                        std::cout<<__FILE__<<":"<<__LINE__<<endl;
                }*/ // This is now in Breed.cpp
                std::string myLastStage = std::to_string(mysqlConnection.getLastStage());
		std::string lastStructureFileName = std::string("last.")+myLastStage+std::string(".pdb");
		runCommand += std::string("echo ") + lastStructureFileName + (" > ") + singleMutantDirectory + std::string("/batch.txt ; ");  // writes "last.x.pdb where x = lastStage, to batch.txt.
		std::cout<<__FILE__<<":"<<__LINE__<<" About to issue: "<<runCommand<<std::endl;
		if (! (system(runCommand.c_str()))) {std::cout<<__FILE__<<":"<<__LINE__<<" Command OK."<<std::endl;}
		else {std::cout<<__FILE__<<":"<<__LINE__<<" Command returned nonzero error code! Letting this go since RPi seems to take warnings as   errors."<<std::endl; }// exit(1); 
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		std::string commandFileName(singleMutantDirectory+"/commands.dat");
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		std::ofstream commandFile(commandFileName.c_str());
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                commandFile << fixed; 
                commandFile.precision (14);
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                int myFlexibilityWindowOffset = mysqlConnection.getFlexibilityWindowOffset(); 
                //std::string myQueryString = std::string(" select flexibilityWindowOffset from jobInfo where jobInfo.runID = \"" +  mysqlConnection.getJobID()+ "\" ;"); 
                //std::cout<<__FILE__<<":"<<__LINE__<<" about to issue "<<myQueryString<<std::endl;
                //int myFlexibilityWindowOffset = mysqlConnection.queryValueAsInt(myQueryString);
		writeCommandFile(commandFile,myFlexibilityWindowOffset, mysqlConnection);
		commandFile.close();
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
	}

	void Chromosome::writeCommonPart(std::ofstream & output, DBManager & mysqlConnection) {
		
		output <<"firstStage 2 "<<std::endl; 
		output <<"lastStage "<<mysqlConnection.getLastStage()<<std::endl; 
		output <<"loadSequencesFromPdb  "<<std::endl; 
		output <<"smallGroupInertiaMultiplier 11  "<<std::endl; 
		output <<"matchPurineN1AtomLocations 0    "<<std::endl; 
		//output <<"#reportingInterval .000000000000000000000001"<<std::endl; 
                std::string myquery = "select integratorAccuracy from jobInfo where runID = \"" + mysqlConnection.getJobID ( ) + "\";";  
                double myintegratorAccuracy = mysqlConnection.queryValueAsDouble (myquery); 
                output <<"integratorAccuracy "<< myintegratorAccuracy <<std::endl;
                std::string myquery2 = "select prolineCorrection from jobInfo where runID = \"" + mysqlConnection.getJobID ( ) + "\";";
                std::string myprolineCorrection = mysqlConnection.queryValueAsString (myquery2);
                output << myprolineCorrection <<std::endl;
                std::string myquery3 = "select ssbondCorrection from jobInfo where runID = \"" + mysqlConnection.getJobID ( ) + "\";";
                output <<"noseHooverTime 1"<<std::endl;
                output <<"reportingInterval ";
                output <<mysqlConnection.getReportingInterval();
		output <<std::endl; 
		output <<"numReportingIntervals "; //150 "<<std::endl; 
 		output << mysqlConnection.getNumReportingIntervals();
		output <<std::endl; 
		//output <<"#numReportingIntervals 1 "<<std::endl; 
		//output <<"readAtStage 2"<<std::endl; 
		output <<"globalBondTorsionScaleFactor 1"<<std::endl; 
		output <<"globalBondBendScaleFactor 1"<<std::endl; 
		output <<"globalCoulombScaleFactor 1"<<std::endl; 
		output <<"globalVdwScaleFactor 1"<<std::endl; 
		output <<"globalAmberImproperTorsionScaleFactor 1"<<std::endl; 
		//output <<"readBlockEnd"<<std::endl; 
		std::string myquery4 = "select randomizeInitialVelocities from jobInfo where runID = \"" + mysqlConnection.getJobID ( ) + "\";";
		std::string myRandomizeInitialVelocities = mysqlConnection.queryValueAsString (myquery4);
		if (myRandomizeInitialVelocities == "1"){
		output <<"randomizeInitialVelocities " << myRandomizeInitialVelocities  <<  std::endl; 
		}
		output <<"mobilizer Rigid"<<std::endl; 
		output <<"constrainChainRigidSegments"<<std::endl; 
		std::string myquery5 = "select temperature from jobInfo where runID = \"" + mysqlConnection.getJobID ( ) + "\";";
		std::string myTemperature = mysqlConnection.queryValueAsString (myquery5);
		if (myTemperature != "NULL"){
		output <<"temperature " << myTemperature<<  std::endl; 
		}
                std::string myssbondCorrection = mysqlConnection.queryValueAsString (myquery3);
                output << myssbondCorrection <<std::endl;
                output <<"# "<<__FILE__<<":"<<__LINE__<<" End of writeCommonPart(std::ofstream & output, DBManager & mysqlConnection) "<<std::endl;
	};

	void Chromosome::writeChains(std::ofstream & output) {
		std::cout<<__FILE__<<":"<<__LINE__<<" This call is obsolete! this is controlled by the Chromosome's biopolymerClassContainer."<<std::endl; 
        }

	void Chromosome::writeMutationFlexibilizers(std::ofstream & output, const int offset, const double radius) {
                biopolymerClassContainer.writeMutationFlexibilizers(output,offset, radius);
	}

	void Chromosome::writeWaterDroplets(std::ofstream & output,  DBManager & mysqlConnection) {
            if (mysqlConnection.getWaterRadius() > 1E-14) {
                biopolymerClassContainer.writeWaterDroplets(output,mysqlConnection.getWaterSpringConstant() , mysqlConnection.getWaterRadius()  );
            } else {
		std::cout<<__FILE__<<":"<<__LINE__<<" Water droplet radius = "<< mysqlConnection.getWaterRadius()<< " so we are not generating any water droplets."<<std::endl; 
            }
	}

	void Chromosome::writePhysicsZones(std::ofstream & output, const int offset) {
                output<<"physicsRadius 1.2"<<std::endl;
	}

	void Chromosome::writeSubstituteResidueCommands(std::ofstream & output) {
		biopolymerClassContainer.writeSubstituteResidueCommands(output);
	}

	void Chromosome::writeCommandFile(std::ofstream & output, const int offset,DBManager & mysqlConnection) {
		writeCommonPart(output,mysqlConnection);
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

                //biopolymerClassContainer.initializeBiopolymers(
                
                std::string myQueryString = "select createDisulphideBridges from jobInfo where runID = \"" + mysqlConnection.getJobID() + "\" ; ";
                if (mysqlConnection.queryValueAsInt(myQueryString)){

			/*CompoundSystem          system;
			SimbodyMatterSubsystem matter(system);
			vector<Displacement> displacementVector;
			displacementVector.clear();*/
                       
			/*std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			BiopolymerClassContainer wildTypeBiopolymerClassContainerForCreatingDisulphideBridges;
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			wildTypeBiopolymerClassContainerForCreatingDisulphideBridges.initializeBiopolymers (system,false,true,false,false,false,false,false,0.0,displacementVector, .1500, .07);
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			wildTypeBiopolymerClassContainerForCreatingDisulphideBridges.initializeAtomInfoVectors(matter);
                        *//*
                        // middle way:
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			//BiopolymerClassContainer biopolymerClassContainer;
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;*/

			 // new way:
			/*
			biopolymerClassContainer.initializeBiopolymers (system,false,true,false,false,false,false,false,0.0,displacementVector, .1500, .07);
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			biopolymerClassContainer.initializeAtomInfoVectors(matter);

			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			biopolymerClassContainer.createDisulphideBridges(output);*/
			/* 
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			BiopolymerClassContainer wildTypeBiopolymerClassContainer;
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			wildTypeBiopolymerClassContainer.loadSequencesFromPdb (mysqlConnection.getWildTypeStructureFileName(),bool(false));
			CompoundSystem          system;
			SimbodyMatterSubsystem matter(system);
			DuMMForceFieldSubsystem dumm(system);
			vector<Displacement> displacementVector; 
			displacementVector.clear();
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			wildTypeBiopolymerClassContainer.initializeBiopolymers (system,false,true,false,false,false,false,false,0.0,displacementVector, .1500, .07);
			std::ofstream output(string("output.txt").c_str());;
			wildTypeBiopolymerClassContainer.initializeAtomInfoVectors(matter);
			wildTypeBiopolymerClassContainer.createDisulphideBridges(output);
			*/
			CompoundSystem          system;
			SimbodyMatterSubsystem matter(system);
			vector<Displacement> displacementVector; 
			displacementVector.clear();
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			biopolymerClassContainer.initializeBiopolymers (system,false,true,false,false,false,false,false,0.0,displacementVector, .1500, .07);
			//std::ofstream output2(string("output2.txt").c_str());;
			biopolymerClassContainer.initializeAtomInfoVectors(matter);
			biopolymerClassContainer.createDisulphideBridges(output);


                }
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		writeMutationFlexibilizers (output, offset, mysqlConnection.getMobilizerWithinRadius());
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		writePhysicsZones(output,offset);
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                // We don't need this line anymore, because this is taken care of by foldx BuildModel
		//writeSubstituteResidueCommands(output);
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                writeWaterDroplets(output, mysqlConnection);
  		output.close();
	}
	
	bool Chromosome::hasBeenComputed(DBManager & mysqlConnection) {
                if (mysqlConnection.getNumJobs("completed",getMutationString())){return 1; } else{return 0;}
		//string myQuery = "select count(*) from results where jobName =  \"" + mysqlConnection.getJobName() + "\" and mutationString = \"" + getMutationString() + "\" ;"; //and status = \"completed\" ;"; 
		//if ((mysqlConnection.queryValueAsString(myQuery)).compare("0") == 0) {return false;} else {return true;} // this needs to be more sophisticated in terms of validation!
	}

   bool Chromosome::checkIndividualComputation(DBManager & mysqlConnection) {
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                string  myQuery = "select computeIndividualWildType from jobInfo where runID = \"" + mysqlConnection.getJobName() + "\"; "; 
                if ((mysqlConnection.queryValueAsString(myQuery)).compare("1") == 0) {return true;} 
                //else {return false;} // this needs to be more sophisticated in terms of validation!
                 
	}
	bool Chromosome::allMutationsDifferFromWildType () {
		return biopolymerClassContainer.allMutationsDifferFromWildType ();
	}
	void Chromosome::revertToWildTypeEquivalent() {
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<endl; 
			biopolymerClassContainer.setCurrentSequencesFromOriginalSequences();
			biopolymerClassContainer. updateMutationResidueTypesFromCurrentSequence();        
	}



        void Chromosome::populateSequenceTable (DBManager & mysqlConnection) {
                cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                // If this doesn't work, can also loop over i like this: myBiopolymerClassContainer.updBiopolymerClass(i)
                
                std::string firstUpdateOrInsertSequenceRowReturnValue, updateOrInsertSequenceRowReturnValue = "";
                map <const String, BiopolymerClass> myBiopolymerClassMap = biopolymerClassContainer.getBiopolymerClassMap();
                for (auto  biopolymerClassMapIterator = myBiopolymerClassMap.begin() ; biopolymerClassMapIterator != myBiopolymerClassMap.end(); biopolymerClassMapIterator++) { 
                    BiopolymerClass myBiopolymerClass = biopolymerClassContainer.updBiopolymerClass(biopolymerClassMapIterator->first) ;
                    //myBiopolymerClass.validateSequence();
                    string myChainId = myBiopolymerClass.getChainID();
                    cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Now processing chain "<<myChainId<<std::endl;
                    vector<ResidueID>  myResidueIdVector = myBiopolymerClass.getResidueIdVector();

                    for (auto myResidueIdVectorIterator = myResidueIdVector.begin() ; myResidueIdVectorIterator != myResidueIdVector.end() ; myResidueIdVectorIterator++) {
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                        ResidueID myResidueId = *myResidueIdVectorIterator;
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                        string myAminoAcidTypeInSingleLetterCode = myBiopolymerClass.getResidueSingleLetterCode(myResidueId);  
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                        int myRenumberedResidueNumber = myBiopolymerClass.getResidueIndex(myResidueId) + 1;
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                        //updateOrInsertSequenceRowReturnValue = 
                        mysqlConnection.updateOrInsertSequenceRow (myChainId, myRenumberedResidueNumber, myResidueId.getResidueNumber(),myResidueId.getInsertionCode(), myAminoAcidTypeInSingleLetterCode);
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                        if (updateOrInsertSequenceRowReturnValue == "UPDATE") {
                                // Explanation: we expect either all rows to be inserted, or all rows to be unchanged. If any rows are UPDATEd, that means some sequence difference has appeared with respect to the last time this function was called on this protein. This is suspicious and should be checked before going further.  
                            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " updateOrInsertSequenceRowReturnValue returned " << updateOrInsertSequenceRowReturnValue << ". This should not happen! " << std::endl; 
                            exit(1);
                        } else if (myResidueIdVectorIterator == myResidueIdVector.begin() ) {
                            firstUpdateOrInsertSequenceRowReturnValue = updateOrInsertSequenceRowReturnValue;
                        } else if (myResidueIdVectorIterator != myResidueIdVector.begin() ) {
                            if ( firstUpdateOrInsertSequenceRowReturnValue != updateOrInsertSequenceRowReturnValue) {
                                // Explanation: we expect either all rows to be inserted, or all rows to be unchanged. If some rows are inserted while others are unchanged, that means this program was run twice with different results. This is suspicious and should be checked before going further.  
                                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " updateOrInsertSequenceRowReturnValue returned " << updateOrInsertSequenceRowReturnValue << ".  This differs from the first residue's value of "<< firstUpdateOrInsertSequenceRowReturnValue <<". This can happen e.g. when two jobs are running at once, for the same protein. Still, keep an euye on this. "<< std::endl;// This should not happen! " << std::endl; 
                                //std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"Explanation: we expect either all rows to be inserted, or all rows to be unchanged. If some rows are inserted while others are unchanged, that means this program was run twice with different results. This is suspicious and should be checked before going further. "<<std::endl;
                                //exit(1);
                            }
                        }
			/*
		       	*/
                        cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                    } 
                    cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
                }
                cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        }


