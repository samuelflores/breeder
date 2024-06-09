// DBManager.cpp
//#include "my_global.h" // Include this file first to avoid problems
 
#ifdef ENABLE_SQLITE
#include <sqlite3.h>
#endif

#include <sys/stat.h>
#include <cstring>
#include "DBManager.h"       
#include <iostream>
#include<fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <regex>
#include <ctgmath>
#include "BiopolymerClass.h"

    std::string   DBManager::getPdbResidueIDFromRenumberedResidueNumber(std::string myChainId, int renumberedResidueNumber){
        std::string sequenceTableSelectionString = " where renumberedResidueNumber = " + to_string(renumberedResidueNumber) + " and pdbId = '" + getPdbId() + "' and chainId = '" + myChainId + "' ";
        std::string residueNumberQueryString = " select (pdbResidueNumber) from sequence " + sequenceTableSelectionString + " ; ";
        std::string insertionCodeQueryString = " select (insertionCode) from sequence " + sequenceTableSelectionString + " ; ";
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Query  : "<<insertionCodeQueryString<<std::endl; 
        std::string insertionCode = queryValueAsString(insertionCodeQueryString);
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Result : >"<<insertionCode<<"< "<<std::endl; 
        if (insertionCode == " ") {
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" insertionCode was a space: >" << insertionCode <<"< ";
            insertionCode = "";
            std::cout << " and is now converted to zero length string: >"<<insertionCode <<"< "<<std::endl;
        } // Convert a space to zero length string.
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Query  : "<<residueNumberQueryString<<std::endl; 
        int      pdbResidueNumber = queryValueAsInt(residueNumberQueryString);
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Result : "<<pdbResidueNumber<<std::endl; 
        std::string queryResult = to_string(pdbResidueNumber) + insertionCode;
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Returning : >"<<queryResult<<"< "<<std::endl;
        return queryResult;
    }
    int         DBManager::getNumSequenceTableEntries(){
        std::string sequenceTableSelectionString = " where  pdbId = '" + getPdbId() + "'  ";
        std::string query = " select count(*) from sequence " + sequenceTableSelectionString + " ; ";
        return queryValueAsInt(query);
    }

    void        DBManager::insertSequenceRow (std::string myChainId, int myRenumberedResidueNumber, int myPdbResidueNumber, char myInsertionCode, std::string myAminoAcidType){
    // For some reason this has to be void. Any return value means breeder crashes.
    // This is a much cheaper version of updateOrInsertSequenceRow . The latter is safer because it updates rows that already exist. This one will simply try to insert a row, and relies on mysql already having primary keys identified that prevent more than one row fro having the same pdb id, chain id, and renumbered residue number.
         string myInsertQueryString = "insert into sequence (pdbId, chainId, renumberedResidueNumber, pdbResidueNumber, insertionCode, aminoAcidType) VALUES ( '" + getPdbId() + "' , '" + myChainId + "' , " + std::to_string(myRenumberedResidueNumber)  + " , " + std::to_string(myPdbResidueNumber) + " , '" + std::string(1,myInsertionCode) + "' , '" + myAminoAcidType + "') ; "; 
         insertUpdateDelete( myInsertQueryString ); 
         std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;  
    };

    void        DBManager::updateOrInsertSequenceRow (std::string myChainId, int myRenumberedResidueNumber, int myPdbResidueNumber, char myInsertionCode, std::string myAminoAcidType){
    // For some reason this has to be void. Any return value means breeder crashes.
         // A cheaper version of this is insertSequenceRow .
         std::string selectionString = " pdbId = '" + getPdbId() +"' and chainId = '" + myChainId + "' and renumberedResidueNumber = " + std::to_string(myRenumberedResidueNumber)  + " ";
         std::string myCountQueryString = "select count(*) from sequence where " + selectionString + " ; ";
         std::string returnValue = "";    
         int myCount = queryValueAsInt(myCountQueryString);

         if (myCount == 0) { 
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " counted " << myCount << " entries in sequence table. This is OK. Will proceed with insertion operation." << std::endl; 
             string myInsertQueryString = "insert into sequence (pdbId, chainId, renumberedResidueNumber, pdbResidueNumber, insertionCode, aminoAcidType) VALUES ( '" + getPdbId() + "' , '" + myChainId + "' , " + std::to_string(myRenumberedResidueNumber)  + " , " + std::to_string(myPdbResidueNumber) + " , '" + std::string(1,myInsertionCode) + "' , '" + myAminoAcidType + "') ; "; 
             insertUpdateDelete( myInsertQueryString ); 
             returnValue = "INSERT";
         } 
         else if (myCount == 1) { std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Counted " << myCount << " entries in sequence table. This is OK. Will proceed with update operation."<<std::endl; 
             std::string selectionString2 = std::string(" pdbResidueNumber = ") + std::to_string(myPdbResidueNumber)  + " and  insertionCode =  '" + std::string(1,myInsertionCode) + "', aminoAcidType = '" + myAminoAcidType + "' ";
             std::string myCountQueryString2 = "select count(*) from sequence where " + selectionString + " and " + selectionString2 + " ; ";
             int myCount2 = queryValueAsInt(myCountQueryString);
             if (myCount2 == 1) returnValue = "NOCHANGE";
             else if (myCount2 == 0) {
                 returnValue = "UPDATE";
                 string myUpdateQueryString = "update sequence set pdbResidueNumber = " + std::to_string(myPdbResidueNumber) + " ,  insertionCode =  '" + std::string(1,myInsertionCode) + "', aminoAcidType = '" + myAminoAcidType + "' where " + selectionString + " ; "; 
                 insertUpdateDelete( myUpdateQueryString ); 
             }
         } 
         else if (myCount > 1) { std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Counted " << myCount << " entries in sequence table. This is NOT OK. Exiting now."<<std::endl; exit(1);}
         int myNewCount = queryValueAsInt(myCountQueryString);
         if ((myNewCount - myCount) == 1) {
             if (returnValue == "INSERT"){
                 std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Counted " << myCount << " entries before this function call, and "<< myNewCount <<" entries now. This is OK, because returnValue = "<<returnValue <<std::endl; 
             } else {
                 std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Counted " << myCount << " entries before this function call, and "<< myNewCount <<" entries now. This is NOT OK, because returnValue = "<<returnValue <<std::endl; exit(1);
             }
         // end of if ((myNewCount - myCount) == 1) 
         } else if ((myNewCount - myCount) == 0) {
             if (returnValue == "NOCHANGE"  ) {  
                 std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< "  Counted " << myCount << " entries before this function call, and "<< myNewCount <<" entries now. This is OK, becuase returnValue = ";
                 std::cout <<returnValue<<std::endl; 
              
             } else if (returnValue == "INSERT"){
                 std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Error: Counted " << myCount << " entries before this function call, and "<< myNewCount <<" entries now. This is NOT OK, because returnValue = "<<returnValue  <<std::endl; exit(1);
             }
         }
         std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;  
         //return std::string("");
         //return returnValue;
         
    };
    // columnName is expected to be alternateRenumberedStructureFile or alternatePdbNumberedStructureFile
    void DBManager::insertIntoAlternatePdbStructuresIfAbsent (std::string columnName , std::string columnValue){
         std::string myQueryString1 = "select count(*) from alternatePdbStructures where pdbId = '" + getPdbId() +"'  ; ";
         int myCount = queryValueAsInt(myQueryString1);
         if (  myCount == 0) {
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " This function only inserts " << columnName <<" = "<< columnValue <<" into the alternatePdbStructures IF there is no entry for the PDB ID in question,  "<<getPdbId()<<" . For this PDB ID there are "<<myCount<<" entries. So we DO update this table. In future runs for this PDB ID, the table should not be updated. In that case, you can still update alternatePdbStructures manually, or delete the entry for this PDB ID to let breeder update it next time it is run for this PDB ID, with the -WTSTRUCTUREFILENAME flag. "<<std::endl;
             std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" In this function, we set status = SUCCESS on presumption of future success, to avoid tripping any job cancellations this early in the run. Later, status will be set to FAIL before any dangerous parts of the code."<<std::endl; 
             updateOrInsertInAlternatePdbStructures(columnName, columnValue,std::string("SUCCESS"));
         } else if (myCount == 1) {
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " This function only inserts  " << columnName <<" = "<< columnValue <<" into the alternatePdbStructures IF there is no entry for the PDB ID in question,  "<<getPdbId()<<" . However for this PDB ID there are "<<myCount<<" entries. So we do NOT update this table. You can update alternatePdbStructures manually, or delete the entry for this PDB ID to let breeder update it next time it is run with -WTSTRUCTUREFILENAME flag. "<<std::endl;
         } else {
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " counted "<<myCount<<" entries in alternatePdbStructures with pdbId = >" << getPdbId() << "< . Expected 0 or 1! "<<std::endl;
             exit(1);
         };
    }
  
    // columnName is expected to be either alternateRenumberedStructureFile or alternatePdbNumberedStructureFile
    void DBManager::updateOrInsertInAlternatePdbStructures (std::string columnName, std::string columnValue,std::string myStatus){
         std::string myQueryString1 = "select count(*) from alternatePdbStructures where pdbId = '" + getPdbId() +"'  ; ";
         int myCount = queryValueAsInt(myQueryString1);
         if (  myCount == 0) {
             std::string myQueryString2 = " insert into  alternatePdbStructures ( pdbId,  " + columnName + " , status) VALUES ('" + getPdbId() +"' , '" + columnValue + "' , '" + myStatus + "') ; ";
             insertUpdateDelete(myQueryString2);
         } else if (myCount == 1) {
             std::string myQueryString3 = " update alternatePdbStructures set " + columnName +  "= '" + columnValue + "' , status = '" + myStatus + "' where pdbId = '" + getPdbId() + "' ; ";
             insertUpdateDelete(myQueryString3);
         } else {
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " counted "<<myCount<<" entries in alternatePdbStructures with pdbId = >" << getPdbId() << "< . Expected 0 or 1! "<<std::endl;
             exit(1);
         };
    }
/* // Replaced with the more general updateOrInsertInAlternatePdbStructures, which can update different columns.
    void DBManager::updateOrInsertAlternateStructureFileName (std::string alternateStructureFileName,std::string myStatus){
         std::string myQueryString1 = "select count(*) from alternatePdbStructures where pdbId = '" + getPdbId() +"'  ; ";
         int myCount = queryValueAsInt(myQueryString1);
         if (  myCount == 0) {
             std::string myQueryString2 = " insert into  alternatePdbStructures ( pdbId,  alternateStructureFile, status) VALUES ('" + getPdbId() +"' , '" + alternateStructureFileName + "' , '" + myStatus + "') ; ";
             insertUpdateDelete(myQueryString2);
         } else if (myCount == 1) {
             std::string myQueryString3 = " update alternatePdbStructures set alternateStructureFile = '" + alternateStructureFileName + "' , status = '" + myStatus + "' where pdbId = '" + getPdbId() + "' ; ";
             insertUpdateDelete(myQueryString3);
         } else {
             std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " counted "<<myCount<<" entries in alternatePdbStructures with pdbId = >" << getPdbId() << "< . Expected 0 or 1! "<<std::endl;
             exit(1);
         };
    }
*/
    // This is intended to set equivalence between breeder formatted mutation string and  userProvidedMutationString (presumed in SKEMPI format)
    void DBManager::setUserProvidedMutationStringInDB(std::string breederFormattedMutationString){
       
        size_t firstMinorSeparatorPosition =  getUserProvidedMutationString().find_first_of(MUTATIONMINORSEPARATOR);
       if (firstMinorSeparatorPosition != std::string::npos) {
           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Your userProvidedMutationString contains a breeder-style MUTATIONMINORSEPARATOR = "<<MUTATIONMINORSEPARATOR<<" . Not adding this to the userProvidedMutationString table."<<std::endl;
           return;
       } else {
           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Your userProvidedMutationString does NOT contain a breeder-style MUTATIONMINORSEPARATOR = "<<MUTATIONMINORSEPARATOR<<" . Continuing.."<<std::endl;
       }
         
       std::string myQueryString1 = " select count(*) from userProvidedMutationString where pdbId =  '"  +  getPdbId()  + "' and breederFormattedMutationString = '" + breederFormattedMutationString + "'; ";
       int myCount = queryValueAsInt(myQueryString1);
       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found " <<myCount<<" entries with given PDB ID and breederFormattedMutationString in table userProvidedMutationString. "<<std::endl;
       if    ( myCount != 0) {
           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found " <<myCount<<" entries with given PDB ID and breederFormattedMutationString in table userProvidedMutationString. Not adding this entry."<<std::endl; 
           return;
       } else {

           std::string myQuery = std::string("insert into userProvidedMutationString (pdbId, breederFormattedMutationString, userProvidedMutationString) VALUES ('")    +  getPdbId()  + "',  '" + breederFormattedMutationString + std::string("', '") + getUserProvidedMutationString()  + std::string("' );"   );
           insertUpdateDelete(   myQuery);
       }
    }

    //std::string matchingSubmittedHomologsString (std::string pdbPrimary, std::string pdbHomolog , std::string complexPrimary, std::string complexHomolog, std::string mutationStringPrimary){
    std::string matchingSubmittedHomologsString (std::string pdbPrimary, std::string pdbHomolog ,  std::string complexHomolog, std::string mutationStringHomolog){
        return std::string(" where pdbPrimary = '") + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "'  AND complexHomolog = '" + complexHomolog +"' AND mutationStringHomolog = '" + mutationStringHomolog + "' ; ";
        //return std::string(" where pdbPrimary = '") + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexPrimary = '" + complexPrimary + "' AND complexHomolog = '" + complexHomolog +"' AND mutationStringPrimary = '" + mutationStringPrimary + "' ; ";
    }


    void DBManager::setTemperatureInK(double myTemperature){
        temperature = myTemperature;
        getTemperatureInK(); // this will call getTemperatureInK's validation.
}
    double DBManager::getTemperatureInK(){
        if (temperature < 0.0){
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The temperature of  >"<<temperature<<" is below 0, which is not permitted in Kelvins. You must set the temperature using the -TEMPERATURE flag prior to setting -AFFINITYMUTANT or -AFFINITYWILDTYPE because these are converged to DDG requiring the temperature."<<std::endl;
            exit(1);
        }
        if (temperature < 273){
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The temperature of  >"<<temperature<<" is below the freezing point of water. While this is not entirely unphysical, it is suspicious when we are discussing affinity. Dying now. "<<std::endl; exit(1);
        }
        return temperature;
    }

    double DBManager::dgInkcalmol (double affinityInM) {
        double idealGasR =  1.9858775E-3; //www.cpp.edu/~lllee/gasconstant.pdf   units: kcal/mol/K .. disagrees with wikipedia  in third decimal place.
        double  referenceConcentration = 1.0; // units: 1 mol/L
        return  idealGasR * getTemperatureInK() * log(affinityInM/referenceConcentration);
    }

    void DBManager::setExperimentalDDG(const double mutantAffinityInM, const double wildTypeAffinityInM, const double temperature) {
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have provided mutant affinity (Kd, in M) of : >"<< mutantAffinityInM <<"< . "<<std::endl;
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have provided wild type affinity (Kd, in M) of : >"<< wildTypeAffinityInM <<"< . "<<std::endl;
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have provided temperature (in K) of : >"<< temperature <<"< . "<<std::endl;
        setTemperatureInK(temperature);
        double mutantDGInkcalmol = dgInkcalmol(mutantAffinityInM);
        double wildTypeDGInkcalmol = dgInkcalmol(wildTypeAffinityInM);
        experimentalDDG = mutantDGInkcalmol - wildTypeDGInkcalmol;
        if ((experimentalDDG > 100.0) ||
            (experimentalDDG < -100.0))

        {

            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" ExperimentalDDG is currently "<<experimentalDDG<<" . this is an unreasonable value. "<<std::endl;
            exit(1);
        }
        if (getExperimentalDDG() == experimentalDDG) { //getExperimentalDDG just to make sure it got set
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Successfully set experimentalDDG to "<<getExperimentalDDG()<<std::endl;
        } else {
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Could not set experimentalDDG to "<<experimentalDDG<<" set to "<<getExperimentalDDG()<<std::endl;
            exit(1);
        }
    }

    double DBManager::getExperimentalDDG() {
        return experimentalDDG;
    }


    // Takes a SKEMPI-formatted PDB ID and chain string
    void DBManager::setPdbAndChainsSkempi(const std::string myPdbChains) {
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Processing provided value: >"<<myPdbChains<<"< "<<std::endl;
        size_t firstUnderscorePosition = myPdbChains.find_first_of('_');
            if ((firstUnderscorePosition == 0) ||
                (firstUnderscorePosition == (myPdbChains.length()-1)) ||
                (firstUnderscorePosition == (std::string::npos)))
                {
                    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" First underscore must be in string, and cannot be first or last character. For the string >"<<myPdbChains<<"< , first underscore position returns: >"<< firstUnderscorePosition << "< "<<std::endl; exit(1);
                } else if (firstUnderscorePosition != 4) {
                    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" First underscore can only be at position 4 (counting first position as zero) because the first four characters are reserved for the PDB ID. For the string >"<<myPdbChains<<"< , first underscore position returns: >"<< firstUnderscorePosition << "< "<<std::endl; exit(1);
                }
        std::string myPdbFragment = myPdbChains.substr(0,firstUnderscorePosition);
        setPdbId(myPdbFragment);
        std::string myChainsFragment = myPdbChains.substr(firstUnderscorePosition + 1);
        std::regex myUnderscore("_");
        myChainsFragment = std::regex_replace(myChainsFragment, myUnderscore,",");
        setChainsInMutatedSubunit(myChainsFragment);
    }

    //int DBManager::countEntriesInSubmittedHomologs (std::string pdbPrimary, std::string pdbHomolog , std::string complexPrimary, std::string complexHomolog, std::string mutationStringPrimary) {
    int DBManager::countEntriesInSubmittedHomologs (std::string pdbPrimary, std::string pdbHomolog ,  std::string complexHomolog, std::string mutationStringHomolog) {
       std::string myQueryString1 = " select count(*) from submittedHomologs " + matchingSubmittedHomologsString( pdbPrimary, pdbHomolog , complexHomolog, mutationStringHomolog);
       return  queryValueAsInt(myQueryString1);
    }

    void DBManager::setSubmittedHomologs    (std::string pdbPrimary, std::string pdbHomolog , std::string complexPrimary, std::string complexHomolog, std::string mutationStringPrimary,  std::string mutationStringHomolog   ) {
      // std::string myMatchingChainsSelectionString = matchingChainsSelectionString ( pdbPrimary, pdbHomolog , complexChainPrimary, complexChainHomolog );
      if (countEntriesInSubmittedHomologs(pdbPrimary, pdbHomolog , complexHomolog, mutationStringHomolog )){
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl; 
          std::string myQueryStringA = "select mutationStringPrimary from submittedHomologs " +  matchingSubmittedHomologsString ( pdbPrimary, pdbHomolog , complexHomolog, mutationStringHomolog );
          std::string currentMutationStringPrimary =  queryValueAsString(myQueryStringA);
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl; 
          // If we are changing mutationStringPrimary, this is potentially bad.
          if (currentMutationStringPrimary != mutationStringPrimary) {
              //std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" ERROR! Current mutationStringPrimary is : >"<<currentMutationStringPrimary<<"< which you are trying to change to : >"<<mutationStringPrimary<<"< . This suggests that you have mixed up cleaned with non-cleaned residue numbers, or otherwise made a mistake. Nonthing wrong with deleting all rows in submittedHomologs, to flush such issues. Dying now."<<std::endl;
              //exit(1);
              std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" WARNING! Current mutationStringPrimary is : >"<<currentMutationStringPrimary<<"< which you are trying to change to : >"<<mutationStringPrimary<<"< . This suggests that you have mixed up cleaned with non-cleaned residue numbers, or otherwise made a mistake. Nonthing wrong with deleting all rows in submittedHomologs, to flush such issues. Dying now."<<std::endl;
          }
          std::string myQueryString0 = "update submittedHomologs set mutationStringPrimary = '" + mutationStringPrimary + "' " + matchingSubmittedHomologsString ( pdbPrimary, pdbHomolog , complexHomolog, mutationStringHomolog );
          //std::string myQueryString0 = "update submittedHomologs set mutationStringHomolog = '" + mutationStringHomolog + "' " + matchingSubmittedHomologsString ( pdbPrimary, pdbHomolog , complexHomolog, mutationStringHomolog );
          insertUpdateDelete( myQueryString0 );
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
      } else {
          std::string myQueryString1 = " insert into submittedHomologs (pdbPrimary, pdbHomolog ,complexPrimary, complexHomolog , mutationStringPrimary, mutationStringHomolog ) VALUES ('" + pdbPrimary + "' ,  '" + pdbHomolog + "' , '" + complexPrimary + "' , '" + complexHomolog + "' , '" + mutationStringPrimary + "' , '" + mutationStringHomolog + "' )     ; ";
          insertUpdateDelete( myQueryString1 );
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
      }
      std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl; 
    }

    std::string localRmsdSelectionString (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog , std::string mutationString,double inclusionRadius) {
          return  std::string(" where pdbPrimary = '") + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexChainPrimary = '" + complexChainPrimary + "' AND complexChainHomolog = '" + complexChainHomolog + "' AND mutationString = '" + mutationString + "' AND inclusionRadius = " + std::to_string(inclusionRadius) ;
}

    int DBManager::countEntriesInLocalSequenceIdentity (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog , std::string mutationString,double inclusionRadius ) {
       std::string myQueryString1 = " select count(*) from localSequenceIdentity " + localRmsdSelectionString( pdbPrimary, pdbHomolog , chainPrimary, chainHomolog , mutationString, inclusionRadius);
       return  queryValueAsInt(myQueryString1);
    }

    std::string matchingChainsSelectionString (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog){
        return std::string(" where pdbPrimary = '") + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND chainPrimary = '" + chainPrimary + "' AND chainHomolog = '" + chainHomolog + "' ; ";    
    }

    int DBManager::countEntriesInMatchingChains (std::string pdbPrimary, std::string pdbHomolog ,std::string  chainPrimary, std::string chainHomolog ) {
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< " <<std::endl; 
       std::string myQueryString1 = " select count(*) from matchingChains " + matchingChainsSelectionString( pdbPrimary, pdbHomolog , chainPrimary ,chainHomolog );
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<myQueryString1<<std::endl;
       int numEntries = queryValueAsInt(myQueryString1);
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" returning : "<<numEntries<<std::endl;
       return numEntries;//  queryValueAsInt(myQueryString1);
    }


    void DBManager::setMatchingChainsValue    (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName , double columnValue  ) {
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< " <<std::endl; 
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" setting column >"<<columnName<<"< to value of >"<<columnValue<<"< "<<std::endl; 
      if (countEntriesInMatchingChains(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog )){
          std::string myQueryString0 = "update matchingChains set " + columnName + " = " + std::to_string(columnValue) +  matchingChainsSelectionString ( pdbPrimary, pdbHomolog , chainPrimary,chainHomolog );
          insertUpdateDelete( myQueryString0 );
      } else {
          std::string myQueryString1 = " insert into matchingChains (pdbPrimary, pdbHomolog ,chainPrimary, chainHomolog , " + columnName + ") VALUES ('" + pdbPrimary + "' ,  '" + pdbHomolog + "' , '" + chainPrimary + "' , '" + chainHomolog + "' , " + std::to_string(columnValue) + " )     ; ";
          insertUpdateDelete( myQueryString1 );
      }
    }

    bool   DBManager::matchingChainsValueIsNull      (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName   ) {
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< " <<std::endl; 
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" checking column >"<<columnName<<"< "<<std::endl; 
      if (countEntriesInMatchingChains(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog )){
          std::string myQueryString0 = "select isnull("+columnName+") from matchingChains " +  matchingChainsSelectionString ( pdbPrimary, pdbHomolog , chainPrimary,chainHomolog );
          return queryValueAsInt   ( myQueryString0 );
      } else {
          std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There are no entries for this pdbHomolog, chainPrimary, and chainHomolog . Error!"<<std::endl; 
          exit (1); 
      }
    }

    double DBManager::getMatchingChainsValueAsDouble    (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog ,  std::string columnName   ) {
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< " <<std::endl; 
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" getting column >"<<columnName<<"< "<<std::endl; 
      if (countEntriesInMatchingChains(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog )){
          std::string myQueryString0 = "select sequenceIdentity from matchingChains " +  matchingChainsSelectionString ( pdbPrimary, pdbHomolog , chainPrimary,chainHomolog );
          bool returnValueIsNull; // This is a dummy. Not being used.
          return queryValueAsDouble( myQueryString0 , returnValueIsNull );
      } else {
          std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There are no entries for this pdbHomolog, chainPrimary, and chainHomolog . Error!"<<std::endl; 
          exit (1); 
      }
    }

    // Determine if the chainwise sequence identity is high enough. sequenceIdentityCutoff should be in the range 0.0 to 100.0
    bool DBManager::chainSequenceAlignmentIsSatisfactory(std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog, double sequenceIdentityCutoff ){
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< .. determining whether sequence alignment is satisfactory.." <<std::endl; 
      if (countEntriesInMatchingChains(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog )){
          double mySequenceIdentity = getMatchingChainsValueAsDouble(pdbPrimary, pdbHomolog, chainPrimary, chainHomolog,"sequenceIdentity");
          //sequenceIdentityIsNull = matchingChainsValueIsNull(pdbPrimary, pdbHomolog, chainPrimary, chainHomolog,"sequenceIdentity");
          
          if (mySequenceIdentity >= sequenceIdentityCutoff){
               std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" sequence identity = "<<mySequenceIdentity<<", which is at or over the cutoff of "<<sequenceIdentityCutoff<<". This is satisfactory. Returning True."<<std::endl;   
               return 1;
          //} elsif(matchingChainsValueIsNull(pdbPrimary, pdbHomolog, chainPrimary, chainHomolog,"sequenceIdentity")) {
          //     return 1; // Force calculation of this value. Do not say it is "unsatisfactory". Actually it is just unknown.
          } else {
               std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" sequence identity = "<<mySequenceIdentity<<", which is UNDER the cutoff of "<<sequenceIdentityCutoff<<". This is notsatisfactory. Returning False."<<std::endl;   
               return 0;
          }  
      } else {
          std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There are no entries for this pdbHomolog, chainPrimary, and chainHomolog . Error!"<<std::endl; 
          exit (1); 
      }
    }
    // Determine if the chainwise sequence identity is Null.
    /*bool DBManager::chainSequenceAlignmentIsNull        (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog, double sequenceIdentityCutoff){
      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" for primary pdbId : "<<pdbPrimary <<" , homolog pdb ID  >"<<pdbHomolog<<"< chainPrimary =  >"<<chainPrimary<<"< , chainHomolog =  >"<< chainHomolog <<"< .. determining whether sequence alignment is satisfactory.." <<std::endl; 
      bool mySequenceIdentityIsNull = 0;
      if (countEntriesInMatchingChains(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog )){
          bool   mySequenceIdentityIsNull = getMatchingChainsValueAsDoubleReturnsNull(pdbPrimary, pdbHomolog, chainPrimary, chainHomolog,"sequenceIdentity");
      } 
          if (mySequenceIdentity >= sequenceIdentityCutoff){
               std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" sequence identity = "<<mySequenceIdentity<<", which is at or over the cutoff of "<<sequenceIdentityCutoff<<". This is satisfactory. Returning True."<<std::endl;   
               return 1;
          } else {
               std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" sequence identity = "<<mySequenceIdentity<<", which is UNDER the cutoff of "<<sequenceIdentityCutoff<<". This is notsatisfactory. Returning False."<<std::endl;   
               return 0;
          }  
      } else {
          std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There are no entries for this pdbHomolog, chainPrimary, and chainHomolog . Error!"<<std::endl; 
          exit (1); 
      }
    }*/

    void DBManager::setLocalSequenceIdentity (std::string pdbPrimary, std::string pdbHomolog , std::string chainPrimary, std::string chainHomolog , std::string mutationString,double inclusionRadius, std::string columnName , double columnValue  ) {
          std::string myLocalRmsdSelectionString = localRmsdSelectionString( pdbPrimary, pdbHomolog , chainPrimary, chainHomolog , mutationString, inclusionRadius);
      if (countEntriesInLocalSequenceIdentity(pdbPrimary, pdbHomolog , chainPrimary, chainHomolog , mutationString, inclusionRadius)){
          std::string myQueryString0 = "update localSequenceIdentity set " + columnName + " = " + std::to_string(columnValue) + myLocalRmsdSelectionString;
          insertUpdateDelete( myQueryString0 );
      } else {
          std::string myQueryString1 = " insert into localSequenceIdentity (pdbPrimary, pdbHomolog , chainPrimary, chainHomolog , mutationString, inclusionRadius, " + columnName + ") VALUES ('" + pdbPrimary + "' ,  '" + pdbHomolog + "' , '" + chainPrimary + "' , '" + chainHomolog + "' , '" + mutationString + "' , " + std::to_string(inclusionRadius) + " , " + std::to_string(columnValue) + " )     ; ";
          insertUpdateDelete( myQueryString1 );
      }
    }

   // columnName is expected to be alternateRenumberedStructureFile or alternatePdbNumberedStructureFile. I guess it could also take status .
   std::string DBManager::getFromAlternatePdbStructures(std::string columnName, std::string myPdbId){
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve "<< columnName <<" from table alternatePdbStructures  for pdbId : "<<myPdbId<<std::endl; 
                std::string myQueryString = "select " + columnName + " from alternatePdbStructures where pdbId = \"" + myPdbId + "\" ; ";
                std::string returnValue =  queryValueAsString(myQueryString);;
                if (returnValue == std::string("")){
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Note! " << columnName <<" was an empty string (\"\") : >"<<returnValue <<"< "<<std::endl;
                }
                return returnValue;
	    };

   std::string DBManager::getAlternatePdbNumberedStructureFile(){ // This one is a little unsafe, because you might be retrieving the alternateStructureFile for the Primary when you want the Homolog, or vice versa. better to use getAlternateStructureFileName(std::string myPdbId)
   //std::string DBManager::getAlternateStructureFileName(){ // This one is a little unsafe, because you might be retrieving the alternateStructureFile for the Primary when you want the Homolog, or vice versa. better to use getAlternateStructureFileName(std::string myPdbId)
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve alternateStructureFile for pdbId : "<<getPdbId()<<std::endl; 
                return getFromAlternatePdbStructures("alternatePdbNumberedStructureFile", getPdbId());  
	    };
   
   std::string DBManager::getAlternateRenumberedStructureFile(){ // This one is a little unsafe, because you might be retrieving the alternateStructureFile for the Primary when you want the Homolog, or vice versa. better to use getAlternateStructureFileName(std::string myPdbId)
   //std::string DBManager::getAlternateStructureFileName(){ // This one is a little unsafe, because you might be retrieving the alternateStructureFile for the Primary when you want the Homolog, or vice versa. better to use getAlternateStructureFileName(std::string myPdbId)
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve alternateStructureFile for pdbId : "<<getPdbId()<<std::endl; 
                return getFromAlternatePdbStructures("alternateRenumberedStructureFile", getPdbId());  
	    };
   
   int DBManager::countRmsdEntriesDirectly (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog  ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Counting rmsd entries as queried.."<<std::endl;
                std::string myQueryString1 = "select count(*) from  rmsd      where pdbPrimary = '" + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexChainPrimary = '" + complexChainPrimary + "' AND complexChainHomolog = '" + complexChainHomolog + "' ; ";
                int myCount = queryValueAsInt(myQueryString1);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found "<<myCount<<" entries for the PDB's and complexChains as given. Returning this value.";
                return myCount;
        };
   int DBManager::countRmsdEntriesSwitched (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog  ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Counting rmsd entries with Primary and Homolog terms switched.."<<std::endl;
                int myCountSwitched = countRmsdEntriesDirectly(pdbHomolog,pdbPrimary,complexChainHomolog,complexChainPrimary ); // Note that we switched Primary vs. Homolog in the argument list.
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found "<<myCountSwitched<<" entries for the PDB's and complexChains in switched order. Returning the switched value. "<<std::endl;    
                return myCountSwitched;
        };

   int DBManager::countRmsdEntriesBothWays (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog  ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl;
                int myCount = countRmsdEntriesDirectly(pdbPrimary,pdbHomolog,complexChainPrimary,complexChainHomolog);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found "<<myCount<<" entries for the PDB's and complexChains as given. ";
                int myCountSwitched = countRmsdEntriesSwitched(pdbPrimary,pdbHomolog,complexChainPrimary,complexChainHomolog); // This is the same as countRmsdEntriesDirectly, except with switched arguments.
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found "<<myCountSwitched<<" entries for the PDB's and complexChains in switched order. ";

                if ((! (myCount)) && (myCountSwitched)) {
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found "<<myCount<<" entries for the PDB's and complexChains as given. However we do have "<<myCountSwitched<<" entries for the PDB's and complexChains in switched order. This should be equivalent, assuming proper convergence. Returning the switched value. "<<std::endl;    
                    return myCountSwitched;
                } else {
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Returning "<<myCount<<" entries as found for the PDB's and complexChains as given. "<<std::endl;
                    return myCount;}
        };

   double DBManager::getRmsd (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog  ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl;
                if (countRmsdEntriesDirectly(pdbPrimary,pdbHomolog,complexChainPrimary,complexChainHomolog)){
                    std::string myQueryString1 = "select rmsd.rmsd     from  rmsd      where pdbPrimary = '" + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexChainPrimary = '" + complexChainPrimary + "' AND complexChainHomolog = '" + complexChainHomolog + "' ; ";
                    return queryValueAsDouble(myQueryString1);
                } else if (countRmsdEntriesSwitched(pdbPrimary,pdbHomolog,complexChainPrimary,complexChainHomolog)){
                     std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found no entries in table rmsd for PDBs and complexes as given. However there is an entry for the reverse ordering of Primary vs. Homolog. Retrieving the corresponding rmsd.."<<std::endl; 
                     return getRmsd(pdbHomolog,pdbPrimary,complexChainHomolog,complexChainPrimary); // Note we reverse order of parameters.
                } else {
                     std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Error! Found no entries in rmsd for PDBs and complexes as given, or even in reverse order. You should check for existence of said entry prior to running this query. Dying now."<<std::endl; exit(1);
                };   
        };

   void DBManager::setRmsd (std::string pdbPrimary, std::string pdbHomolog , std::string complexChainPrimary, std::string complexChainHomolog , double myRmsd ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl;
                //std::string myQueryString1 = "select count(*) from  rmsd      where pdbPrimary = '" + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexChainPrimary = '" + complexChainPrimary + "' AND complexChainHomolog = '" + complexChainHomolog + "' ; ";
                 std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl;
                int myJobIDCount = countRmsdEntriesDirectly(pdbPrimary, pdbHomolog, complexChainPrimary, complexChainHomolog);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl;
                /*if (myJobIDCount > 0) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" An entry exists in rmsd table. Deleting that now."<<std::endl;
                    std::string myQueryString2 = "delete from  rmsd      where pdbPrimary = '" + pdbPrimary + "'  AND pdbHomolog = '" + pdbHomolog + "' AND complexChainPrimary = '" + complexChainPrimary + "' AND complexChainHomolog = '" + complexChainHomolog + "' ; ";
                    insertUpdateDelete(myQueryString2);
                }
                std::string myQueryString3 = std::string("insert into  rmsd      ( pdbPrimary , pdbHomolog , complexChainPrimary , complexChainHomolog, rmsd) VALUES ('") + pdbPrimary + "'  ,  '" +   pdbHomolog + "' ,  '" + complexChainPrimary + "' , '" + complexChainHomolog + "'  , " + std::to_string(myRmsd) + " ) ; ";
                insertUpdateDelete(myQueryString3);*/
                if (myJobIDCount > 1) {
	            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Counted "<<myJobIDCount<<" entres in rmsd table. Error!"<<std::endl; exit(1);
                
	        } else if (myJobIDCount == 1) {
		    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Counted "<<myJobIDCount<<" entres in rmsd table. Going with an UPDATE query."<<std::endl;
		    std::string myQueryString3 = "update rmsd set rmsd = "+ std::to_string(myRmsd)+ " where pdbPrimary = '"+ pdbPrimary+ "' AND pdbHomolog = '" + pdbHomolog+ "' AND complexChainPrimary ='" + complexChainPrimary+ "' AND  complexChainHomolog = '"+ complexChainHomolog+ "' ; ";	
                    insertUpdateDelete(myQueryString3);
		
		} else if (myJobIDCount == 0) {
		    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Counted "<<myJobIDCount<<" entres in rmsd table. Going with an INSERT query."<<std::endl;
	            std::string myQueryString3 = std::string("insert into  rmsd      ( pdbPrimary , pdbHomolog , complexChainPrimary , complexChainHomolog, rmsd) VALUES ('") + pdbPrimary + "'  ,  '" +   pdbHomolog + "' ,  '" + complexChainPrimary + "' , '" + complexChainHomolog + "'  , " + std::to_string(myRmsd) + " ) ; ";
                    insertUpdateDelete(myQueryString3);
		} else {
	            // Surely this error would never occur:
	            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Error! myJobIDCount = "<<myJobIDCount<<std::endl; exit(1);
		}
        };

 
   void DBManager::setJobID(std::string myJobID){
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"   runID = >"<<  runID<<"<"<<std::endl;
		runID = myJobID;
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"   runID = >"<<  runID<<"<"<<std::endl;
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl; 
                std::string myQueryString = "select runID    from jobInfo where not ISNULL(runID) and runID = \"" + runID      + "\" ; ";
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl; 
                std::string myJobIDCount = queryValueAsString(myQueryString);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" "<<std::endl; 
                if (myJobIDCount.compare(runID) == 0) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" An entry exists in jobInfo table with runID = "<< runID<<"."<<std::endl;}
                else if (myJobIDCount.compare(runID) != 0) 
                {   std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" No entry exists in jobInfo with runID = "<< runID<<". Will now create said entry."<<std::endl;
                    myQueryString = "insert into jobInfo (runID) VALUES (\""+runID + "\");",    
                    insertUpdateDelete(myQueryString);
                } else {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Unreasonable result : "<<myJobIDCount<<std::endl; exit(1);} // Just being paranoid        
	};
   void         DBManager::setJobLibraryPath(const std::string myJobLibraryPath = ""){
                // This is not in use currently, since jobLibraryPath is just set by the user directly in mysql.  But it should be made possible to run as a command line parameter as well.
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to set job library path .. "<<std::endl; 
                std::string myQueryString = "update jobInfo set jobLibraryPath = \"" + myJobLibraryPath + "\" where runID = \"" + getJobID() + "\" ; ";
                insertUpdateDelete(myQueryString);
		};
   void DBManager::setPdbId(std::string myPdbId){
                ////std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" pdbId was >"<<  pdbId<<"<"<<std::endl;
                if (myPdbId.length() == 4) {
		    pdbId = myPdbId;}
                else {
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Attempted to change pdbId to >"<<myPdbId<<"< . However this has length "<<myPdbId.length()<<std::endl; exit (1);
                }
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" pdbId is now >"<<  pdbId<<"<"<<std::endl;
	};

   void DBManager::setWorkingDirectory(const std::string myWorkingDirectory) {

		if (myWorkingDirectory.length() > 0) {
		    std::cout<<__FILE__<<":"<<__LINE__<<" You have specified workingDirectory  = >"<<myWorkingDirectory <<"< "<<std::endl;
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<": About to call myMkdir("<<myWorkingDirectory<<") "<<std::endl;
                    myMkdir(myWorkingDirectory);
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<": done with  myMkdir("<<myWorkingDirectory<<") "<<std::endl;
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
		    //--dbConnection->setWorkingDirectory(workingDirectory);
		    // we are no longer setting workingDirectory in jobInfo. This is because there could be two breeder sessions going at once with different pdbId but same jobName.    
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<": workingDirectory = >"<<workingDirectory<<"< "<<std::endl;
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<": which we will set to myWorkingDirectory, or  >"<<myWorkingDirectory <<"< "<<std::endl;
		    workingDirectory = myWorkingDirectory;   
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<":"<<std::endl;
                    //dbManager->setWorkingDirectory(myWorkingDirectory); // This should be the sole way to set the working directory 
		} 
		else {
		    std::cout<<__FILE__<<":"<<__LINE__<<" You have not specified a valid value of workingDirectory, apparently: >" <<myWorkingDirectory<<"<.  "<<std::endl;
		    exit(1);
                }
                //std::cout<<__FILE__<<":"<<__LINE__<<" Should not get to this point."<<std::endl;
                //exit(1);                                    
	        };
   /*
   void DBManager::setWildTypeStructureFileName(const std::string wtStructureFileName) {
       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Obsolete! Use the newer function, insertAlternateStructureFileNameIfAbsent instead."<<std::endl;
       exit(1);
	        validate();
                struct stat st;
                std::cout <<__FILE__<<":"<<__LINE__<< "Checking status of provided structure file, named >"<<wtStructureFileName <<"< : "<< stat(wtStructureFileName.c_str(),&st) << std::endl;
                if (st.st_mode & S_IRUSR) {
                    std::cout << " Owner appears to have write permissions on "<<wtStructureFileName <<"."<<std::endl;
                } else {
                    std::cout << " Owner does NOT appear to have read  permissions on input PDB structure file >"<<wtStructureFileName <<"<. Please confirm that the file exists and is accessible. If the file name is blank or has zero length, then you have not provided it. Please set the -WTSTRUCTUREFILENAME breeder command line argument, or the wtStructureFileName column in the jobInfo table."<<std::endl; exit(1);
                }
		    std::string myQueryString;
		    myQueryString = std::string("update jobInfo set  wildTypeStructureFileName = \"") +wtStructureFileName +  std::string("\"   where runID = \"") + runID + std::string("\" limit 1;");
                    std::cout <<__FILE__<<":"<<__LINE__<< "About to issue : "<< myQueryString  << std::endl;
		    insertUpdateDelete (myQueryString);
                    wildTypeStructureFileName = wtStructureFileName;
                    if (getWildTypeStructureFileName() != wtStructureFileName) {
                        std::cout <<__FILE__<<":"<<__LINE__<< " Error! getWildTypeStructureFileName() returned >"<< getWildTypeStructureFileName()   <<"< "<< std::endl;
                        exit(1);
                    }
	        };*/
   void DBManager::setPartition       (const std::string myPartition       ) {
	        validate();
		std::string myQueryString;
		myQueryString = std::string("update jobInfo set `partition`   = \"") + myPartition        +  std::string("\"   where runID = \"") + runID + std::string("\" limit 1;");
		insertUpdateDelete (myQueryString);
	        };
   void DBManager::setChainsInMutatedSubunit(const std::string myChainsInMutatedSubunit) {

       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Processing provided value: >"<<myChainsInMutatedSubunit<<"< "<<std::endl;

       if (getChainsInMutatedSubunit() != "") {
           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" For safety we do not allow you to overwrite chainsInMutatedSubunit. It has already been set to : >"<<getChainsInMutatedSubunit()<<" . "<<std::endl; exit(1);
       }
       size_t commaPosition = myChainsInMutatedSubunit.find(',',1);
       if ((commaPosition == 0) ||
           (commaPosition == (myChainsInMutatedSubunit.length()-1)) ||
           (commaPosition == (std::string::npos)))
       {
           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Comma must be in string, and cannot be first or last character. For the string >"<<myChainsInMutatedSubunit<<"< , comma position returns: >"<< commaPosition << "< "<<std::endl; exit(1);
       }

       chainsInMutatedSubunit = myChainsInMutatedSubunit;


       //std::string updateComplexStringInResultsQuery = std::string("update results set complexString = \"" + myChainsInMutatedSubunit + "\" where  "+ jobIDAndPdbIdSelectionString() + " and mutationString = \"" +         getUserProvidedMutationString() + "\";" ) ;
       //insertUpdateDelete(updateComplexStringInResultsQuery);        
   };

   void DBManager::setComplexStringInResultsTable( const std::string myMMBFormattedMutationString) {
       std::string condition = " where  "+ jobIDAndPdbIdSelectionString() + " and mutationString = \"" + myMMBFormattedMutationString + "\";";
       std::string updateComplexStringInResultsQuery = std::string("update results set complexString = \"" + getChainsInMutatedSubunit() + "\"" + condition); //"\" where  "+ jobIDAndPdbIdSelectionString() + " and mutationString = \"" +  myMMBFormattedMutationString + "\";" ) ;
       insertUpdateDelete(updateComplexStringInResultsQuery);        
       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Checking the complexString in results table using similar query.. "<<std::endl;
       std::string myCheck = queryValueAsString("select complexString from results " + condition);
       if (myCheck.compare(getChainsInMutatedSubunit()) != 0){
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" ERROR! The results table has : >" <<myCheck<<"< "<<std::endl;
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" We expected : >" << getChainsInMutatedSubunit()  <<"< "<<std::endl;
          exit(1);
       } 
       else { std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got : >" + myCheck + "<" <<std::endl;
          std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" We expected : >" << getChainsInMutatedSubunit()  <<"< "<<std::endl;}
   };

   std::string DBManager::getChainsInMutatedSubunit(){
       //std::string myQueryString = "select complexString from results where "+ jobIDAndPdbIdSelectionString() + " and mutationString = \"" +  breederFormattedMutationString + "\";"  ;
       //return queryValueAsString(myQueryString);
       return chainsInMutatedSubunit;
   }

   void DBManager::setNumReportingIntervals (const int myNumReportingIntervals) {
       std::stringstream myQueryStringStream ; myQueryStringStream << std::string("update jobInfo set numReportingIntervals = ") << myNumReportingIntervals <<  std::string(" where runID = \"") << getJobID() <<  std::string("\" limit 1;");
 
       std::string myQueryString = myQueryStringStream.str();// std::string("update jobInfo set numReportingIntervals = ") + myNumReportingIntervals + std::string(" where runID = \"") + runID + std::string("\" limit 1;");
       insertUpdateDelete (myQueryString);
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to confirm that numReportingIntervals was set correctly .."<<std::endl;
       getNumReportingIntervals(); // We call this function to confirm that numReportingIntervals was set, but also because it has a validation check.
       }; 
   void DBManager::setReportingInterval (const double  myReportingInterval) {
       std::stringstream myQueryStringStream ; myQueryStringStream <<std::string("update jobInfo set reportingInterval = ") <<myReportingInterval<< std::string(" where runID = \"") <<getJobID() << std::string("\" limit 1;");
       std::string myQueryString = myQueryStringStream.str(); // = std::string("update jobInfo set reportingInterval = ") + myReportingInterval +  std::string(" where runID = \"") + runID + std::string("\" limit 1;");
       insertUpdateDelete (myQueryString);
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to confirm that reportingInterval was set correctly .."<<std::endl;
       getReportingInterval(); // We call this function to confirm that reportingInterval was set, but also because it has a validation check.
       }; 
   void DBManager::setFlexibilityWindowOffset(const int myFlexibilityWindowOffset){
       if ((myFlexibilityWindowOffset < 0) ||
           (myFlexibilityWindowOffset > 10))  
       {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have requested an invalid flexibilityWindowOffset, of "<<myFlexibilityWindowOffset<<" . The offset must be zero at a minimum. We have verified that values greater than 2 are perjudicial."<<std::endl; exit(1);  
       }
       std::stringstream myQueryStringStream ; myQueryStringStream << std::string("update jobInfo set flexibilityWindowOffset = ") << myFlexibilityWindowOffset <<  std::string(" where runID = \"") <<getJobID()  <<  std::string("\" limit 1;");
       std::string myQueryString = myQueryStringStream.str();// std::string("update jobInfo set numReportingIntervals = ") + myNumReportingIntervals + std::string(" where runID = \"") + runID + std::string("\" limit 1;");
       insertUpdateDelete (myQueryString);
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to confirm that flexibilityWindowOffset was set correctly .."<<std::endl;
       getFlexibilityWindowOffset(); // We call this function to confirm that flexibilityWindowOffset was set, but also because it has a validation check.
   } 
   int DBManager::getFlexibilityWindowOffset(){
       std::string myQueryString = "select flexibilityWindowOffset from jobInfo where runID = \'" + getJobID() + "\' limit 1;";                                        
       int myFlexibilityWindowOffset = queryValueAsInt(myQueryString);
       if ((myFlexibilityWindowOffset < 0) ||
           (myFlexibilityWindowOffset > 10))  
       {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have retrieved an invalid flexibilityWindowOffset, of "<<myFlexibilityWindowOffset<<" . The offset must be zero at a minimum. We have verified that values greater than 2 are perjudicial."<<std::endl; exit(1);  
       } else {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have retrieved a flexibilityWindowOffset of "<<myFlexibilityWindowOffset<<" . All appears in order."<<std::endl;
           return myFlexibilityWindowOffset;
       }
       exit(1); // should never reach this point.
   }  
   void DBManager::setLastStage(const int myLastStage) {
       std::stringstream myQueryStringStream ; myQueryStringStream << std::string("update jobInfo set lastStage = ") << myLastStage <<  std::string(" where runID = \"") << getJobID() <<  std::string("\" limit 1;");
 
       std::string myQueryString = myQueryStringStream.str();// std::string("update jobInfo set lastStage = ") + myLastStage + std::string(" where runID = \"") + runID + std::string("\" limit 1;");
       insertUpdateDelete (myQueryString);
       std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to confirm that lastStage was set correctly .."<<std::endl;
       getLastStage(); // We call this function to confirm that lastStage was set, but also because it has a validation check.
       }; 
   std::string DBManager::getJobLibraryPath(){
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve job library path.. "<<std::endl; 
                std::string myQueryString = "select jobLibraryPath from  jobInfo where runID = \"" + getJobID() + "\" ; ";
                return queryValueAsString(myQueryString);;
		};
   std::string DBManager::getAccountName(){
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve account name.. "<<std::endl; 
                return account;
		};
        
   std::string DBManager::getJobID(){
		return    runID;
	};
   std::string DBManager::getPdbId(){
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve pdbId :  "<<pdbId <<std::endl; 
                if (  pdbId.length() == 4) {
		    // Validation passed. 
		}         
                else {
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Attempted to get pdbId  >"<<   pdbId<<"< . However this has length "<<   pdbId.length()<<std::endl; exit (1);
                }
                return pdbId;  // This is just a local variable, since one runID could have many pdbId's.                               
   };
   bool  DBManager::hasEntriesInAlternatePdbStructures(){
       std::string myQuery = "select count(*) from alternatePdbStructures where pdbId = \"" + getPdbId() + "\" ; ";
       int numEntries = queryValueAsInt(myQuery);
       if (numEntries == 0) {return 0;}
       else if (numEntries == 1) {return 1;}
       else {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" pdbId is a primary key. Somehow this query returned "<<numEntries<<" entries with pdbId = "<< getPdbId() <<" . Dying now."<<std::endl;;
       }
   }
   bool DBManager::getPdbStatusFail(){
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to determine if PDB has status FAIL .."<<std::endl;
        if ( hasEntriesInAlternatePdbStructures()) {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Verified that there is exactly one entry for pdbId "<<  getPdbId()  <<" . Will now get status. " <<std::endl;
           std::string myQueryString = "select status from alternatePdbStructures  where  pdbId = \"" + getPdbId() + "\" ; "; // For now, assuming pdbId is same as jobID
           std::string result = queryValueAsString(myQueryString);
           if (result.compare("FAIL") == 0) {
               return 1; // 1 means job has failed in the past 
           } else return 0;
        } else {
           std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  There are no entries in alternatePdbStructures for pdbId "<<  getPdbId()  <<" . This probably means the job has not run before. Returning NOT FAIL. " <<std::endl;
           return 0;
        }
   }

    void DBManager::setPdbStatusFail(){

        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to set PDB status to FAIL for pdbId "<<  getPdbId()   <<std::endl;
        if ( hasEntriesInAlternatePdbStructures()) {
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Verified that there is exactly one entry for pdbId "<<  getPdbId()  <<" . Will now set status to FAIL" <<std::endl;
            std::string myQueryString = "update alternatePdbStructures set status = \"FAIL\" where  pdbId = \"" + getPdbId() + "\" ; "; // For now, assuming pdbId is same as jobID
            insertUpdateDelete(myQueryString);
        } else {
            std::string myQuery2 = "insert into alternatePdbStructures (pdbId, alternateStructureFile, status) VALUES (\"" + getPdbId() + "\" , \"\" , \"FAIL\");";
            insertUpdateDelete(myQuery2);
        }

        if (getPdbStatusFail()) {
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Confirmed that PDB status is  FAIL for pdbId "<<  getPdbId()   <<std::endl;
        } else {
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  PDB status is NOT FAIL for pdbId "<<  getPdbId() <<" . This means something is wrong!"  <<std::endl;
            exit(1);
        }

    }

    void DBManager::setPdbStatusSuccess(){
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to set PDB status to SUCCESS for pdbId "<<  getPdbId()   <<std::endl;
        std::string myQueryString = "update alternatePdbStructures set status = \"SUCCESS\" where  pdbId = \"" + getPdbId() + "\" ; "; // For now, assuming pdbId is same as jobID
        insertUpdateDelete(myQueryString);
        if (!(getPdbStatusFail())) {
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Confirmed that PDB status is  NOT FAIL for pdbId "<<  getPdbId()   <<std::endl;
        } else {
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  PDB status is  FAIL for pdbId "<<  getPdbId() <<" . This means something is wrong!"  <<std::endl;
            exit(1);
        }
        
    }
	std::string DBManager::getPartition () { 
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve partition .."<<std::endl; 
		std::string myQueryString = "select `partition` from jobInfo where not ISNULL(`partition`) and runID = \"" + getJobID() + "\" ; ";
		return queryValueAsString(myQueryString);}

// returns 1 for OK, 0 for problem
int DBManager::checkFileStatus(std::string myFileName){

    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    ifstream testFile(myFileName.c_str());
    int returnValue = (testFile.good()); // Returns true for good file, false for bad file.
    testFile.seekg(0, ios::end);
    int file_size = testFile.tellg();
    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" File >"<< myFileName << " has size : >"<<file_size << "< . "        <<std::endl;
    testFile.close();
    if (!(returnValue)){ // case file is bad or does not exist:
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Owner does NOT appear to have read  permissions on  file >"<<myFileName <<"<. Please confirm that the file exists and is accessible."<<std::endl;
        //exit(1);

    } else {
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Owner DOES appear to have read  permissions on  file >"<<myFileName <<std::endl;
    }    
    if (file_size == 0) { 
        returnValue = 1; // return OK. Because maybe it was just busy downloading under command of another job. 
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The file being checked has zero size. Not going to remove it, though. Returning "<<returnValue<<std::endl;
        //system((std::string("rm ") + std::string(myFileName)).c_str());
    }    

    return ((returnValue)); 


/*
            ifstream testFile(myFileName.c_str());
            int returnValue = !(testFile.good());
            testFile.close();
            if (returnValue){ // case file is bad or does not exist:
		std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Owner does NOT appear to have read  permissions on  file >"<<myFileName <<"<. Please confirm that the file exists and is accessible."<<std::endl;
                //exit(1);
            
            } else {
		std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Owner DOES appear to have read  permissions on  file >"<<myFileName <<std::endl;
                
            }
            return returnValue;

*/



            /*
	    struct stat st;
            if ( (stat(myFileName.c_str(),&st) ) == -1){
                std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Error checking status of  input PDB structure file >"<<myFileName <<"<. Please confirm that the file exists  and is accessible. "<<std::endl;
                return 1;
            }
	    //std::cout << "Checking status of structure file from jobInfo table, named >"<<myFileName <<"< : "<< stat(myFileName.c_str(),&st) << std::endl;
	    if (st.st_mode & S_IRUSR) {
		std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner appears to have write permissions on "<<myFileName <<"."<<std::endl;
                return 0;
	    } else {
		std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Owner does NOT appear to have read  permissions on input PDB structure file >"<<myFileName <<"<. Please confirm that the file exists and is accessible. If the file name is blank or has zero length, then you have not provided it. Please set the -WTSTRUCTUREFILENAME breeder command line argument, or the wildTypeStructureFileName column in the jobInfo table."<<std::endl; 
                return 1;
                //exit(1);
	    }
            */
}       

// New way. 
std::string DBManager::getRenumberedWildTypeStructureFileName () {
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    return getFromAlternatePdbStructures("alternateRenumberedStructureFile",getPdbId());
}

std::string  DBManager::getDefaultPdbNumberedStructureFileName(){   //(std::string myStructureFileName){
    // This is the default file name in case no such file has been manually prepared. Should think of some way to prevent the manually prepared version is never overwritten. getDefaultPdbNumberedStructureFileName() - indicated file CAN be overwritten with the wget command below.
    std::string defaultStructureFileName = getWorkingDirectory() + getPdbId() + std::string(".pdb");
    // should check that the  getWorkingDirectory()   exists and is readable and writeable. Do NOT check the full defaultStructureFileName, because it may not have been writen yet.
    if (access(getWorkingDirectory().c_str(), R_OK) == 0) { 
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found that we have read access to a directory called "<< getWorkingDirectory()   <<" . So far so good."<<std::endl;
    } else {
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Heads up! Found that we do NOT have read access to a directory called "<< getWorkingDirectory()   <<" . Maybe you have not yet set this up. That may be OK."<<std::endl; //exit(1);
    }    
    if (access(getWorkingDirectory().c_str(), W_OK) == 0) { 
    //if (access(getWorkingDirectory().c_str(), W_OK) == 0) {
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Found that we have write access to a directory called "<< getWorkingDirectory()   <<" . So far so good."<<std::endl;
    } else {
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Heads up! Found that we do NOT have write access to a directory called "<< getWorkingDirectory()   <<" . Maybe you have not yet set this up. That may be OK. "<<std::endl; //exit(1);
    }    
    return defaultStructureFileName;   
}


// New way. Now we will compose a PDB-numbered wild type structure file name, and then if it does not exist we will fetch it from the PDB.                                           
std::string DBManager::getPdbNumberedWildTypeStructureFileName () {

    std::string myWildTypeStructureFileName = getAlternatePdbNumberedStructureFile();
    if (getAlternatePdbNumberedStructureFile() == std::string("")) {
        // We have a default location for structure files. Since we have nothing in table:alternatePdbStructures column:alternatePdbNumberedStructureFile, then we will compose a name.
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Composing a default wild type structure file name"<< std::endl; 
        myWildTypeStructureFileName = getDefaultPdbNumberedStructureFileName();
        //getWorkingDirectory() + std::string("/") + getPdbId()+std::string(".pdb");
    if (!(checkFileStatus(myWildTypeStructureFileName))){
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Unable to find a readable PDB file at "<<myWildTypeStructureFileName<<" . Will now fetch one from RCSB."<<std::endl;
        std::string systemCall2 = std::string(" cd ")    + getWorkingDirectory() + std::string(" ;  rm ") + myWildTypeStructureFileName + std::string(" ; wget -nc http://www.rcsb.org/pdb/files/") + getPdbId() + std::string(".pdb ") +  myWildTypeStructureFileName  ; 
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue : "<<systemCall2<<std::endl;
        system(systemCall2.c_str());
        // Check again. This time, if there is no readable file, die.
        if (!(checkFileStatus(myWildTypeStructureFileName))){
            std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner does NOT appear to have read  permissions on input PDB structure file >"<<myWildTypeStructureFileName <<"<. Please confirm that the file exists and is accessible. If the file name is blank or has zero length, then you have not provided it. Please set the -WORKINGDIRECTORY  breeder command line argument (so we know where to put the file we fetch from the PDB), or the wildTypeStructureFileName column in the jobInfo table (if you have an alternate file you want to use)."<<std::endl; 
        exit(1);}
    }
    } else { // In case we got the file name from the alternatePdbStructures table, then we don't fetch. We just test for existence and die otherwise.
        if (!(checkFileStatus(myWildTypeStructureFileName))){
            std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner does NOT appear to have read  permissions on input PDB structure file >"<<myWildTypeStructureFileName <<"<. We got this file name from the alternatePdbStructures table. It would therefore not be appropriate to fetch from RCSB. "<<std::endl;
        exit(1);}

    }
    return myWildTypeStructureFileName;
}

        /*  
	std::string DBManager::getPdbNumberedWildTypeStructureFileName () {
            std::string myWildTypeStructureFileName = "";
            
            if (getAlternatePdbNumberedStructureFileName() != std::string("")) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There appears to be a non-empty alternateStructureFile in the alternatePdbStructures table"<<std::endl; 
                myWildTypeStructureFileName = getAlternateStructureFileName();
                if (checkFileStatus(myWildTypeStructureFileName)){
                    std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner does NOT appear to have read  permissions on input PDB structure file >"<<myWildTypeStructureFileName <<"<. This is a file which you have specified in the alternatePdbStructures table."<<std::endl;
                    exit(1);}
            } else 
            { // We have a default location for structure files:
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Composing a default wild type structure file name.. Actually if you are doing this something is wrong. alternatePdbStructures should always be populated at this point. "<< std::endl; 
                // here we will just die now. defaultLocation will be decommissioned!
                exit(1);
                //myWildTypeStructureFileName = defaultLocation;
            }
	    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  Returning "<<myWildTypeStructureFileName<<std::endl;
	    return myWildTypeStructureFileName;
        } */

        /*
        // Old way:
	std::string DBManager::getWildTypeStructureFileName () {
            std::string myWildTypeStructureFileName;
	    std::string myQueryString = "select wildTypeStructureFileName from jobInfo where runID = \"" + getJobID() + "\" ; ";
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  getWorkingDirectory() "<< getWorkingDirectory()<<std::endl; 
            if ( queryValueAsString(myQueryString) != ""){
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  the user provided a wild type structure file name .. "<<std::endl; 
                myWildTypeStructureFileName = queryValueAsString(myQueryString); //wildTypeStructureFileName; 
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"You set the -WTSTRUCTUREFILENAME flag. Retrieved the value : "<<myWildTypeStructureFileName<<std::endl;
            } else if (getAlternateStructureFileName() != std::string("")) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" There appears to be an alternate structure file name in the alternatePdbStructures table"<<std::endl; 
                myWildTypeStructureFileName = getAlternateStructureFileName();
                if (checkFileStatus(myWildTypeStructureFileName)){
                    std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner does NOT appear to have read  permissions on input PDB structure file >"<<myWildTypeStructureFileName <<"<. This is a file which you have specified in the alternatePdbStructures table."<<std::endl;
                    exit(1);}
            } else { // We have a default location for structure files:
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Composing a default wild type structure file name"<< std::endl; 
                myWildTypeStructureFileName = defaultLocation;
                if (checkFileStatus(myWildTypeStructureFileName)){
                    std::string systemCall2 = std::string(" cd ")    + getWorkingDirectory() + std::string(" ;  wget -nc http://www.rcsb.org/pdb/files/") + getPdbId() + std::string(".pdb ") +  myWildTypeStructureFileName  ; 
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue : "<<systemCall2<<std::endl;
                    system(systemCall2.c_str());
                    // Check again. This time, if there is no readable file, die.
                    if (checkFileStatus(myWildTypeStructureFileName)){
	                std::cout <<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< " Owner does NOT appear to have read  permissions on input PDB structure file >"<<myWildTypeStructureFileName <<"<. Please confirm that the file exists and is accessible. If the file name is blank or has zero length, then you have not provided it. Please set the -WORKINGDIRECTORY  breeder command line argument (so we know where to put the file we fetch from the PDB), or the wildTypeStructureFileName column in the jobInfo table (if you have an alternate file you want to use)."<<std::endl; 
                        exit(1);}
                }
            }
	    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"  Returning "<<myWildTypeStructureFileName<<std::endl;
	    return myWildTypeStructureFileName;
        } */
	std::string DBManager::getFoldXRunFileName () {
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve FoldX runfile name      .."<<std::endl; 
		std::string myQueryString = "select foldXRunFileName from jobInfo where runID = \"" + getJobID() + "\" ; ";
		return queryValueAsString(myQueryString);
        }
	std::string DBManager::getSingleMutantFilesDirectory() {return singleMutantFilesDirectory; } 
	std::string DBManager::getWorkingDirectory() {
                if (workingDirectory.length() < 1) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" workingDirectory is >"<<workingDirectory<<"< . This is not allowed!"<<std::endl; exit(1);}
                return workingDirectory;
	};

	double DBManager::getReportingInterval() {
		validate();
		std::string myQueryString;
		myQueryString = std::string("select reportingInterval  from jobInfo where runID = \"") + runID + std::string("\" ;");
		double returnValue = (queryValueAsDouble(myQueryString));
                if (returnValue < 1E-14) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" reportingInterval is too small or negative: "<<returnValue<<" . This is not allowed!"<<std::endl; exit(1);}
		return (returnValue);
	};
	int DBManager::getNumReportingIntervals() {
		validate();
		std::string myQueryString;
		myQueryString = std::string("select numReportingIntervals  from jobInfo where runID = \"") + runID + std::string("\" ;");
                double returnValue = (queryValueAsInt   (myQueryString));
                if (returnValue < 1) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" numReportingIntervals is less than unity      : "<<returnValue<<" . This is not allowed!"<<std::endl; exit(1);}
		return (returnValue);
	};
	int DBManager::getLastStage() {
		validate();
		std::string myQueryString;
		myQueryString = std::string("select lastStage  from jobInfo where runID = \"") + runID + std::string("\" ;");
                int myLastStage = queryValueAsInt(myQueryString);
                if (myLastStage < 0) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" lastStage is less than unity      : "<<myLastStage<<" . This is not allowed!"<<std::endl; exit(1); }  
                else {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" lastStage is  : "<<myLastStage<<" . This is OK!"<<std::endl; }
		return (queryValueAsInt(myQueryString));
	};
	std::string DBManager::getJobName() {
		//return getWorkingDirectory();
		validate();
		return runID;
	}
	int  DBManager::getMaxGenerations(){
		validate();
		std::string myQueryString;
		myQueryString = std::string(" select maxGenerations  from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		return (queryValueAsInt(myQueryString));
	}
	int  DBManager::getMaxMutations  (){
		validate();
		std::string myQueryString;
		myQueryString = std::string(" select maxMutants      from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		return (queryValueAsInt(myQueryString));
	}
	int  DBManager::getGenerationSize(){
		validate();
		std::string myQueryString;
		myQueryString = std::string(" select generationSize  from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		return (queryValueAsInt(myQueryString));
	}
        double DBManager::getDecayConstantsBetweenHighestAndLowest () {
		validate();
		std::string myQueryString;
		myQueryString = std::string(" select decayConstantsBetweenHighestAndLowest  from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		return (queryValueAsDouble(myQueryString));

	}
	double DBManager::getMaximumParentEnergy () {
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to retrieve maximumParentEnergy."<<std::endl;
		std::string myQueryString;
		myQueryString = std::string("select maximumParentEnergy from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		return (queryValueAsDouble(myQueryString));
	}
	bool  DBManager::isNull(std::string variableName){
		std::string myQueryString;
		myQueryString = std::string(" select ISNULL(")+std::string(variableName) + std::string(")  from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" not sure this works"<<std::endl; exit(1);
		return (queryValueAsInt(myQueryString));
	}
        std::string DBManager::getWildTypeSequence() {
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Obsolete call!"<<std::endl; exit(1);
		validate();
		std::string myQueryString;
		myQueryString = std::string(" select wildTypeSequence  from jobInfo where runID = \"") + runID + std::string("\" limit 1;");
		std::string myWildTypeSequence =  (queryValueAsString(myQueryString).c_str());
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Just retrieved wild type sequence for job "<<runID<<" , it's : "<<myWildTypeSequence<<std::endl;
		return myWildTypeSequence; 

	}
	void DBManager::initialize (std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ) {
                #ifdef ENABLE_SQLITE
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" This should never be run in the parent!"<<std::endl;
		exit(1);
                temperature      = -11111.11111;
                experimentalDDG  = 11111.11111;
                chainsInMutatedSubunit = "";
                workingDirectory = ""; 
                wildTypeStructureFileName  = "";
                userProvidedMutationString = "";
		runID.clear();
		server = (char*)myServer.c_str();//SERVER;     
		user = (char*)myUser.c_str(); //USER;   
		password = (char*)myPassword.c_str(); //PASSWORD; /* set me first */
		database = (char*)myDatabase.c_str(); //DATABASE;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" user = "<<  user<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" database = "<<  database<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" password = "<<  password<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" server = "<<  server<<std::endl;
                
		int rc = sqlite3_open(database.c_str() , &db);
		if( rc ){
		      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		      //sqlite3_close(db); 
                      close();
		      exit(1);
		} else {
                      std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Database opened successfully."<<std::endl;
                }
                #endif

	}
        void DBManager::close() {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" This should never be run in the parent!"<<std::endl;
		exit(1);
	}

	void DBManager::validate() {
		if (runID.length() == 0) {
			std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" runID is of zero length! "<<std::endl; exit(1);
		}
	}
        void DBManager::setDefaultValues(){
            // Smeet default values:
            gromacsSimulationTime = 0.;
            workingDirectory = "NOT-YET-SET";
        }
        DBManager::DBManager(){
            setDefaultValues();
        }
	DBManager::DBManager(std::string myServer = SERVER,      std::string myPassword = PASSWORD, std::string myUser = USER, std::string myDatabase = DATABASE ){
            std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Initializing with database = >"<<myDatabase<<"< "<<std::endl;
            // Initialize DB. Also set default values.
            setDefaultValues();
            initialize(myServer,myPassword,myUser,myDatabase);}

	std::string DBManager::getMysqlCommandString() {
		return "/usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys ";
	}

	void DBManager::insertUpdateDelete(std::string queryString ) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to execute query : "<<queryString<<std::endl;
		executeStatement(queryString);
	};
		
        double DBManager::queryValueAsDouble (std::string queryString, bool returnValueIsEmptyString) {

		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Expecting a double"<<std::endl;
                std::string myString = queryValueAsString (queryString);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got the following string : >"<<myString<<"<"<<std::endl;               
                if (myString == "") {
                    returnValueIsEmptyString = 1;
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got an empty string : >"<<myString<<"<. So setting the returnValueIsEmptyString parameter to "<< returnValueIsEmptyString<<std::endl;   
                } else {
                    returnValueIsEmptyString = 0;
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got a non-empty string : >"<<myString<<"<. So setting the returnValueIsEmptyString parameter to "<< returnValueIsEmptyString<<std::endl;   
                }
                double myDouble=  atof(myString.c_str()); 
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Which we interpret as the double : >"<<myDouble<<"<"<<std::endl;
                return myDouble;
	} ;
        /* 
        bool DBManager::queryValueAsDoubleReturnsNull (std::string queryString) {

		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Expecting a double"<<std::endl;
                std::string myString = queryValueAsString (queryString);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got the following string : >"<<myString<<"<"<<std::endl;
                bool returnValue = 0;
                if (myString == "") { returnValue = 1;} // the query indeed returns a null 
                else                { returnValue = 0;} // the query indeed returns a null 
                //double myDouble=  atof(myString.c_str()); 
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Is it Null? Answer is: >"<<returnValue<<"<"<<std::endl;
                return returnValue;
	} ; */
        int DBManager::queryValueAsInt (std::string queryString) {
                /*
		// send SQL query   
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" c_str() equivalent is : "<<queryString.c_str()<<std::endl;
                sqlite3_stmt *ppStmt; //const char **pzTail
                int result = sqlite3_prepare(db, queryString.c_str(),strlen(queryString.c_str())+1 ,  &ppStmt, NULL    );
                if (result != SQLITE_OK) {
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Failed to prepare database"<<std::endl;
                        close();
			exit (1);
                }
                sqlite3_step(ppStmt );
                int returnInt = sqlite3_column_int   (ppStmt, 0)	;
                sqlite3_finalize(ppStmt);
			return returnInt   ;
                */
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Expecting an int"<<std::endl;
                std::string myString = queryValueAsString (queryString);
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Got the following string : >"<<myString<<"<"<<std::endl;
                int myInt =  atoi(myString.c_str()); 
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Which we interpret as the int : >"<<myInt   <<"<"<<std::endl;
                return myInt;
	}; 
        std::string DBManager::queryValueAsString (std::string queryString) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" This should never be run in the parent!"<<std::endl;
		exit(1);
                /*  
		// send SQL query   
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" c_str() equivalent is : "<<queryString.c_str()<<std::endl;
                sqlite3_stmt *ppStmt; //const char **pzTail
                int result = sqlite3_prepare(db, queryString.c_str(),strlen(queryString.c_str())+1 ,  &ppStmt, NULL    );
                if (result != SQLITE_OK) {
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Failed to prepare database.  sqlite3_prepare returned error code: >"<<result<<"<"<<std::endl;
                        close();
			exit (1);
                }
                int stepReturnValue = sqlite3_step(ppStmt );
                if ((stepReturnValue == SQLITE_ROW) || (stepReturnValue == SQLITE_DONE ) ) {
                        std::string returnString;
			if ((stepReturnValue ) == SQLITE_ROW) {
			        returnString =  reinterpret_cast<const char*>(sqlite3_column_text  (ppStmt, 0))	;
				} else {
					std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" returned  SQLITE_DONE, not SQLITE_ROW.  Were you expecting a return value?"<<std::endl;
					returnString = "NO-RESULT-FROM-THIS-OPERATION-WERE-YOU-EXPECTING-ONE?";}
    				sqlite3_finalize(ppStmt);
				return returnString; 
                } else if ((stepReturnValue ) == SQLITE_BUSY ) { // If database is locked, try again:
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Pausing briefly to catch my breath ..."<<std::endl;
                    //sleep(2);
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Database is busy. Trying again.  If this continues for too long, the database is in some sense locked permanently."<<std::endl;
                    return queryValueAsString(queryString); // trying again, recursively.
                } else if (stepReturnValue == SQLITE_ERROR ){
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" SQLITE_ERROR !  "<<std::endl;exit(1);
                } else if (stepReturnValue == SQLITE_MISUSE ){
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" SQLITE_MISUSE ! "<<std::endl;exit(1);
                } else {
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Some sort of error occurred.  No result found! "<<std::endl;exit(1);
                } 
                */
	} ;
	void DBManager::executeStatement (std::string queryString ) { // issue an SQL statement, no results expected.
                // Now just queryValueAsString, and expecting no result.
                queryValueAsString(queryString);
                // old way:
                /*
	        // submits a query, returns a string.  Expects to retrieve exactly one row, dies otherwise.
		// from : http://www.cyberciti.biz/tips/linux-unix-connect-mysql-c-api-program.html
		// Connect to database 
		// send SQL query   
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to issue mysql query : "<<queryString<<std::endl;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" c_str() equivalent is : "<<queryString.c_str()<<std::endl;
                sqlite3_stmt *ppStmt; //const char **pzTail;
                int result = sqlite3_prepare(db, queryString.c_str(),-1 ,  &ppStmt, NULL    );
                if (result != SQLITE_OK) {
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Failed to prepare database.  sqlite3_prepare returned error code: >"<<result<<"<"<<std::endl;
                        close();
			exit (1);
                }
                int stepReturnValue = sqlite3_step(ppStmt );
                if ((stepReturnValue ) == SQLITE_BUSY ) { // If database is locked, try again:
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Pausing briefly to catch my breath ..."<<std::endl;
                    sleep(2);
                    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Database is busy. Trying again.  If this continues for too long, the database is in some sense locked permanently."<<std::endl;
                    executeStatement(queryString);  // trying again, recursively.
                }
		sqlite3_finalize(ppStmt ) ; 
                */
        };

	bool DBManager::acceptanceTest (double energy) {
		double lowestEnergy =  (queryValueAsDouble(std::string("select foldx_energy from results where jobName = \"")+getJobID()+std::string("\" and status = \"completed\" order by foldx_energy  asc limit 1;")));
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Lowest energy in database = "<<lowestEnergy<<std::endl;
		double highestEnergy = (queryValueAsDouble(std::string("select foldx_energy from results where jobName = \"")+getJobID()+std::string("\" and status = \"completed\" order by foldx_energy desc limit 1;")));
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Highest energy in database = "<<highestEnergy<<std::endl;
		double metropolisThreshold = exp(-(energy-lowestEnergy)/(highestEnergy - lowestEnergy ) * getDecayConstantsBetweenHighestAndLowest() );
		double randomNumber = rand();
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Initially generated random number is : "<<randomNumber<<", while RAND_MAX = "<<RAND_MAX<<std::endl;
		randomNumber = randomNumber/RAND_MAX;
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Your proposed energy = "<<energy<<" was compared to the metropolis criterion of "<<metropolisThreshold<<" with a random number of "<<randomNumber<<std::endl; 
                if ( randomNumber < metropolisThreshold)
		{
			std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The proposed mutant is ACCEPTED"<<std::endl;
			return true;
		} else {
			std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The proposed mutant is REJECTED"<<std::endl;
			return false;
		}

	};
        //std::string jobIDAndPdbIdSelectionString(){
        //    return " jobName = \"" + getJobID() + "\" and pdbId = \'" + getPdbId()  + "\' ";
        //}  
	int DBManager::getNumJobs(std::string status,std::string mutationString) {
                //std::string statusJobCountFile = "/bubo/home/h17/samuelf/jobCount.txt";
		std::string queryString ="";
                queryString += " select count(*) from results where status like \'";
                queryString += status;
		queryString += "\'  and ";
                queryString += jobIDAndPdbIdSelectionString(); 
		//queryString += " and jobName = \'";
		//queryString += getJobID();      
		//queryString += "\' and pdbId = \'" ;
                //queryString += getPdbId();
		queryString += " and mutationString = \'" ;
                queryString += mutationString;
                queryString += "\' ;  "; 
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to submit query : "<<queryString<< std::endl;              
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Note the use of 'status like ' .. this is a bit dangerous because certain characters could be interpreted as wildcards."<< std::endl; 
		return queryValueAsInt(queryString);
	};
	int DBManager::getNumJobs(std::string mutationString) {
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" About to look for jobs with status '%', which should be interpreted as ANY. "<< std::endl;              
		return getNumJobs("%",mutationString);
	};
        // Retrieves a sequence at random for breeding.  Note that we make sure foldx_energy is available.
        std::string DBManager::retrieveSequenceAtRandom(){
		std::string retrievedSequence;
		double retrievedSequenceFoldxEnergy;
		do     {
				retrievedSequence = queryValueAsString ("  select sequence from results where " + jobIDAndPdbIdSelectionString()  + " and not ISNULL(foldx_energy) order by RAND() limit 1 ; ");
 			std::string retrievedSequenceFoldxEnergyQuery = std::string("  select (foldx_energy - foldx_energy_wild_type) from results where ") + jobIDAndPdbIdSelectionString() + std::string(" and sequence = \"") + retrievedSequence + std::string("\" ; ") ;
		 	retrievedSequenceFoldxEnergy = (queryValueAsDouble(retrievedSequenceFoldxEnergyQuery));
 		} while (!acceptanceTest (retrievedSequenceFoldxEnergy)); // as soon as we pass acceptanceTest, we can break out of the loop.
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Having found an acceptable sequence : "<<retrievedSequence<<", we are moving on .."<<std::endl;
		return retrievedSequence;
        };
        std::string DBManager::getFoldxWildTypeEnergyBaseQuery(std::string wildTypeMutationString){ return  " from results where wildTypeString = \"" + wildTypeMutationString + "\" and " + DBManager::jobIDAndPdbIdSelectionString()  + " and not ISNULL(foldx_energy_wild_type)  ";}   
        double DBManager::getFoldxWildTypeEnergy(std::string wildTypeMutationString){ 
                    //std::string baseQuery =  " from results where wildTypeString = \"" + wildTypeMutationString + "\" and " + jobIDAndPdbIdSelectionString()  + " and not ISNULL(foldx_energy_wild_type)  ";
                    std::string countQuery =  "select count(*) " + getFoldxWildTypeEnergyBaseQuery( wildTypeMutationString) + " limit 1; ";
                    //from results where wildTypeString = \"" + wildTypeMutationString + "\" and " + jobIDAndPdbIdSelectionString()  + " and not ISNULL(foldx_energy_wild_type) limit 1; ";
                    if (queryValueAsInt(countQuery) == 0) {    
                        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Please only call this method if you are sure that the value you seek is available."<<std::endl;
                        exit(1);
                    }
                     
                    std::string foldx_energy_wild_type_query =  "select foldx_energy_wild_type " +  getFoldxWildTypeEnergyBaseQuery( wildTypeMutationString) + " limit 1; "; 
                    std::cout<<__FILE__<<":"<< __LINE__ <<" : " <<std::endl; 
                    return queryValueAsDouble(foldx_energy_wild_type_query);         
        }

        bool DBManager::shouldRunZEMuForJobWildType(std::string mutationString, std::string wildTypeMutationString){ // this tells us whether there is a need to do a ZEMu wild type run for this mutant.
                    // ZEMu is no longer being used. FoldX uses coordinated moves, so we should be computing wild type energy every single time.
                    return true;
                };


         
        // orderField is the field by which we will sort results. order is that order: can be either asc or desc.
        // if index exceeds the number of results in the database, the index is reset to zero, so it can loop again from the beginning.
        std::string DBManager::retrieveSequenceInOrder(int & index, std::string orderField , std::string order){
                
                if ((order.compare("asc") != 0) && (order.compare("desc") != 0)) {std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" the \"order\" parameter must be either asc or desc."<<std::endl; exit(1);} // SCF: get error handling protocol from Alex
		std::string retrievedSequence;
		//double retrievedSequenceEnergy;
		std::ostringstream queryStreamCore;
		std::ostringstream queryStream;
		std::ostringstream queryStreamMutationString;
		std::ostringstream queryStreamCount;
		queryStreamCore <<" from results where jobName = \"" <<getJobID()<<"\" and (foldx_energy - foldx_energy_wild_type) <= "<<getMaximumParentEnergy() << " and  not ISNULL("<<orderField<< ") and numMutations < "<<getMaxMutations  ();
		queryStreamCount<<"select count(*)"<<queryStreamCore.str()<<" ; ";
		//queryStreamCount<<std::string("select count(*) from results where jobName = \"") <<getJobID()<<"\" and (foldx_energy - foldx_energy_wild_type) <= "<<getMaximumParentEnergy() << " and  not ISNULL("<<orderField<< ") and maxMutants < "<<getMaxMutations  ()<<" ;";
 		int numResults = (queryValueAsInt(queryStreamCount.str()));               
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Total available parents = "<<numResults<<std::endl;
		//int nextIndex = index;
		if (numResults <= index) {
			std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Resetting index from : "<<index<<std::endl;
			index = 0; // if we've run out of possible parents, loop back to the beginning
			std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" to : "<<index<<std::endl;
			//nextIndex = index;
		} //else nextIndex = index+1;
		queryStream<<std::string("select sequence ")<<queryStreamCore.str()<<" order by "<<orderField<<" "<<order<<" limit "<<index<<" , 1 ; ";
		queryStreamMutationString<<std::string("select mutationString ")<<queryStreamCore.str()<<" order by "<<orderField<<" "<<order<<" limit "<<index<<" , 1 ; ";
		retrievedSequence = queryValueAsString (queryStream.str());
		std::string retrievedMutationString = queryValueAsString(queryStreamMutationString.str());
		std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Selected parent has mutationString = >"<<retrievedMutationString<<"< "<<std::endl;
		//std::string retrievedSequenceFoldxEnergyQuery = std::string("  select (foldx_energy - foldx_energy_wild_type) from results where jobName = \"")+ getJobID()  + std::string("\" and sequence = \"") + retrievedSequence + std::string("\" ; ") ;
		//retrievedSequenceEnergy = (queryValueAsDouble(retrievedSequenceFoldxEnergyQuery));
                std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Having found an acceptable sequence : "<<retrievedSequence<<", we are moving on .."<<std::endl;
		return retrievedSequence;
        };


//};
