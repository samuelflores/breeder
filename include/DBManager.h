// SQLite.h            

#ifdef ENABLE_SQLITE  
#include <sqlite3.h>
#endif

#include <iostream>
#include <sstream>
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
		std::string pdbId;
                #ifdef ENABLE_SQLITE  
                sqlite3 *db; 
                #endif
                std::string server ;
                std::string user ;
                std::string password ; /* set me first */
                std::string database ;
                std::string account;
                //std::string jobLibraryPath;
                std::string MMBExecutable;
                std::string FoldXScript;
                std::string FoldXExecutable;
                std::string SQLExecutable;
                std::string singleMutantFilesDirectory;
                std::string breederMainDirectory      ;
                std::string workingDirectory      ;
                std::string wildTypeStructureFileName;
                std::string msiScannedChains; // When one specifies an interface and depth for multiscanning, this string contains the chains to be mutated.
                std::string msiPartnerChains; // .. and this one the chains msiScannedChains are interacting with
                std::string chainsInMutatedSubunit;
                std::string userProvidedMutationString;
                double      msiRadius;        // .. and the depth of the interface
                double      mobilizerWithinRadius;
                double      waterRadius;
                double      maxTime;
                double      gromacsSimulationTime;
                double      temperature;
                double      experimentalDDG;
                bool        sequenceOnly;

	public:
                std::string emailAddress;
                int    checkFileStatus(std::string myFileName);
                std::string   getPdbResidueIDFromRenumberedResidueNumber(std::string myChainId, int renumberedResidueNumber);
                int           getNumSequenceTableEntries();
                void          insertSequenceRow (std::string myChainId, int myRenumberedResidueNumber, int myPdbResidueNumber,char myInsertionCode, std::string myAminoAcidType);
                void          updateOrInsertSequenceRow (std::string myChainId, int myRenumberedResidueNumber, int myPdbResidueNumber,char myInsertionCode, std::string myAminoAcidType);
                //void   insertAlternateStructureFileNameIfAbsent (std::string alternateStructureFileName);
                void   insertIntoAlternatePdbStructuresIfAbsent (std::string columnName, std::string columnValue);
                void   updateOrInsertInAlternatePdbStructures (std::string columName, std::string columnValue, std::string myStatus);
                //void   updateOrInsertAlternateStructureFileName (std::string alternateStructureFileName, std::string myStatus);
                void   updateOrInsertAlternatePdbNumberedStructureFileName (std::string alternatePdbNumberedStructureFileName, std::string myStatus);
                void   setUserProvidedMutationString      (const std::string  myUserProvidedMutationString      ){userProvidedMutationString = myUserProvidedMutationString;};
                //void   setEmailAddress(std::string myEmailAddress){emailAddress = myEmailAddress;};
                //std::string getEmailAddress(){return emailAddress;};
                void   setSequenceOnly(const bool mySequenceOnly) {sequenceOnly = mySequenceOnly;}
                bool   getSequenceOnly() {
		    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Returning "<<sequenceOnly<<std::endl;  
		    return sequenceOnly ;}
                std::string getUserProvidedMutationString(){return userProvidedMutationString;}
                void   setUserProvidedMutationStringInDB(std::string breederFormattedMutationString );   
		void   setTemperatureInK(double myTemperature);
		double getTemperatureInK();
                double dgInkcalmol (double affinityInM);
		void   setExperimentalDDG(const double mutantAffinityInM, const double wildTypeAffinityInM, const double temperature);
		double getExperimentalDDG();
		void   setPdbAndChainsSkempi(const std::string myPdbChains);

                int countEntriesInSubmittedHomologs (std::string pdbPrimary, std::string pdbHomolog ,  std::string complexHomolog, std::string mutationStringHomolog);
                void setSubmittedHomologs    (std::string pdbPrimary, std::string pdbHomolog , std::string complexPrimary, std::string complexHomolog, std::string mutationStringPrimary,  std::string mutationStringHomolog   ); 
                // Accessors for local sequence identity
                int countEntriesInLocalSequenceIdentity (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog , std::string mutationString,double inclusionRadius );
                
                void setLocalSequenceIdentity (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog , std::string mutationString,double inclusionRadius, std::string columnName , double columnValue  );
                int  countEntriesInMatchingChains (std::string pdbPrimary, std::string pdbHomolog , std::string  chainPrimary,  std::string chainHomolog ) ;
                void setMatchingChainsValue (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName , double columnValue  );
                bool   matchingChainsValueIsNull      (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName   ) ;
                double getMatchingChainsValueAsDouble    (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName   );
                bool   chainSequenceAlignmentIsSatisfactory(std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog, double sequenceIdentityCutoff );
                // this sets the -p flag required on Uppmax
                bool hasEntriesInAlternatePdbStructures();     
		//std::string getDefaultStructureFileName();
		//std::string getAlternateStructureFileName(std::string myPdbId);
		//std::string getAlternateStructureFileName();
		std::string getAlternateRenumberedStructureFile();
		std::string getAlternatePdbNumberedStructureFile();
                std::string getFromAlternatePdbStructures(std::string columnName, std::string myPdbId);
                int  countRmsdEntriesDirectly(std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary,std::string complexChainHomolog);
                int  countRmsdEntriesSwitched(std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary,std::string complexChainHomolog);
                int  countRmsdEntriesBothWays(std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary,std::string complexChainHomolog);
                double getRmsd (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog   );
                void setRmsd (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog , double myRmsd );
		bool getPdbStatusFail();
		void setPdbStatusFail();
		void setPdbStatusSuccess();
		std::string getPartition ();
		// this is a function which issues a mysql query "queryString" and returns an answer of type myType
		//template <class myType>
		void setJobID(std::string myJobID);
		void setPdbId(std::string myPdbId);
		void setJobLibraryPath(std::string myJobLibraryPath); // This has a default parameter of "" as set in the function definition.  Remember, as things are now, this method is not being called ever.  User sets parameter in mysql directly.
                void setWorkingDirectory(const std::string myWorkingDirectory);
                //void setWildTypeStructureFileName(const std::string wtStructureFileName); // Set the path and file name of the structure file. Validates to make sure owner has read access.
                void setPartition       (const std::string myPartition       );
                void setChainsInMutatedSubunit       (const std::string  myChainsInMutatedSubunit       );
                void setComplexStringInResultsTable( const std::string myMMBFormattedMutationString);
                void setNumReportingIntervals        (const int    );
                void setReportingInterval            (const double );
                void setFlexibilityWindowOffset(const int myFlexibilityWindowOffset);
                int getFlexibilityWindowOffset();
                void setLastStage                    (const int    );
                std::string getJobLibraryPath();
		std::string getJobID();
		std::string getPdbId();
                //{return getJobID();}; // For now, assuming PDB ID is same as jobID.  Should make this more formal later, by creating a new column perhaps in jobInfo.
		//void setPassword(std::string myPassword)  {password = myPassword; };
		void setAccountName(std::string myAccount )  {account = myAccount; };
		//std::string getPassword() {return password;};
		std::string getAccountName();
		void setMobilizerWithinRadius(double myRadius)  {mobilizerWithinRadius = myRadius ; };
		void setWaterRadius(double myRadius)  {
                    /*std::string myQuery = "update jobInfo set waterRadius = ";
                    std::ostringstream ss; 
                    ss<<myRadius;
                    myQuery += std::string(ss.str());
                    myQuery += " where runID = " + getJobID()+";" ; 
                    insertUpdateDelete(myQuery); */
                    waterRadius = myRadius;
                    };
		double getWaterRadius()  {
                    //std::string myQuery;
                    //myQuery = std::string("select  waterRadius from jobInfo where  runID = \""); 
                    //myQuery += getJobID() + std::string("\" ;") ; 
                    //return queryValueAsDouble(myQuery); 
                    return waterRadius;
                    };
		double getWaterSpringConstant()  {return 300.;}; //std::string myQuery = "select  waterRadius from jobInfo where  runID = " + getJobID() + ";" ; queryValueAsString(myQuery); };
		double getMobilizerWithinRadius() {return mobilizerWithinRadius;}
                void setMsiScannedChains (std::string myMsiScannedChains) {
                    if (myMsiScannedChains.length() < 1) {std::cout <<__FILE__<<":"<<__LINE__<< "Error <chains to be mutated> must be a string of length >0. You have supplied : >" << myMsiScannedChains<<"<"<<std::endl; exit(1);}
                    msiScannedChains = myMsiScannedChains;}
                 
                void setMsiPartnerChains (std::string myMsiPartnerChains) {
                    if (myMsiPartnerChains.length() < 1) {std::cout <<__FILE__<<":"<<__LINE__<<   "Error <partner chains> must be a string of length >0. You have supplied : >" <<myMsiPartnerChains<<"<"<<std::endl; exit(1);}
                    msiPartnerChains = myMsiPartnerChains;}
                std::string getMsiScannedChains() {return msiScannedChains;}
                std::string getMsiPartnerChains() {return msiPartnerChains;}
                void setMsiRadius (double myRadius) {
                    if (myRadius < 1.0e-14) {std::cout <<__FILE__<<":"<<__LINE__<< "Error <radius> must be > 0 nm. You have requested : "<<myRadius <<" ."<<std::endl; exit(1);}
                    msiRadius = myRadius;}
                double getMsiRadius (){ return msiRadius;}
		void setMaxTime(double myRadius)  {maxTime = myRadius ; };
		void setGromacsSimulationTime(double myGromacsSimulationTime)  {
                    if (myGromacsSimulationTime < -1E-14) {
			    std::cout <<__FILE__<<":"<<__LINE__<<   "Error! Simulation time of "<<myGromacsSimulationTime<<" is negative."<<std::endl; exit(1);}
			    //#std::cout <<__FILE__<<":"<<__LINE__<<   "Error! Simulation time of "<<myGromacsSimulationTime<<" is too small or negative."<<std::endl; exit(1);}
		    gromacsSimulationTime = myGromacsSimulationTime ; 
	            std::cout<<__FILE__<<":"<< __LINE__ <<" checking: getGromacsSimulationTime()  = "<<getGromacsSimulationTime() <<std::endl;
                };
		double getGromacsSimulationTime()  {return gromacsSimulationTime  ; };
		double getMaxTime() {return maxTime;}
		std::string getDatabase() {
                    return database;} ;
		void setMMBExecutable( std::string myMMBExecutable) { MMBExecutable = myMMBExecutable;} ;
		std::string getMMBExecutable() {return MMBExecutable;} ;
		void setFoldXScript  (std::string myFoldXScript) { FoldXScript = myFoldXScript ;} ;
		std::string getFoldXScript  () {return FoldXScript  ;} ;
		void setSQLExecutable  (std::string mySQLExecutable) { SQLExecutable = mySQLExecutable ;} ;
		std::string getSQLExecutable  () { return SQLExecutable ;} ;
		void setFoldXExecutable  (std::string myFoldXExecutable) { FoldXExecutable = myFoldXExecutable ;} ;
		std::string getFoldXExecutable  () {return FoldXExecutable  ;} ;
		//std::string getWildTypeStructureFileName ();

		std::string getDefaultPdbNumberedStructureFileName ();
		std::string getPdbNumberedWildTypeStructureFileName ();
		std::string getRenumberedWildTypeStructureFileName ();
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
                int getNumCores (){
			std::string myQueryString = std::string("select numCores  from jobInfo where runID = \"") + getJobID() + std::string("\" ;");
                        return queryValueAsInt(myQueryString);};
		virtual void initialize (std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		//void close() { mysql_close(conn );}
		virtual void close();
		void validate();
                void setDefaultValues();
		DBManager(   ); // Default constructor more or less required for inheritance
		DBManager(std::string myServer ,      std::string myPassword , std::string myUser , std::string myDatabase  );
		std::string getMysqlCommandString();
		void insertUpdateDelete(std::string queryString );
		void executeStatement ( std::string queryString); //, bool resultIsExpected = true);
		double queryValueAsDouble (std::string queryString, bool returnValueIsNull = 1);
		int    queryValueAsInt    (std::string queryString);
		virtual std::string queryValueAsString (std::string queryString);
                bool acceptanceTest (double energy);
		int getNumJobs(std::string status,std::string mutationString); //   counts  all jobs with current jobName, pdbId, and given status and mutationString from results table
		int getNumJobs(std::string mutationString);  // count     all jobs with current jobName, pdbId, and given mutationString from results table.
                std::string getChainsInMutatedSubunit();//{return chainsInMutatedSubunit;} //std::string queryString  =  "select chainsInMutatedSubunit from jobInfo where runID   = \"" + getJobID() + "\" limit 1; ";return queryValueAsString( queryString);};
		std::string retrieveSequenceAtRandom();
		std::string retrieveSequenceInOrder(int & index, std::string orderField , std::string order);
                bool gromacsShouldRun(){
	            std::cout<<__FILE__<<":"<< __LINE__ <<" getGromacsSimulationTime()  = "<<getGromacsSimulationTime() <<std::endl;
                    return (getGromacsSimulationTime() > 1E-14); // May need a more sophisticated criterion .. let's see how things go. 
                };
                std::string jobIDAndPdbIdSelectionString(){
                    return " jobName = \"" + getJobID() + "\" and pdbId = \'" + getPdbId()  + "\' ";
                }
                std::string getFoldxWildTypeEnergyBaseQuery(std::string wildTypeMutationString);
                double getFoldxWildTypeEnergy( std::string wildTypeMutationString); 
                bool shouldRunZEMuForJobWildType(std::string mutationString, std::string wildTypeMutationString);
                    /*
                    { // this tells us whether there is a need to do a ZEMu wild type run for this mutant.
                    std::string foldx_energy_wild_type_for_this_mutant_query =  "select   foldx_energy_wild_type from results where         mutationString = \"" +         mutationString + "\" and jobName = \"" + getJobID() + "\" and pdbId = \'" + getPdbId()  + "\' and not ISNULL(foldx_energy) limit 1; "; 
                    std::string foldx_energy_wild_type_for_this_mutant_string = queryValueAsString(foldx_energy_wild_type_for_this_mutant_query);
                    std::string  computeIndividualWildTypeRequestedQuery = "select computeIndividualWildType from jobInfo where runID = \"" + getJobName() + "\"; ";
                    std::string foldx_energy_wild_type_query =  "select   foldx_energy_wild_type from results where wildTypeString = \"" + wildTypeMutationString + "\" and jobName = \"" + getJobID() + "\"  and not ISNULL(foldx_energy) limit 1; "; 
                    std::string foldx_energy_wild_type_string = queryValueAsString(foldx_energy_wild_type_query);
                    std::cout<<__FILE__<<":"<< __LINE__ <<std::endl;
                    // WT computed for this mutant, individual wild type requested, wild type exists elsewhere
                    //  1 X X 
                    if ((foldx_energy_wild_type_for_this_mutant_string.compare("NO-RESULT") != 0) && (foldx_energy_wild_type_for_this_mutant_string.compare("") != 0) ) { 
                        std::cout<<__FILE__<<":"<< __LINE__ <<" (foldx_energy_wild_type_for_this_mutant_string.compare(NO-RESULT) != 0) .. wild type energy has been computed, hence no need to recompute wild type ."<<std::endl;
                        return false;}  // If wild type energy has been computed, there is no need to recompute, ever.
                    //  0 0 0  
                    //  0 1 0  
                    else if (((foldx_energy_wild_type_for_this_mutant_string.compare("NO-RESULT") == 0) 	| (foldx_energy_wild_type_for_this_mutant_string.compare("") == 0) )
                        && ((foldx_energy_wild_type_string.compare("NO-RESULT") == 0) || (foldx_energy_wild_type_string.compare("") == 0 )) ) {
                        std::cout<<__FILE__<<":"<< __LINE__ <<"  ((foldx_energy_wild_type_for_this_mutant_string.compare(NO-RESULT) == 0) && (foldx_energy_wild_type_string.compare(NO-RESULT) == 0)) .. No wild type  energy has been computed, for this mutant or for any with the same wild type . so definitely compute wild type energy.  "<<std::endl;
                        return true;}
                    //  0 0 1  
                    else if  ((queryValueAsString(computeIndividualWildTypeRequestedQuery)).compare("1") != 0) 
                    {
                        // foldx_energy_wild_type has been computed SOMEWHERE. We should not recompute, then, unless this was a case of computeIndividualWildType TRUE.
                        std::cout<<__FILE__<<":"<< __LINE__ <<"   ((queryValueAsString(computeIndividualWildTypeRequestedQuery)).compare(1) != 0) ...  foldx_energy_wild_type has been computed SOMEWHERE. We should not recompute, then, unless this was a case of computeIndividualWildType TRUE  "<<std::endl;
                        std::string updateWildTypeEnergyQuery = std::string("update results set foldx_energy_wild_type = "  ) + foldx_energy_wild_type_string + " where  jobName = \"" + getJobID() + "\" and mutationString = \"" +         mutationString + "\";"  ;
                        queryValueAsString(updateWildTypeEnergyQuery); // The wild type energy is available elsewhere. We will go ahead and use that energy to update the job wildTypeMutationString wild type energy for future convenience.
 			return false; // There is no need to run an individual wild type.
                    } 
                    //  0 1 1 
                    else  {
                        std::cout<<__FILE__<<":"<< __LINE__ <<" This is the ELSE case"<<std::endl;
                        return true;} // This is the case that WT has not been computed for this mutant, Individual wild type has been requested, another wild type is available (but will not be used)

                    */  

                    /*else if (foldx_energy_wild_type_for_this_mutant_string.compare("NO-RESULT") == 0) && ((queryValueAsString(computeIndividualWildTypeRequestedQuery)).compare("1") != 0)
                    if (foldx_energy_wild_type_for_this_mutant_string.compare("NO-RESULT") != 0) { return false;} // If wild type energy has been computed, there is no need to recompute, ever.
                    else if ((queryValueAsString(computeIndividualWildTypeRequestedQuery)).compare("1") == 0) {return true;}  // If computeIndividualWildType is TRUE, and we have not yet obtained wild type energy for this mutant, we need to run now 
                          
                    else if (foldx_energy_wild_type_string.compare("NO-RESULT") == 0) {return true;} // No wild type energy is available. So we should run the wild type, even if computeIndividualWildType is FALSE.
                    else { 
                        // foldx_energy_wild_type has been computed SOMEWHERE. We should not recompute, then, unless this was a case of computeIndividualWildType TRUE.
                        std::string updateWildTypeEnergyQuery = std::string("update results set foldx_energy_wild_type = "  ) + foldx_energy_wild_type_string + " where  jobName = \"" + getJobID() + "\" and mutationString = \"" +         mutationString + "\";"  ;
                        queryValueAsString(updateWildTypeEnergyQuery); // The wild type energy is available elsewhere. We will go ahead and use that energy to update the job wildTypeMutationString wild type energy for future convenience.
 			return false; // There is no need to run an individual wild type.
                    }*/
                //};
                virtual std::string runGromacsSimulationString(std::string mySingleMutantID,  std::string mySingleMutantDirectory,  bool computeIndividualWildType ) {std::cout<<__FILE__<<":"<<__LINE__<<" Never call this function in the parent!"<<std::endl; exit(1);};
                virtual std::string runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType) {std::cout<<__FILE__<<":"<<__LINE__<<" Never call this function in the parent!"<<std::endl; exit(1);}
};
#endif
