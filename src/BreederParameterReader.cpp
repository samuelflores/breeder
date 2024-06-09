#include "BreederParameterReader.h"
#include <string>
#include  <iostream>
#include <unordered_set>


void BreederParameterReader::printUsage() {
   std::cout<<std::endl;
   std::cout << "Usage: ./breeder [options] " << std::endl;
   std::cout << "Options: " << std::endl;
   std::cout << " -help                         Display this information " << std::endl;
   std::cout << " -ID                           <Run ID>    This is the overarching job ID, which can span several values of -PDBID. -ID corresponds to the column 'runID' in table 'jobInfo' and 'jobName' in table 'results'. " << std::endl;
   std::cout << " -PDBID                        <PDB ID>    This is the four-character PDB ID. It is now required by both breeder and homologyScanner. If you are using a file of your own creation, make up a suitable four-character ID.  "<<std::endl; //However be sure to set  -WTRENUMBEREDSTRUCTUREFILENAME so breeder knows where to get the coordinates.  "<<std::endl;
   std::cout << " -PDBCHAINS                    <PDB ID, chains in complexes 1 & 2>    PDB ID and chains, in SKEMPI format. It looks like XXXX_AA_BB where XXXX is the PDB ID (4 characters), AA are the chains in complex 1, and BB are the chains in complex 2 (one character per chain, one or more chains per complex). This is equivalent to the -PDBID and -CHAINSINCOMPLEX parameters, except together and in a different format.   "<<std::endl;
   std::cout << " -SEQUENCE                     < no arguments > If invoked (requires no following arguments, just -SEQUENCE), we will only populate the sequence table in the database, and not perform any calculations." << std::endl;
   std::cout << " -L                            <list file name>  Read sequences from the provided file (CR delimited). Deprecated! Not guaranteed to work." << std::endl;
   std::cout << " -MS                           <list file name>  Read residue numbers (including any insertion codes) from the provided file (CR delimited).  A MultiScanning run will be executed, in which each residue position is mutated to each of the 19 possible alternatives." << std::endl;
   std::cout << "                                   Format: C-NNNI where e.g. C are chain ID's, NNN are residue numbers, I are insertion codes (if any). " << std::endl;
   std::cout << " -ONEMUTANT                    <mutation string> This tells breeder to compute one mutant. For the mutation string, follow the format: C-NNNI-S.c-nnni-s, where e.g. C,c are chain ID's, NNN and nnn are residue numbers, I and i are insertion codes (if any), and S and s are the mutant residue type in single letter code.  These can be concatenated with '.', up to the limitation of maxMutants. " << std::endl;
   std::cout << "                               You can slo use the SKEMPI format. It looks like CWNNNIS, where C = chain ID, W = wild type residue type in single letter code, NNN =  the residue number (1 or more digits), I =  the insertion code (0 or 1 character) and S = the substituted residue type in single letter code. breeder will infer which format you are using. " << std::endl;
   ///std::cout << " -SKEMPIMUTATION               <mutation string>   In SKEMPI format. It looks like CWNNNIS, where C = chain ID, W = wild type residue type in single letter code, NNN =  the residue number (1 or more digits), I =  the insertion code (0 or 1 character) and S = the substituted residue type in single letter code. " <<std::endl;
   std::cout << " -MLF                          <mutation list file name>  Read mutations from the provided file (CR delimited)" << std::endl;   std::cout << "                                   Format: C-NNNI-S.c-nnni-s as explained for -ONEMUTANT. " << std::endl;
   std::cout << " -MSI                          <chains to be mutated> <partner chains> <radius (nm)>  <list file name> ";   
   std::cout<<  " Do multiscanning on all residues that are on <chains to be mutated> and within <radius> of <partner chains>. Both <chains to be mutated> and <partner chains> are each to be given as concatenated strings of chain ID's. Lastly, <list file name> is the full path and file name for where you want the interface residues to be written (whatever was in this file will be overwritten).  For example, if you want to affinity mature an antibody (chains A,B) against an antigen (chains C,D), defining the interface as all residues in A,B and within 0.6 nm of C or D, and write the interface residues to /path/myInterfaceResiduesFile.txt,  you would issue: -MSI AB CD 0.6 /path/myInterfaceResiduesFile.txt  " << std::endl;
   std::cout << " -ACCOUNT                      <supercomputer allocation account number> e.g. snic-2013-1-302"<<std::endl;
   std::cout << " -USER                         <user name> often left blank"<<std::endl;   
   std::cout << " -BREEDERMAINDIRECTORY         <directory>  Specifies the main breeder directory, e.g. ~/svn/breeder. " << std::endl;
   std::cout << " -SINGLEMUTANTFILESDIRECTORY   <directory>  Specifies where standard files such as parameters.csv and rotabase.txt  are to be found. Defaults to <BREEDERMAINDIRECTORY/singleMutantFiles ." << std::endl; 
   std::cout << " -DATABASE                     <database file with full path>  If using sqlite3, the fully specified database file name containing the breeder database" << std::endl;
   std::cout << " -SQLSYSTEM                    <SQLite | MySQL> Database management system.  Only the two named are supported at this time."<<std::endl;
   std::cout << " -SQLEXECUTABLE                <database executable with full path>  If using sqlite3, this would be e.g. /usr/bin/sqlite3" << std::endl;
   std::cout << " -SQLSERVER                    <mysql server> e.g. rembrandt.bmc.uu.se .  Applies only for -SQLSYSTEM MySQL. " << std::endl;
   std::cout << " -SQLUSER                      <mysql user> " << std::endl;
   std::cout << " -SQLPASSWORD                  <mysql password> " << std::endl;
   std::cout << " -EMAILADDRESS                 <submitter email address> Validly formatted email address, for progress notifications." << std::endl;
   std::cout << " -MMBEXECUTABLE                <MMB executable with full path>  The MMB executable to be called " << std::endl;
   std::cout << " -FOLDXSCRIPT                  <script that runs foldx, with full path>  This is the location of the foldx script, e.g. ~/svn/breeder/scripts/run-foldx.pl " 
<< std::endl;
   std::cout << " -FOLDXEXECUTABLE              <foldx executable, with full path>  This is the location of the foldx executable, e.g. ~/local/foldx64  " << std::endl;
   std::cout << " -MOBILIZERRADIUS              <radius (nm)>  There is a \"applyMobilizerWithin <radius> \" command, this sets the radius for that. Defaults to 0.  " << std::endl;
   std::cout << " -WATERRADIUS                  <radius (nm)>  There is a \"waterDropletAboutResidue\" command for each substitution, this sets the radius for that. If 0 (which is the default), the command is not invoked.  " << std::endl;
   std::cout << " -MAXTIME                      <hours>  This is the maximum time that an individual job will be allowed to run before being killed. Default = 48 h."<<std::endl;         
   std::cout << " -JOBLIBRARYPATH               <path>  Using this parameter adds a line to the beginning of the batch file that looks like: export LD_LIBRARY_PATH=<path> . Otherwise (at least on Uppmax) the LD_LIBRARY_PATH will be propagated from the login shell.  " << std::endl;
   std::cout << " -GROMACSSIMULATIONTIME        <ns> Simulation time for GROMACS MD simulation.  If this is set, a GROMACS simulation will be run. This assumes a ZEMu calculation has been performed successfully.   " << std::endl;
   std::cout << " -WORKINGDIRECTORY             <path>  This sets the batch_directory column in the jobInfo table for the given runID.  In this directory, subdirectories will appear named after each mutation, and each of these subdirectories will contain the files generated by breeder, foldx, etc., for that mutation. " << std::endl;
   std::cout << " -PARTITION                    <partition name>  The partition name to be used for SLURM. This sets the \"partition\" column of the jobInfo table. " << std::endl;
   std::cout << " -CHAINSINCOMPLEX              <chain IDs of complex 1, chain IDs of complex 2>  The chain IDs of complex 1, then a comma (\",\"), and then the chains of complex 2 with no spaces. For example, if A and B are in one subunit, interacting with C, you specify \"AB,C\". " << std::endl;
   std::cout << " -WTRENUMBEREDSTRUCTUREFILENAME          <PDB structure file full path and file name> Location of the wild type structure file. Optional. If you use it, do so with care." << std::endl;
   std::cout << " -REPORTINGINTERVAL            <reporting interval (ps)> Set the length of each reporting interval (in ps) in the MMB equilibration. Total simulation time = reportingInterval * numReportingIntervals." << std::endl;
   std::cout << " -NUMREPORTINGINTERVALS        <number of reporting intervals> Number of reporting intervals in the MMB equilibration. Expects an integer." << std::endl;
   std::cout << " -FLEXIBILITYWINDOWOFFSET      <residues> Number of residues in flexibility zone, to each side of each mutated residue. Defaults to 2, for a total of 5 residues in flexibility zone, in the case of a single substitution."<< std::endl;
   std::cout << " -RAMP                         <0 | 1> Setting this flag to 1 ramps the reportingInterval from .0000001, .00001, .001, .1, (for stages 2,3,4,5) and then up to -REPORTINGINTERVAL (minus .1010101). It also sets jobInfo.lastStage  to 6. This greatly reduces convergence time for systems with initial steric clashes." <<std::endl ;
   std::cout << " -LASTSTAGE                    <lastStage> This is MMB's lastStage parameter. Defaults to 2. If you set to 1, this will force a REAL foldx-only run, meaning MMB will die and the foldx RepairPDB + BuildModel output PDB structure will be used for the foldx AnalyzeComplex calculation." <<std::endl <<std::endl;
   std::cout << "  " << std::endl;
   std::cout << " homologyScanner specific parameters. These are not required by breeder: " << std::endl;
   std::cout << " -BREEDEREXECUTABLE            <breeder executable with full path> Breeder executable. " << std::endl;
   std::cout << " -FASTAEXECUTABLE              <fasta_lwp.pl with full path> This executable queries NCBI to find PDB IDs based on significance of sequence alignment. Used by homologyScanner, not required by breeder." << std::endl;
   std::cout << " -FASTATEMPDIRECTORY           <directory> fasta_lwp.pl needs a place to write the results of its search. Within this directory, a subdirectory will appear named with the PDB ID of the PrimaryJob.  Under that directory, there will be one further subdirectory for each chain in the provided complex.  Within each of those chain directories there will be files containing information about homologs for that chain. It is best to leave the fasta-pg.ids.txt file so that the search can be skipped on future runs, but the other files can probably be deleted without much consequence." << std::endl;
   std::cout << " -TEMPERATURE                  <temperature (K)> Set the temperature for computing DDG from the affinities." << std::endl;
   std::cout << " -AFFINITYMUTANTANDWILDTYPE    <mutant affinity (Kd, in mol/L)> <wild type affinity (Kd, in mol/L)> Provide the affinities of wild type and mutant, from which breeder will compute experimentalDDG. Requires -TEMPERATURE .." << std::endl;
   std::cout << " -SINGLEHOMOLOGYSCANNERRUN    <mutant affinity (Kd, in mol/L)> <wild type affinity (Kd, in mol/L)> Provide the affinities of wild type and mutant, from which breeder will compute experimentalDDG. Requires -TEMPERATURE .." << std::endl;
   std::cout << " -CHAINSINHOMOLOGCOMPLEX      <(chain IDs of complex 1, comma, chain IDs of complex 2 -- all one string>  This is only for homologyScanner single-job runs (set -SINGLEHOMOLOGYSCANNERRUN). The chain IDs of complex 1, then a comma (\",\"), and then the chains of complex 2 with no spaces. For example, if A and B are in one subunit, interacting with C, you specify \"AB,C\". " << std::endl;
   std::cout << " -HOMOLOGPDBID                <PDB ID of homolog job>  This is only for homologyScanner single-job runs (set -SINGLEHOMOLOGYSCANNERRUN). Give the PDB ID of the child (not parent). " << std::endl;
}


int  BreederParameterReader::runBreeder(){
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
	// This alone causes a problem:
        breed.loadSequenceTable(*dbConnection); // this loads the "sequence" table with chain IDs, residue numbers, and amino acid types.
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        if (dbConnection->getSequenceOnly()){
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have requested to set the amino acid sequence only. Skipping the rest of the run. "<<std::endl;
            return 0;
        }
        // I tried WAL model, which would Allow multiple simultaneous readers for this database.  However I lost all entries to the results database for jobName PRL-hi-aff-site-vs-PRLR.3N06 , and suspect this was the cause. Therefore, switching back to delete mode.
       	if (listFileName.compare("XXXX") != 0) { // if the user specified a listFileName.. proceed to read sequences from list:
		std::cout<<__FILE__<<":"<<__LINE__<<" Detected that you want to read sequences from a file, "<<listFileName<<".  Proceeding to read this .."<<std::endl;
		breed.spawnFromList(*dbConnection, listFileName.c_str(),listTypeEnum );
                std::cout<<__FILE__<<":"<<__LINE__<<endl;
                if (oneMutationString.compare("") != 0){ std::cout<<__FILE__<<":"<<__LINE__<<" You have specified both -MLF "<<listFileName<<" and -ONEMUTANT "<< oneMutationString  <<". This is not allowed!"<<std::endl; exit(1);}
	} else if (oneMutationString.compare("") != 0){ // if the user specified a oneMutationString, meaning they want to run one calculation only
                std::cout<<__FILE__<<":"<<__LINE__<< " You have specified that you want to run the mutant "<<oneMutationString<<"."<<std::endl;
                int numSpawn = 0;
                breed.spawnOneMutant(*dbConnection, numSpawn, oneMutationString);
	        /*	
                if (numSpawn == 1) {std::cout<<__FILE__<<":"<<__LINE__<<" spawnOneMutant successfully spawned "<<numSpawn<<" mutant."<<std::endl; }
                else if (numSpawn == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" spawnOneMutant spawned "<<numSpawn<<" mutants. See messages preceding for an explanation."<<std::endl; }
                else {std::cout<<__FILE__<<":"<<__LINE__<<" spawnOneMutant spawned "<<numSpawn<<" mutants. This is suspicious! Stopping execution.   "<<std::endl; exit(1);}
		*/
        }
	else for (int i = 0 ; i < dbConnection->getMaxGenerations() ; i++) {
		std::cout<<__FILE__<<":"<<__LINE__<< " At the moment there are "<<dbConnection->getNumJobs(std::string("running"))<<" running jobs"<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<< " At the moment there are "<<dbConnection->getNumJobs(std::string("submitted"))<<" submitted jobs"<<std::endl;
		// This loop is somewhat superfluous with the 1.0 factor .
		   		while ((dbConnection->getNumJobs(std::string("running")) + dbConnection->getNumJobs(std::string("submitted"))) > (1.0 * dbConnection->getGenerationSize()))  {
			std::cout<<__FILE__<<":"<<__LINE__<<" At the moment there are " << dbConnection->getNumJobs(std::string("running"))<<" jobs running, and " << dbConnection->getNumJobs(std::string("submitted"))  << " jobs submitted. Current specified generation size is : "<< dbConnection->getGenerationSize()<<". Currently waiting for more jobs to complete."<<std::endl;
		}
                std::cout<<__FILE__<<":"<<__LINE__<<" Starting generation # "<<i<<std::endl;
		breed.spawnNewGeneration(*dbConnection );
	}
	/*
		  */
        std::cout<<__FILE__<<":"<<__LINE__<<endl;
	dbConnection->close();
	//std::cout <<__FILE__<<":"<<__LINE__<< " num_args="<<num_args<<" args="<<args[0]<<std::endl;
	          delete dbConnection;
	return 0;
};

void BreederParameterReader::updateDatabase(){
  	if (sQLSystem == SQLite) {
		struct stat st;
		if(stat(database .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<database<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<database<<std::endl; exit(1);}
        	dbConnection = new SQLiteConnection( server ,       password ,  user ,  database );
        	dbConnection->insertUpdateDelete(" PRAGMA journal_mode=delete;"); 
        } 

        //#ifdef ENABLE_MYSQL
        else if (sQLSystem == MySQL) {
	    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
            dbConnection = new MysqlConnection( server ,       password ,  user ,  database );
	    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        }
        //#endif
        else {
                std::cout<<__FILE__<<":"<<__LINE__<<" -SQLSYSTEM was set to an unrecognized value, or one which is not enabled in your current compilation."<<std::endl;
        }
        std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl; 
        dbConnection->setWorkingDirectory(workingDirectory); // This will fully validate workingDirectory and create it if needed. Note that it no longer touches the jobInfo table, since that may conflict with other breeder jobs running on the same jobInfo table but different pdbId.
	std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
	if (emailAddress.length() > 0) {
            dbConnection->emailAddress = emailAddress;}
	if (pdbId.length() > 0) {
            dbConnection->setPdbId(pdbId);}
        dbConnection->setSequenceOnly(sequence);
        if (dbConnection->getSequenceOnly()){
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have requested to set the amino acid sequence only. Skipping the rest of the database work. "<<std::endl;
            return;
        }  
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
	dbConnection->setJobID(jobId);
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        dbConnection->setAccountName(account);
        dbConnection->setMobilizerWithinRadius(mobilizerWithinRadius);
        dbConnection->setWaterRadius(waterRadius);
        dbConnection->setMaxTime(maxTime);
        dbConnection->setMMBExecutable(MMBExecutable);
        //dbConnection->setEmailAddress (myEmailAddress);
        dbConnection->setFoldXScript(foldXScript);
        dbConnection->setSQLExecutable(SQLExecutable);
        dbConnection->setFoldXExecutable(foldXExecutable);
        dbConnection->setGromacsSimulationTime (gromacsSimulationTime);
        dbConnection->setTemperatureInK (temperature);
        dbConnection->setExperimentalDDG (mutantAffinityInM, wildTypeAffinityInM, temperature);
        dbConnection->setUserProvidedMutationString (oneMutationString);
      
        if (pdbChains.length() > 0) { // --PDBCHAINS , pdbChains are not required. 
            std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            dbConnection->setPdbAndChainsSkempi (pdbChains); 
            pdbId = dbConnection->getPdbId(); // The validation method does not have dbConnection
        }
        if (reportingInterval == -1111.0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not set -REPORTINGINTERVAL . Checking jobInfo.reportingInterval to see if the current value is acceptable.."<<std::endl; dbConnection->getReportingInterval();
        } else dbConnection->setReportingInterval(reportingInterval);
        if (numReportingIntervals == -1111) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not set -NUMREPORTINGINTERVALS . Checking jobInfo.numReportingInterval to see if the current value is acceptable.."<<std::endl; dbConnection->getNumReportingIntervals();
        } else dbConnection->setNumReportingIntervals(numReportingIntervals);
        dbConnection->setFlexibilityWindowOffset(flexibilityWindowOffset); 
	if (lastStage) {
	     dbConnection->setLastStage (lastStage);
	}
        if (ramp) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have invoked the -RAMP feature "<<std::endl; 
            std::string rampString = " \n readAtStage 2 \n reportingInterval .0000001 \n readBlockEnd \n readAtStage 3 \n reportingInterval .00001 \n readBlockEnd \n readAtStage 4 \n reportingInterval .001 \n readBlockEnd \n readAtStage 5  \n reportingInterval .1 \n   readBlockEnd \n";
            std::string myQueryString = "update jobInfo set ssBondCorrection = \"" + rampString + "\" where runID = \"" + dbConnection->getJobID() + "\" ; "; 
            std::cout<<__FILE__<<":"<<__LINE__<<" About to issue query : "<<std::endl <<myQueryString<<std::endl; 
            dbConnection->insertUpdateDelete (myQueryString);   
            std::cout<<__FILE__<<":"<<__LINE__<<" Done with query : "<<std::endl; 
            if (lastStage) {std::cout <<__FILE__<<":"<<__LINE__<<" You tried to set -LASTSTAGE AND -RAMP. This is not allowed!"<<std::endl; exit(1);}
            dbConnection->setLastStage (6); // Consistent with the ramp going through stages 2,3,4,5, and then 6 for final.  
            dbConnection->setReportingInterval(dbConnection->getReportingInterval() - 0.1010101);
        }
        /*  
        if (workingDirectory.length() > 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have specified jobInfo.batch_directory = >"<<workingDirectory <<"< "<<std::endl;
            struct stat st;
            cout << "Checking status of directory "<<workingDirectory<<" : "<< stat(workingDirectory.c_str(),&st) << endl;
            bool isdir = S_ISDIR(st.st_mode);
            if (!(isdir)) 
                {std::cout<<__FILE__<<":"<<__LINE__<<" The directory "<<workingDirectory<<" does not appear to exist. Creating it now."<<std::endl; //exit(1);
                int status;
                status = mkdir(workingDirectory.c_str(),S_IRWXU);
                std::cout<<__FILE__<<":"<<__LINE__<<" Returned status : "<<status<<std::endl;
            } 
            else 
                {std::cout<<__FILE__<<":"<<__LINE__<<" The directory "<<workingDirectory<<" appears to exist."<<std::endl;}
            if (st.st_mode & S_IWUSR) {
                std::cout<<__FILE__<<":"<<__LINE__<<" Owner has write permissions on "<<workingDirectory<<"."<<std::endl; }
            else {std::cout<<__FILE__<<":"<<__LINE__<<" Owner does NOT have write permissions on "<<workingDirectory<<"."<<std::endl;exit(1);}
            dbConnection->setWorkingDirectory(workingDirectory);} // If workingDirectory is not "", then do not touch jobInfo.batch_directory .  the user may have set it on their own in SQL.  Actually this should be the case for all these variables, but I have not gotten around to SQLizing them all.
        else {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not specified -WORKINGDIRECTORY, apparently.  So we will leave jobInfo.batch_directory at its current value, which is >"<<dbConnection->getWorkingDirectory()<<"< "<<std::endl;} 
        */

        if (wtRenumberedStructureFileName.compare("") != 0) { // If -WTRENUMBEREDSTRUCTUREFILENAME is set, then update jobInfo table:
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameters -WTRENUMBEREDSTRUCTUREFILENAME has been set to "<<wtRenumberedStructureFileName<<std::endl; 
            //std::cout<<__FILE__<<":"<<__LINE__<<" The parameters -WTRENUMBEREDSTRUCTUREFILENAME  is disabled. Please put your preferred file name in the alternatePdbStructures table.  Or in the WORKINGDIRECTORY (just above the single mutant directory)."<<std::endl;
            //exit(1);
            
            dbConnection->insertIntoAlternatePdbStructuresIfAbsent("alternateRenumberedStructureFile",wtRenumberedStructureFileName);
            //dbConnection->setWildTypeStructureFileName(wtRenumberedStructureFileName);
            std::cout<<__FILE__<<":"<<__LINE__<<" Confirming.. dbConnection->getRenumberedWildTypeStructureFileName() returns : "<<dbConnection->getRenumberedWildTypeStructureFileName()<<std::endl; 
        } else { // User has not set -WTRENUMBEREDSTRUCTUREFILENAME . But it is possible this field has already been set. So check. 
            std::cout<<__FILE__<<":"<<__LINE__<<" No longer allowed toNo longer allowed to   set -WTRENUMBEREDSTRUCTUREFILENAME on the command line. Checking .. Getting : >"<<dbConnection->getRenumberedWildTypeStructureFileName()<<"<."<<std::endl;
        }
        std::cout<<__FILE__<<":"<<__LINE__<<" Checking FoldXExecutable from SQL: "<<dbConnection->getFoldXExecutable()<<std::endl;
        if (jobLibraryPath.length() > 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have specified jobInfo.jobLibraryPath = >"<<jobLibraryPath<<"< "<<std::endl;
            dbConnection->setJobLibraryPath(jobLibraryPath);} // If jobLibraryPath is not "", then do not touch jobInfo.jobLibraryPath .  the user may have set it on their own in SQL.  Actually this should be the case for all these variables, but I have not gotten around to SQLizing them all.
        else {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not specified -JOBLIBRARYPATH , apparently.  So we will leave jobInfo.jobLibraryPath at its current value, which is >"<<dbConnection->getJobLibraryPath()<<"< "<<std::endl;} 
        if (partition       .length() > 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have specified jobInfo.partition       = >"<<partition     <<"< "<<std::endl;
            dbConnection->setPartition(partition);} // If partition is not "", then do not touch jobInfo.partition .  the user may have set it on their own in SQL.  Actually this should be the case for all these variables, but I have not gotten around to SQLizing them all.
        else {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not specified -PARTITION, apparently.  So we will leave jobInfo.partition       at its current value, which is >"<<dbConnection->getPartition       ()<<"< "<<std::endl;
            std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        } 
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        if (chainsInMutatedSubunit.length() > 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
            std::cout<<__FILE__<<":"<<__LINE__<<" You have specified jobInfo.chainsInMutatedSubunit       = >"<<chainsInMutatedSubunit     <<"< "<<std::endl;
            dbConnection->setChainsInMutatedSubunit(chainsInMutatedSubunit);} // If chainsInMutatedSubunit is not "", then do not touch jobInfo.chainsInMutatedSubunit .  the user may have set it on their own in SQL.  Actually this should be the case for all these variables, but I have not gotten around to SQLizing them all.
        else {
            std::cout<<__FILE__<<":"<<__LINE__<<" You have not specified -CHAINSINCOMPLEX, apparently. "<<std::endl; 
            std::cout<<__FILE__<<":"<<__LINE__<<" So we will attempt to use the current jobInfo.chainsInMutatedSubunit"<<std::endl;
            std::cout<<__FILE__<<":"<<__LINE__<<" The current value of jobInfo.chainsInMutatedSubunit is >"<<dbConnection->getChainsInMutatedSubunit()<<"< "<<std::endl;
            if (dbConnection->getChainsInMutatedSubunit().compare("") == 0) {
                std::cout<<__FILE__<<":"<<__LINE__<<" jobInfo.chainsInMutatedSubunit is empty or null. This is not acceptable. Please specify -CHAINSINCOMPLEX or edit jobInfo.chainsInMutatedSubunit."<<std::endl; exit(1); 
            }
            std::cout<<__FILE__<<":"<<__LINE__<<" Successfully retrieved jobInfo.chainsInMutatedSubunit"<<std::endl;
            } 
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
	dbConnection->setSingleMutantFilesDirectory(singleMutantFilesDirectory);
	dbConnection->setBreederMainDirectory(breederMainDirectory);
       
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        std::cout<<__FILE__<<":"<<__LINE__<<"msiScannedChains >"<<msiScannedChains<<"<"<<std::endl;
        if (msiScannedChains.compare("") != 0)
	    dbConnection->setMsiScannedChains(msiScannedChains);              
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        if (msiPartnerChains.compare("") != 0)
	    dbConnection->setMsiPartnerChains(msiPartnerChains);              
        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
        if (msiRadius > 0) 
	    dbConnection->setMsiRadius(msiRadius);              
	std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
	if (dbConnection->getMsiRadius() > 0.0) { 	 
            prepareMultiScanningInterfaceFile();
        } 
    } // of updateDatabase



void BreederParameterReader::prepareMultiScanningInterfaceFile (){
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		BiopolymerClassContainer wildTypeBiopolymerClassContainerForInterfaceScanning;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		CompoundSystem          system; 
                SimbodyMatterSubsystem matter;
		vector<Displacement> displacementVector; 
		displacementVector.clear();    
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		wildTypeBiopolymerClassContainerForInterfaceScanning.loadSequencesFromPdb( dbConnection->getRenumberedWildTypeStructureFileName(),bool(false),"",true,true); 
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                wildTypeBiopolymerClassContainerForInterfaceScanning.initializeBiopolymers (system,false,true,false,false,false,false,false,0.0,displacementVector, .1500, .07);
                wildTypeBiopolymerClassContainerForInterfaceScanning.initializeAtomInfoVectors(matter);
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                vector<String> referenceChains; referenceChains.clear();
                vector<String> partnerChains;   partnerChains.clear();
                for (int i = 0; i < dbConnection->getMsiScannedChains().length(); i++) {
                    referenceChains.push_back((dbConnection->getMsiScannedChains()).substr(i,1));
                }
                for (int i = 0; i < dbConnection->getMsiPartnerChains().length(); i++) {
                    partnerChains.push_back((dbConnection->getMsiPartnerChains()).substr(i,1));
                }
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                ResidueStretch tempResidueStretch; // just making sure it's ok instantiating a ResidueStretch
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                ResidueStretchContainer <ResidueStretch>   myResidueStretchContainer; // To instantiate a template class, I understand it must be defined in same compilation unit.  So, I include ResidueStretchContainer.h
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                ///myResidueStretchContainer = new (ResidueStretchContainer <ResidueStretch> ());
                                  myResidueStretchContainer.addAllMutualChainResidues(dbConnection->getMsiRadius(),referenceChains, partnerChains, wildTypeBiopolymerClassContainerForInterfaceScanning);  
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                int j,k;
                for (int i = 0; i < myResidueStretchContainer. getNumResidueStretches() ; i++) {
                    myResidueStretchContainer.getResidueStretch(i).printStretch();
                    //myResidueStretchContainer.residueStretchVectorPrintStretch(i);
                    if (myResidueStretchContainer.getResidueStretch(i).getStartResidue() != myResidueStretchContainer.getResidueStretch(i).getEndResidue()) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Actually we are expecting that the residue stretch start and end residues are the same residue.  That did not happen here."<<std::endl;
                        myResidueStretchContainer.getResidueStretch(i).printStretch(); exit(1);
                        //myResidueStretchContainer.residueStretchVectorPrintStretch(i); exit(1);
                        
                    }
                    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                    for ( j = 0; j < referenceChains.size(); j++) {
			    if (myResidueStretchContainer.getResidueStretch(i).getChain().compare(referenceChains[j] ) ==0) {
			    //if (myResidueStretchContainer.residueStretchVectorGetChain(i).compare(referenceChains[j] ) ==0) {
				std::cout<<__FILE__<<":"<<__LINE__<<" This residue (stretch) is in chain "<<referenceChains[j]<< ", which is a reference chain, therefore we are keeping it."<<std::endl; 
                                break;}  // remember, break terminates the loop. continue only goes on to the next iteration early.
                    } 
                    std::cout<<__FILE__<<":"<<__LINE__<<" j = "<<j<<std::endl;
                    for ( k = 0; k < partnerChains.size(); k++) {
                            std::cout<<__FILE__<<":"<<__LINE__<<"partnerChains[ k] = "<<partnerChains[ k]  <<std::endl;
			    if (myResidueStretchContainer.getResidueStretch(i).getChain().compare(partnerChains[k] ) ==0) {
				std::cout<<__FILE__<<":"<<__LINE__<<" This residue (stretch) is in chain "<<partnerChains[k]<< ", which is a partner chain, therefore we are deletingit."<<std::endl; 
                                std::cout<<__FILE__<<":"<<__LINE__<<" i = "<<i<<std::endl;
                                myResidueStretchContainer.deleteResidueStretch(i);//residueStretchVectorErase(i);
                                i--;
                                break;}
                    }
                    if ((j == referenceChains.size()) && (k == partnerChains.size())) {
                        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                        std::cout<<__FILE__<<":"<<__LINE__<<" Unexplained error! Apparently the chain "<<myResidueStretchContainer.getResidueStretch(i).getChain()<<" is neither a reference not a partner chain."<<std::endl; exit(1);
                    }
                } 
                myResidueStretchContainer.printResidueStretchVector();
                std::ofstream myofstream(listFileName.c_str());
                if (!(myofstream.good())) {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Unable to open "<<listFileName<<" for writing."<<std::endl; exit(1);
                }
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                myResidueStretchContainer.printResidueStretchVectorBreederFormat(myofstream);
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                myofstream.close();
}


int BreederParameterReader::validate () { // Limited validation.
        if (database .compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-DATABASE' is required!"<<std::endl;
            exit(1);
        }
        if (SQLExecutable .compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-SQLEXECUTABLE' is required!"<<std::endl;
            exit(1);
        }
        if (pdbChains.length() == 0) {
		if    (pdbId.length() == 4)   
		{
		    std::cout<<__FILE__<<":"<<__LINE__<<" You have specified a PDB ID "<<pdbId<<" of length "<<pdbId.length()<<" .  "<<std::endl;
		} else {
		    std::cout<<__FILE__<<":"<<__LINE__<<" You have specified a PDB ID "<<pdbId<<" of length "<<pdbId.length()<<" . PDB IDs must be of length 4. "<<std::endl;
		    exit(1);
		}
        }
        //dbConnection->setSequenceOnly(sequence);
        if (sequence){
            std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have requested to set the amino acid sequence only. Skipping the rest of the validation.    "<<std::endl;
            return 0;
        }
        if (jobId.compare("XXXX") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-ID' is required!"<<std::endl;
            exit(1);
        }
        if (singleMutantFilesDirectory.compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-SINGLEMUTANTFILESDIRECTORY' is required! You can also provide -BREEDERMAINDIRECTORY and have breeder infer this parameter to be <BREEDERMAINDIRECTORY>/singleMutantFiles"<<std::endl;
            exit(1);
        }
        if (foldXScript .compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-FOLDXSCRIPT' is required!"<<std::endl;
            exit(1);
        }
        if (foldXExecutable .compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-FOLDXEXECUTABLE' is required!"<<std::endl;
            exit(1);
        }
        if (MMBExecutable .compare("") == 0) {
            std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-MMBEXECUTABLE' is required!"<<std::endl;
            exit(1);
        }
        } // of validate


int BreederParameterReader::validateForHomologyScanner () { // Limited validation for homologyScanner only.  homologyScanner still needs to call the regular validate() because it uses breeder.
        
        if     (pdbId.length() != 4)

         {
             std::cout<<__FILE__<<":"<<__LINE__<<" You have specified a PDB ID "<<pdbId<<" of length "<<pdbId.length()<<" . This is not acceptable. "<<std::endl;
             exit(1);
         } else {
             std::cout<<__FILE__<<":"<<__LINE__<<" You have specified a PDB ID "<<pdbId<<" of length "<<pdbId.length()<<" . PDB ID is accepted. "<<std::endl;

         }
         
         //dbConnection->getPdbId();
         if (fastaExecutable.compare("") == 0) {
             std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-FASTAEXECUTABLE' is required! "<<std::endl;

             exit(1);
         }

         if (breederExecutable.compare("") == 0) {
             std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-BREEDEREXECUTABLE' is required! "<<std::endl;

             exit(1);
         }
         if (fastaTempDirectory.compare("") == 0) {
             std::cout<<__FILE__<<":"<<__LINE__<<" The parameter '-FASTATEMPDIRECTORY' is required! "<<std::endl;

             exit(1);
         }
}

 
        void BreederParameterReader::readParameters (int myNumArguments, char * myArguments[]){
            std::string arg = "";
            std::string option = "" ;
	    for( int i=1;i<myNumArguments;i++) {

	       arg = std::string( myArguments[i]);
               std::cout<<__FILE__<<":"<<__LINE__<<" Processing myArguments["<<i<<"] = "<<myArguments[i]<<std::endl;
               std::cout<<__FILE__<<":"<<__LINE__<<" myNumArguments = "<<myNumArguments <<std::endl;
	       option.resize(arg.length());
	       for(int j=0;j<(int)arg.length();j++) {
		   option[j] = toupper(arg[j]);
	       }
	       if( (option == "-HELP" )  || (option == "-H") ) {
		    printUsage();
	       } else if ( option == "-ID")   {
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of job ID        "  << std::endl; exit(1);
		  	 exit(1);
		    } else  {
			 jobId         = myArguments[++i];
		    }
	       } else if ( option == "-PDBID")   {
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of PDB ID        "  << std::endl; exit(1);
		  	 exit(1);
		    } else  {
			 pdbId         = myArguments[++i];
		    }
               } else if ( option == "-PDBCHAINS"    ){
                   if( myNumArguments < i+2 ) {
                       std::cout << "Error: missing -PDBCHAINS value   "<< std::endl;
                       exit(1);
                   } else  {
                       pdbChains  = (myArguments[++i]);
                   }
               } else if ( option == "-SEQUENCE"    ){
                   sequence = 1;                 

               } else if ( option == "-AFFINITYMUTANTANDWILDTYPE"    ){

                   if( myNumArguments < i+3 ) {
                         std::cout << "Error: -AFFINITYMUTANTANDWILDTYPE must be followed by two floating-point values.    "<< std::endl;
                         exit(1);
                   } else  {
                       mutantAffinityInM  = atof(myArguments[++i]);
                       wildTypeAffinityInM  = atof(myArguments[++i]);
                       //setExperimentalDDG( mutantAffinityInM , wildTypeAffinityInM);
                   }
               } else if ( option == "-SINGLEHOMOLOGYSCANNERRUN"    ){
                   singleHomologyScannerRun = 1;                 
               } else if ( option == "-CHAINSINHOMOLOGCOMPLEX"){
                   if( myNumArguments < i+2 ) {
                       std::cout << "Error: missing -CHAINSINHOMOLOGCOMPLEX value   "<< std::endl;
                       exit(1);
                   } else  {
                       chainsInHomologComplex  = (myArguments[++i]);
                   }
               } else if ( option == "-HOMOLOGPDBID"){
                   if( myNumArguments < i+2 ) {
                       std::cout << "Error: missing -HOMOLOGPDBID  value   "<< std::endl;
                       exit(1);
                   } else  {
                       homologPdbId = (myArguments[++i]);
                   }
               } else if ( option == "-TEMPERATURE"    ){
                   if( myNumArguments < i+2 ) {
                       std::cout << "Error: missing temperature value   "<< std::endl;
                       exit(1);
                   } else  {
                       temperature  = atof(myArguments[++i]);
                   }


	       } else if (( option == "-L") ||   
	                  ( option == "-MLF"))  {
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of list file     "  << std::endl; exit(1);
		  	 exit(1);
		    } else  {
                         if (listFileName.compare("XXXX") != 0) {
		             std::cout<<__FILE__<<":"<<__LINE__<<" Something is wrong. The listFileName has already been set to: "<<listFileName<<" . You can't set this twice!  If you issued the -MSI flag, you can't also issue -MS !"<<std::endl; exit(1);  // prevent people from issuing in both -MSI and -MS
		  	     exit(1);
                         }
			 listFileName        = myArguments[++i];
		    }
	            if (option == "-L")         listTypeEnum = sequencesString;
		    else if ( option == "-MLF") listTypeEnum = mutationString;
		    else {std::cout<<__FILE__<<":"<<__LINE__<<" Unsupported list file type! "<<std::endl; exit(1); } // just for sanity
               } else if ( option == "-ONEMUTANT"    ){  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing mutation string   "<< std::endl; 
			exit(1);
		    } else  {
			oneMutationString  = (myArguments[++i]);
                    } 

               } else if ( option == "-SKEMPIMUTATION"    ){
                   if( myNumArguments < i+2 ) {
                       std::cout << "Error: missing mutation string   "<< std::endl;
                       exit(1);
                   } else  {
                       std::string skempiMutationString;
                       skempiMutationString  = (myArguments[++i]);
                       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" This is no longer allowed. Just use -ONEMUTANT . breeder will infer whether you are using SKEMPI or breeder format. >"<<skempiMutationString <<"< "<<std::endl;
                       exit(1);
                       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Processing provided value: >"<<skempiMutationString<<"< "<<std::endl;
                       //if (skempiMutationString.substr(1,1){ // or MUTATIONSTRINGMINORSEPARATOR ?
                       std::unordered_set<std::string> aminoAcids {  "A", "R", "N", "D", "C", "X" , "E", "Q", "G", "H", "I", "L", "K", "M", "F", "P", "S", "T", "W", "Y", "V"};
                       if (aminoAcids.find(skempiMutationString.substr(1,1)) == aminoAcids.end() ) {
                           std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" The wild type residue type of  >"<<skempiMutationString.substr(1,1)<<" from the SKEMPI formatted mutation string >"<< skempiMutationString<<"< is not in the list of acceptable amino acid single letter codes. "<<std::endl; exit(1);
                       }
                       skempiMutationString.replace(1,1,"-"); // "-" or MUTATIONSTRINGMINORSEPARATOR ?
                       skempiMutationString.insert((skempiMutationString.length()-1),"-"); // "-" or MUTATIONSTRINGMINORSEPARATOR ?
                       std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" Transformed provided value to : >"<<skempiMutationString<<"< "<<std::endl;
                       oneMutationString  = skempiMutationString;
                     }


               } else if ( option == "-WTRENUMBEREDSTRUCTUREFILENAME"    ){  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing wild type structure file name. "<< std::endl; 
			exit(1);
		    } else  {
			wtRenumberedStructureFileName  = (myArguments[++i]);
                        std::cout<<__FILE__<<":"<<__LINE__<<" The parameter -WTRENUMBEREDSTRUCTUREFILENAME has been set with "<<wtRenumberedStructureFileName<<std::endl;
                        //std::cout<<__FILE__<<":"<<__LINE__<<" This parameter -WTRENUMBEREDSTRUCTUREFILENAME is no longer allowed. Set your preferred file location in alternatePdbStructures table. Or put in the default location just aboeve the single mutant directories."<<std::endl;; 
			//exit(1);
                    } 
	       } else if ( option == "-MSI")   {
                    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
	            listTypeEnum = residues;          
		    if (listFileName.compare("XXXX") != 0) {
		        std::cout<<__FILE__<<":"<<__LINE__<<" Something is wrong. The listFileName has already been set to: "<<listFileName<<" . You can't set this twice!  If you issued the -MSI flag, you can't also issue -MS !"<<std::endl; exit(1);  // prevent people from issuing in both -MSI and -MS
		    }
		    if( myNumArguments < i+5 ) {
			 std::cout << "Error not enough parameters. See syntax above.   "  << std::endl; exit(1);
		    } else  {
			 msiScannedChains = myArguments[++i];
                         if (msiScannedChains.length() < 1) {
                             std::cout <<__FILE__<<":"<<__LINE__<< "Error <chains to be mutated> must be a string of length >0. You have provided : >"<<msiScannedChains<<"<" <<std::endl; exit(1);}
                         std::cout<<__FILE__<<":"<<__LINE__<<" For -MSI, the parameter <chains to be mutated> has been set to : >"<<msiScannedChains<<"<"<<std::endl;
			 msiPartnerChains = myArguments[++i];
                         if (msiScannedChains.length() < 1) {std::cout << "Error <partner chains> must be a string of length >0."<<std::endl; exit(1);}
                         std::cout<<__FILE__<<":"<<__LINE__<<" For -MSI, the parameter <partner chains> has been set to : >"<<msiPartnerChains<<"<"<<std::endl;
			 msiRadius        = atof(myArguments[++i]);
                         if (msiRadius < 1.0e-14) {std::cout <<__FILE__<<":"<<__LINE__<< " Error <radius> must be > 0 nm. You have requested : "<<myArguments[++i]<<" ."<<std::endl; exit(1); }
                         std::cout<<__FILE__<<":"<<__LINE__<<" For -MSI, the parameter <radius> has been set to : >"<<msiRadius<<"<"<<std::endl;
                         listFileName = myArguments[++i];
                         std::cout<<__FILE__<<":"<<__LINE__<<" For -MSI, the parameter <list file name> has been set to : >"<<listFileName <<"< . The contents of this file will be overwritten by breeder's computed interface residue list. Please check this file and make sure it contains the residues you want to scan."<<std::endl;
		    }
               // maybe it would be better to set generation size in mysql..
               } else if ( option == "-SQLUSER"    ) {   //  e.g. rembrandt.bmc.uu.se            
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of SQLUSER"     << std::endl;
		  	 exit(1);
                    }
                    user    =      myArguments[++i];
	       } else if ( option == "-SQLPASSWORD") {   //  e.g. rembrandt.bmc.uu.se            
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of SQLPASSWORD" << std::endl;
		  	 exit(1);
                    }
                    password=      myArguments[++i];
	       } else if ( option == "-SQLSERVER") {   //  e.g. rembrandt.bmc.uu.se            
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of SQLSERVER     "  << std::endl;  exit(1);
		  	 exit(1);
                    }
                    server =      myArguments[++i];
	       } else if ( option == "-SQLSYSTEM") {   //  Set -SQLSYSTEM  to MySQL or SQLite
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of SQLSYSTEM  "  << std::endl;  exit(1);
		  	 exit(1);
                    }
                    std::string tempSQLSYSTEM = myArguments[++i];
                    if (tempSQLSYSTEM.compare("MySQL") == 0) { sQLSystem = MySQL;}
                    else if (tempSQLSYSTEM.compare("SQLite") == 0) { sQLSystem = SQLite;}
                    else {std::cout << " Unknown -SQLSYSTEM option given : "<<tempSQLSYSTEM <<std::endl; exit(1);}
	       } else if ( option == "-MS") {   // MultiScanning run 

                    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		    if (listFileName.compare("XXXX") != 0) {
		        std::cout<<__FILE__<<":"<<__LINE__<<" Something is wrong. The listFileName has already been set to: "<<listFileName<<" . You can't set this twice!  If you issued the -MSI flag, you can't also issue -MS !"<<std::endl; exit(1);  // prevent people from issuing in both -MSI and -MS
		  	 exit(1);
		    }
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of residue list file     "  << std::endl;
		    } else  {
			 listFileName        = myArguments[++i];
		    }
	            listTypeEnum = residues;          
	       } else if ( option == "-SERVER") {  
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of server     "  << std::endl;
		  	 exit(1);
		    } else  {
                         std::cout<<__FILE__<<":"<<__LINE__<<" Error! the -SERVER flag is obsolete. Perhaps you meant -SQLSERVER"<< std::endl;
			 server        = myArguments[++i];
		  	 exit(1);
		    }
	       } else if ( option == "-ACCOUNT" ) {  
		    if( myNumArguments < i+2 ) {
			 std::cout << "Error missing name of ACCOUNT      "  << std::endl;
		  	 exit(1);
		    } else  {
			 account         = myArguments[++i];
		    }
	       } else if ( option == "-USER") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of USER     "  << std::endl;
			exit(1);
		    } else  {
			 user            = myArguments[++i];
		    }
	       } else if ( option == "-DATABASE") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of DATABASE    "  << std::endl;
			exit(1);
		    } else  {
			database        = myArguments[++i];
		    }
	       } else if ( option == "-MMBEXECUTABLE") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of MMBExecutable"  << std::endl;
			exit(1);
		    } else  {
			 MMBExecutable   = myArguments[++i];
			struct stat st;
			if(stat(MMBExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<MMBExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<MMBExecutable<<std::endl; exit(1);}
		    }
	       } else if ( option == "-EMAILADDRESS") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of -EMAILADDRESS "  << std::endl;
			exit(1);
		    } else  {
			emailAddress   = myArguments[++i];
		    }
	       } else if ( option == "-FOLDXSCRIPT") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of FoldXScript"  << std::endl;
			exit(1);
		    } else  {
			foldXScript   = myArguments[++i];
			struct stat st;
			if(stat(foldXScript .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<foldXScript<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<foldXScript<<std::endl; exit(1);}
		    }
	       } else if ( option == "-SQLEXECUTABLE") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of SQL executable"  << std::endl;
			exit(1);
		    } else  {
			SQLExecutable   = myArguments[++i];
			struct stat st;
			if(stat(SQLExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<SQLExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<SQLExecutable<<std::endl; exit(1);}
		    }
	       } else if ( option == "-FOLDXEXECUTABLE") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of FoldX executable"  << std::endl;
			exit(1);
		    } else  {
			foldXExecutable   = myArguments[++i];
			struct stat st;
			if(stat(foldXExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<foldXExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<foldXExecutable<<std::endl; exit(1);}
                        
		    }
	       } else if ( option == "-BREEDERMAINDIRECTORY") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of -BREEDERMAINDIRECTORY"  << std::endl;
			exit(1);
		    } else  {
			breederMainDirectory          = myArguments[++i];
			struct stat st;
			if(stat(breederMainDirectory .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<breederMainDirectory<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<breederMainDirectory<<std::endl; exit(1);}
			singleMutantFilesDirectory    = breederMainDirectory+ "/singleMutantFiles";
                        std::cout<<__FILE__<<":"<<__LINE__<<" Set singleMutantFilesDirectory = >"<< singleMutantFilesDirectory<<"< ."<<std::endl;   
		    }
	       } else if ( option == "-SINGLEMUTANTFILESDIRECTORY") {  
                    //if (singleMutantFilesDirectory.compare("") != 0) {
                                          //    std::cout << "Error: -SINGLEMUTANTFILESDIRECTORY is already set to "<<singleMutantFilesDirectory<<" . If you wish to set this variable, set it before others which may also modify it. For example, -BREEDERMAINDIRECTORY also sets it. "<<std::endl;
                                                                    //    exit(1);
                                                                                              // }
                    if( myNumArguments < i+2 ) {
			std::cout << "Error missing name of SINGLEMUTANTFILESDIRECTORY"  << std::endl;
			exit(1);
		    } else  {
			singleMutantFilesDirectory    = myArguments[++i];
			struct stat st;
			if(stat(singleMutantFilesDirectory .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<singleMutantFilesDirectory<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<singleMutantFilesDirectory<<std::endl; exit(1);}
		    }
	       } else if ( option == "-MAXTIME") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing MAXTIME parameter"  << std::endl;
			exit(1);
		    } else  {
                        //std::cout << __FILE__<<":"<< __LINE__<< " >"<<myArguments[++i]<<"<   " <<std::endl;
                                                  //std::cout << __FILE__<<":"<< __LINE__<< " "<<atof(myArguments[++i])<<std::endl;
                                                                            std::cout<<__FILE__<<":"<<__LINE__<<" Processing myArguments["<<i<<"] = "<<myArguments[i]<<std::endl;
			maxTime    = atof(myArguments[++i]);
                        std::cout<<__FILE__<<":"<<__LINE__<<" Processing myArguments["<<i<<"] = "<<myArguments[i]<<std::endl;
			//maxTime    = atof(myArguments[++i]);
			  			if(maxTime <= -1e-14) { std::cout<<__FILE__<<":"<<__LINE__<<" MAXTIME must be > 0, you have specified : "<<maxTime<<std::endl;exit(1);} 
		    }
	       } else if ( option == "-MOBILIZERRADIUS") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing radius"  << std::endl;
			exit(1);
		    } else  {
			mobilizerWithinRadius    = atof(myArguments[++i]);
			if(mobilizerWithinRadius <= -1e-14) { std::cout<<__FILE__<<":"<<__LINE__<<" MOBILIZERRADIUS must be > 0, you have specified : "<<mobilizerWithinRadius<<" if you meant to leave  this value at zero, just leave it blank."<<std::endl;exit(1);} 
		    }
	       } else if ( option == "-WATERRADIUS") {  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing radius"  << std::endl;
			exit(1);
		    } else  {
			waterRadius    = atof(myArguments[++i]);
			if(waterRadius <= -1e-14) { std::cout<<__FILE__<<":"<<__LINE__<<" WATERRADIUS must be > 0, you have specified : "<<waterRadius<<" if you meant to leave  this value at zero, just leave it blank."<<std::endl;exit(1);} 
		    }
	       } 
               else if ( option == "-JOBLIBRARYPATH"){  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing job library path"  << std::endl; 
			exit(1);
		    } else  {
			jobLibraryPath    = (myArguments[++i]);
                        //dbConnection.setJobLibraryPath(jobLibraryPath);
                                              } 
	       } 
               else if ( option == "-GROMACSSIMULATIONTIME"){  
                    std::cout << __FILE__<<":"<< __LINE__<<std::endl;
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing gromacs simulation time"  << std::endl; 
			exit(1);
		    } else  {
                        std::cout << __FILE__<<":"<< __LINE__<< " myArguments ["<<i<<"] = >"<<myArguments[i  ]<<"<   " <<std::endl;
                        std::cout << __FILE__<<":"<< __LINE__<< " myArguments ["<<i+1<<"] = >"<<myArguments[i+1  ]<<"<   " <<std::endl;
                        std::cout << __FILE__<<":"<< __LINE__<< "atof( myArguments ["<<i+1<<"]) = >"<<atof(myArguments[i+1  ])<<"<   " <<std::endl;
                        //std::cout << __FILE__<<":"<< __LINE__<< " >"<<myArguments[++i]<<"<   " <<std::endl;
                                                  //std::cout << __FILE__<<":"<< __LINE__<< " "<<atof(myArguments[++i])<<std::endl;
                                                    			//dbConnection->setGromacsSimulationTime   (atof(myArguments[++i]));
                                                    			                          gromacsSimulationTime = atof(myArguments[++i]);
			//dbConnection->setGromacsSimulationTime   (atof(myArguments[i+1]));
			                          std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                        //++i;
                                                  std::cout <<__FILE__<<":"<<__LINE__<<" You have requested a GROMACS run with simulation time = "<<gromacsSimulationTime <<" ns."<<std::endl;
                        //dbConnection.setJobLibraryPath(jobLibraryPath);
                                              } 
	       } 
               else if ( option == "-WORKINGDIRECTORY"){  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing working directory "<< std::endl; 
			exit(1);
		    } else  {
			workingDirectory  = (myArguments[++i]); // "ERROR! please do not try to access this directly!";//i
                        //std::string tempWorkingDirectory = (myArguments[++i]);
                        //dbConnection->setWorkingDirectory(tempWorkingDirectory); // This will fully validate workingDirectory and create it if needed. Note that it no longer touches the jobInfo table, since that may conflict with other breeder jobs running on the same jobInfo table but different pdbId.
                    } 
	       } 
               else if ( option == "-PARTITION"    ){  
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing partition         "<< std::endl; 
			exit(1);
		    } else  {
			partition  = (myArguments[++i]);
                    } 
	       } 
               else if ( option == "-CHAINSINCOMPLEX"){
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing chainsInMutatedSubunit  "<< std::endl; 
			exit(1);
		    } else  {
			chainsInMutatedSubunit  = (myArguments[++i]);
                    } 
	       } 
               else if ( option == "-REPORTINGINTERVAL"    ){  
                    std::cout << __FILE__<<":"<< __LINE__<<std::endl;
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing reporting interval     "  << std::endl; 
			exit(1);
		    } else  {
                        reportingInterval     = atof(myArguments[++i]);
                        std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                        std::cout <<__FILE__<<":"<<__LINE__<<" You set reportingInterval for the MMB equilibration to  "<< reportingInterval <<" ps."<<std::endl;
                    } 
	       } 
               else if ( option == "-NUMREPORTINGINTERVALS"    ){  
                    std::cout << __FILE__<<":"<< __LINE__<<std::endl;
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: missing  number of reporting intervals     "  << std::endl; 
			exit(1);
		    } else  {
                        numReportingIntervals     = atoi(myArguments[++i]);
                        std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                        std::cout <<__FILE__<<":"<<__LINE__<<" You set numReportingIntervals for the MMB equilibration to  "<< numReportingIntervals <<" ps."<<std::endl;
                    } 
	       } 
               else if ( option == "-FLEXIBILITYWINDOWOFFSET"    ){  
                    std::cout << __FILE__<<":"<< __LINE__<<std::endl;
		    if( myNumArguments < i+2 ) {
			std::cout << "Error: value for -FLEXIBILITYWINDOWOFFSET         "  << std::endl; 
			exit(1);
		    } else  {
                        flexibilityWindowOffset   = atoi(myArguments[++i]);
                        std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                        std::cout <<__FILE__<<":"<<__LINE__<<" You set flexibilityWindowOffset for  the MMB equilibration to  "<< flexibilityWindowOffset <<" residues. This will give you a flexibility zone of "<< flexibilityWindowOffset + flexibilityWindowOffset + 1 <<" residues."<< std::endl;
                    } 
	       }

               else if ( option == "-LASTSTAGE"    ){

                     std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                                     if( myNumArguments < i+2 ) {

                         std::cout << "Error: missing lastStage                          "  << std::endl;
                                 exit(1);
                     } else  {
                         lastStage                      = atoi(myArguments[++i]);

                         if ((lastStage <= 0)) {
                             std::cout << __FILE__<<":"<< __LINE__<<" Unacceptable value of -LASTSTAGE : "<<lastStage<<std::endl;
                             exit(1);
                         }
                         std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                         std::cout <<__FILE__<<":"<<__LINE__<<" You set -LASTSTAGE to  "<< lastStage                          <<std::endl;
                     }
               }
 
               else if ( option == "-RAMP"    ){  
                    std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                    ramp = 1   ; 
                    //i--;
                      		    if( myNumArguments < i+2 ) {
		    	std::cout << "Error: missing  number of reporting intervals     "  << std::endl; 
	            		exit(1);
		    } else  {
                        ramp                       = atoi(myArguments[++i]);
                        if ((ramp != 0) && (ramp != 1)) {
                             std::cout << __FILE__<<":"<< __LINE__<<" Unacceptable value of -RAMP : "<<ramp<<std::endl;
                             exit(1);
                        }
                        std::cout << __FILE__<<":"<< __LINE__<<std::endl;
                        std::cout <<__FILE__<<":"<<__LINE__<<" You set -RAMP to  "<< ramp                          <<std::endl;
                    } 
	       }
               else if ( option == "-FASTAEXECUTABLE") {
                     if( myNumArguments < i+2 ) {
                         std::cout << "Error missing name of fasta executable"  << std::endl;

                         exit(1);
                     } else  {
                         fastaExecutable   = myArguments[++i];

                         struct stat st;
                         if(stat(fastaExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<fastaExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<fastaExecutable<<std::endl; exit(1);}
                     }
               }
               else if ( option == "-BREEDEREXECUTABLE") {
                     if( myNumArguments < i+2 ) {
                         std::cout << "Error missing name of breeder executable"  << std::endl;

                         exit(1);
                     } else  {
                         breederExecutable   = myArguments[++i];

                         struct stat st;
                         if(stat(breederExecutable .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<breederExecutable<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<breederExecutable<<std::endl; exit(1);}
                     }
               }
               else if ( option == "-FASTATEMPDIRECTORY") {
                     if( myNumArguments < i+2 ) {

                         std::cout << "Error missing name of -FASTATEMPDIRECTORY"  << std::endl;
                         exit(1);

                     } else  {

                         fastaTempDirectory            = myArguments[++i];

                         struct stat st;

                         if(stat(fastaTempDirectory .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<fastaTempDirectory<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<fastaTempDirectory<<std::endl; exit(1);}
                         std::cout<<__FILE__<<":"<<__LINE__<<" Set fastaTempDirectory = >"<< fastaTempDirectory<<"< ."<<std::endl;
                     }
	       }
               else {
		    std::cout << "Error! Unrecognized option: >" << myArguments[i] <<"< "<< std::endl;
		    //printUsage();
		                          exit(1);
               }
	   } // of for i
        } // of  readParameters

void BreederParameterReader::setDefaults (){
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        pdbId = "";
        jobId = "XXXX";
        listFileName = "XXXX";
        msiScannedChains   = "";
        msiPartnerChains   = "";
        jobLibraryPath   = "";
        workingDirectory = "";
        partition        = "";
        chainsInMutatedSubunit = "";
        oneMutationString  = "";
        wtRenumberedStructureFileName= "";
        msiRadius = -1111.0 ;
        listTypeEnum = unassigned;
        listTypeEnum = sequencesString;
        SQLTypes sQLSystem = SQLite; // Default
        server = SERVER;
        user = USER;
        database = DATABASE;
        password = PASSWORD;
        account  = ACCOUNT ;
        foldXScript = "";
        SQLExecutable = "";
        SQLPassword   = "";
        foldXExecutable = "";
        breederMainDirectory = "";
        MMBExecutable = ""  ;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        emailAddress = "";
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        singleMutantFilesDirectory = "";
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        mobilizerWithinRadius = 0.0;
        waterRadius = 0.0;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        maxTime = 70;
        gromacsSimulationTime = 0.0;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        reportingInterval     = -1111.0;
	temperature = -11111.11111;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
	mutantAffinityInM = 11111.11111;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
	wildTypeAffinityInM = 11111.11111;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
	pdbChains = "";
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        sequence = 0;  
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        numReportingIntervals = -1111;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        flexibilityWindowOffset = 2;
        lastStage = 0    ;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        ramp = 0    ;
        // homologyScanner specific parameters. Not required by breeder:
        fastaExecutable = ""  ;     
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        fastaTempDirectory = ""  ;
        //arg ="";
        //option ="";
        singleHomologyScannerRun = 0;
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        chainsInHomologComplex = "";     
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
        homologPdbId = "";          
        std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
}
