// Breed.cpp
//#include "DBManager.h"
//#include <sys/stat.h>
#include <Breed.h>      
#include "SimTKmolmodel.h"

#include "BiopolymerClass.h"
#include <unistd.h>
	std::string runJobString(std::string mySingleMutantID,std::string myWorkingDirectory,  std::string mySingleMutantDirectory,  bool computeIndividualWildType, DBManager & mysqlConnection) { 
                return mysqlConnection.runJobString(mySingleMutantID,myWorkingDirectory,mySingleMutantDirectory,computeIndividualWildType);
	}

	void Breed::spawn (Chromosome *  chromosome, DBManager & mysqlConnection) { 
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
		std::string singleMutantDirectory = mysqlConnection.getWorkingDirectory() ; //+ "/" + chromosome->getSingleMutantID() + "/"  ;
		singleMutantDirectory +=  "/" + chromosome->getSingleMutantID() + "/"  ;
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
		Chromosome * wildTypeEquivalentChromosome = new Chromosome();
		*wildTypeEquivalentChromosome = *chromosome;
		//if (chromosome->biopolymerClassContainer.get
		wildTypeEquivalentChromosome->revertToWildTypeEquivalent(); 
                std::string mySQLExecutableAndParameters = mysqlConnection.getSQLExecutable() //+ std::string(" --socket=/home/samuelf/mysql-5.7.13-linux-glibc2.5-x86_64/socket ")  
                    + std::string(" --defaults-extra-file=") +  mysqlConnection.getSingleMutantFilesDirectory() + std::string("/option-file.mysql");
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;                
		bool computeIndividualWildType = wildTypeEquivalentChromosome->checkIndividualComputation(mysqlConnection);
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		bool wildTypeIndividualComputation = false; 
		bool chromosomeHasBeenComputed = chromosome->hasBeenComputed( mysqlConnection);
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
   		if (!(chromosomeHasBeenComputed )) { 
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
			chromosome->prepareSingleMutant(mysqlConnection, singleMutantDirectory, wildTypeIndividualComputation);
		}
                // SCF added 26 June 2014:
                //mysqlConnection.insertUpdateDelete("update results set wildTypeString = \'" + wildTypeEquivalentChromosome->getSingleMutantID() + "\' where mutationString = \'" + chromosome->getSingleMutantID() + "\' and jobName = \'" + mysqlConnection.getJobID() + "\' ;"); // Useful to have later, when generating MultiScanning plots.
                mysqlConnection.insertUpdateDelete("update results set wildTypeString = \'" + wildTypeEquivalentChromosome->getSingleMutantID() + "\' where mutationString = \'" + chromosome->getSingleMutantID() + "\' and " + mysqlConnection.jobIDAndPdbIdSelectionString() +  " ;");
                mysqlConnection.setComplexStringInResultsTable(chromosome->getSingleMutantID()); // This sets the complexString in the results table, using the mutationString = chromosome->getSingleMutantID() .
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
                // Here we create the job file and set SLURM parameters. However all the brainy work is done in runJobString in MysqlConnection.cpp
		std::string jobFileName = singleMutantDirectory + std::string("/job.") + chromosome->getSingleMutantID();
		std::ofstream jobFile;//(jobFileName.c_str());
                jobFile.open(jobFileName.c_str(),  std::ofstream::out );
		std::cout<<__FILE__<<":"<<__LINE__<<" rdstate = >"<<jobFile.rdstate() <<"<" << std::endl; 
		std::cout<<__FILE__<<":"<<__LINE__<<" bad = >"<<jobFile.bad() <<"<" << std::endl; 
		std::cout<<__FILE__<<":"<<__LINE__<<" fail = >"<<jobFile.fail() <<"<" << std::endl; 
		std::cout<<__FILE__<<":"<<__LINE__<<" eof = >"<<jobFile.eof() <<"<" << std::endl; 
                if (jobFile.fail()) {
		    std::cout<<__FILE__<<":"<<__LINE__<<" fail = >"<<jobFile.fail() <<"<" << std::endl; 
		    std::cout<<__FILE__<<":"<<__LINE__<<" failbit or badbit set for file  >"<<jobFileName <<"<" << std::endl; 
                    exit(1);
                }

	        if (jobFile.is_open()) {
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
     		        jobFile<<"#!/bin/bash -l\n";
     		        //jobFile<<"#  The following is generated in "<<__FILE__<<":"<<__LINE__<<"\n"<<std::endl;
			jobFile<<"#SBATCH -J "<<chromosome->getMutationString()<<"\n";
			std::string accountName = mysqlConnection.getAccountName();
			std::string myPartition = mysqlConnection.getPartition();
                        if ((myPartition.compare("NULL") != 0) && 
                             (myPartition.compare("") != 0)   &&
                             (myPartition.compare("NO-RESULT") != 0)){
			    jobFile<<"#SBATCH -p "<< myPartition<<"\n"; // was -p batch .  However tintin no longer tolerates this.  Now went to "core" meaning we request one core of the 16-core node, getting 4GB RAM max.  A "node" job would get all 16 cores on the node.
                        }
                        int myNumCores = 0;
                        //std::string  myQueryString = std::string("select numCores  from jobInfo where runID = \"") + mysqlConnection.getJobID() + std::string("\" ;");
                        myNumCores = mysqlConnection.getNumCores();//queryValueAsInt(myQueryString);
                        if (myNumCores > 0) {
                            // This is temporarily disabled .. seems not to work right
			    //jobFile<<"#SBATCH -n "<<myNumCores<<"\n"; 
                        }
                        if (mysqlConnection.gromacsShouldRun()){
                            jobFile<<"#SBATCH -N 2 \n";
                            jobFile<<"#SBATCH --tasks-per-node=20 \n";
                            jobFile<<"#SBATCH --exclusive \n";}
                        if (accountName.length() > 0) {
			    jobFile<<"#SBATCH -A "<<accountName<<"\n"; // snic2013-1-302\n";  // Only write this line if account name is of nonzero length
                        }
                        float floatHours = mysqlConnection.getMaxTime();
			int hours = int(floatHours);
                        float floatMinutes = ((floatHours-hours)*60);
  			int   minutes = int(floatMinutes);
                        int   seconds = int((floatMinutes-minutes)*60);
                        if ((minutes <0) || (minutes >60)) {
			    std::cout<<__FILE__<<":"<<__LINE__<<" Unreasonable number of minutes : "<<minutes<< " ! Error! "<<std::endl; exit(1);
                        }
                        if ((seconds <0) || (seconds >60)) {
			    std::cout<<__FILE__<<":"<<__LINE__<<" Unreasonable number of seconds : "<<seconds<< " ! Error! "<<std::endl; exit(1);
                        }
			jobFile<<"#SBATCH -t "<<hours<<":"<<setfill('0')<<setw(2)<<minutes<<":"<<setfill('0')<<setw(2)<<  seconds<<"\n";
			jobFile<<"#SBATCH --mem 4000"<<"\n";
                        jobFile<<"#SBATCH -o "<< jobFileName << ".%j.slurm.out"<<"\n"; 
                        std::string myJobLibraryPath = mysqlConnection.getJobLibraryPath();
                        if (myJobLibraryPath.length() > 0) {
			    jobFile<<"export LD_LIBRARY_PATH="<<myJobLibraryPath<<"\n";   // Only write this line if jobInfo table entry "jobLibraryPath"" is of nonzero length
                        }
                        //jobFile <<"echo \"update results set startTime = NOW() where jobName = \\\""<< mysqlConnection.getJobID()  <<"\\\" and mutationString = \\\""<< chromosome->getMutationString()  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl; 
                        jobFile <<"echo \"update results set startTime = NOW() where jobName = \\\""<< mysqlConnection.getJobID()  <<"\\\" and pdbId = \\\""<< mysqlConnection.getPdbId() <<"\\\"  and mutationString = \\\""<< chromosome->getMutationString()  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl;
                        jobFile<<"# End part that is written in "<<__FILE__<<":"<<__LINE__<<"\n"<<std::endl;
                } else {
			std::cout<<__FILE__<<":"<<__LINE__<<" Unable to write to "<<jobFileName<<std::endl; exit(1);
		}

	
	        //std::string wildTypeSingleMutantDirectory = mysqlConnection.getWorkingDirectory() + "/" + wildTypeEquivalentChromosome->getSingleMutantID() + "/" ;
		bool wildTypeEquivalentChromosomeHasBeenComputed = wildTypeEquivalentChromosome->hasBeenComputed( mysqlConnection);
		
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
		

                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                std::string wildTypeSingleMutantDirectory = singleMutantDirectory +"/"+ wildTypeEquivalentChromosome->getSingleMutantID() + "/";
                //std::string wildTypeWorkingDirectory = mysqlConnection.getWorkingDirectory() +"/"+ chromosome->getSingleMutantID();
                // 1 was wildTypeIndividualComputation. Have to prepare WT directory regardless, so it receives the WT_raw_Repair_1.pdb file.
                wildTypeEquivalentChromosome->prepareSingleMutant(mysqlConnection, wildTypeSingleMutantDirectory, 1 );


                 std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                 // This condition should parallel taht a couple of blocks down.
                 if (!(chromosomeHasBeenComputed))   {
                     // check again for existence of raw.pdb
                     //std::ifstream testFileStream2(singleMutantDirectory + std::string("/raw.pdb"));
                     //if (!(testFileStream2.good())) {std::cout<<__FILE__<<":"<<__LINE__<<" Could not open "<<singleMutantDirectory + std::string("/raw.pdb")<<std::endl; exit(1); }
                     //testFileStream2.close();

                     jobFile<< "cd "     << singleMutantDirectory<<" ; "<<std::endl;
                     jobFile<< mysqlConnection. getFoldXExecutable()  <<" --command=RepairPDB   --repair_Interface=ONLY --temperature="<<mysqlConnection. getTemperatureInK()<<"  --water=-CRYSTAL --pdbHydrogens=false --pdb=raw.pdb    ;"<<std::endl;
                     // The semicolon after the mutation string matters.
                     //jobFile<< "echo \"" << mysqlConnection. getUserProvidedMutationString() <<";\" > individual_list.txt ; "<<std::endl;
                     jobFile<< "echo \"" <<                     chromosome->updBiopolymerClassContainer().getFoldxFormattedMutations()  <<";\" > individual_list.txt ; "<<std::endl ;
                     jobFile<< mysqlConnection. getFoldXExecutable()  <<" --command=BuildModel --pdb=raw_Repair.pdb --mutant-file=individual_list.txt   "<<std::endl;
                     jobFile<< "cp raw_Repair_1.pdb     "<< /*singleMutantDirectory        <<*/ "last.1.pdb ; "<<std::endl;
		     // we have alredy cd'd into singleMutantDirectory. For the docker version it is problematic to now also redundantly specify the full path.
                     //jobFile<< "cp WT_raw_Repair_1.pdb  "<< /*wildTypeSingleMutantDirectory<<"/"<<*/ "last.1.pdb ; "<<std::endl;
		     jobFile<< "cp WT_raw_Repair_1.pdb  "<< wildTypeEquivalentChromosome->getSingleMutantID() << "/"  <<"last.1.pdb ; "<<std::endl;

                 }



                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		if (mysqlConnection.shouldRunZEMuForJobWildType(chromosome->getSingleMutantID(), wildTypeEquivalentChromosome->getSingleMutantID() )){
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
			std::cout<<__FILE__<<":"<<__LINE__<<" At this point, mysqlConnection.getFoldXExecutable     () returns: "<<mysqlConnection.getFoldXExecutable     ()<<std::endl; 
			std::cout<<__FILE__<<":"<<__LINE__<< " chromosome->getSingleMutantID() = "<<chromosome->getSingleMutantID()<<std::endl; 
			std::cout<<__FILE__<<":"<<__LINE__<< " singleMutantDirectory  = "<<singleMutantDirectory<<std::endl; 
			wildTypeIndividualComputation = true; // Deprecated
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
			//std::string wildTypeSingleMutantDirectory = singleMutantDirectory +"/"+ wildTypeEquivalentChromosome->getSingleMutantID() + "/";
			std::string wildTypeWorkingDirectory = mysqlConnection.getWorkingDirectory() +"/"+ chromosome->getSingleMutantID();
			//wildTypeEquivalentChromosome->prepareSingleMutant(mysqlConnection, wildTypeSingleMutantDirectory, wildTypeIndividualComputation);
			jobFile<< runJobString(wildTypeEquivalentChromosome->getSingleMutantID(),wildTypeWorkingDirectory, wildTypeSingleMutantDirectory, computeIndividualWildType, mysqlConnection); //.getDatabase(), mysqlConnection.getMMBExecutable(), mysqlConnection.getFoldXScript() ,mysqlConnection.getFoldXExecutable     () );
		} else { // If we are not computing the individual wild type, breeder should populate foldx_energy_wild_type with the found suitable value. At this point that should have happened. 
                        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                }
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		if (!(chromosomeHasBeenComputed))   {
			std::cout<<__FILE__<<":"<<__LINE__<< " wildTypeEquivalentChromosome->getSingleMutantID() = "<<wildTypeEquivalentChromosome->getSingleMutantID()<<std::endl; 
			std::cout<<__FILE__<<":"<<__LINE__<< " wildTypeSingleMutantDirectory  = "<<wildTypeSingleMutantDirectory<<std::endl; 
			jobFile<<"#"<<__FILE__<<":"<<__LINE__<< " wildTypeSingleMutantDirectory  = "<<wildTypeSingleMutantDirectory<<std::endl; 
			jobFile<< runJobString(chromosome->getSingleMutantID(), mysqlConnection.getWorkingDirectory(),  singleMutantDirectory, !computeIndividualWildType,  mysqlConnection); 
		}
		if (mysqlConnection.gromacsShouldRun()) {
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl; 
                        // First we need to prepare a PDB structure with only the mutated chain present
                        std::string zemuEquilibratedPdbFileName(singleMutantDirectory + std::string("/last.2.pdb"));
                        // I think we don't need this.. but we should watch out for conflicts..
                        chromosome->updBiopolymerClassContainer().deleteAllBiopolymerClasses();
                        chromosome->updBiopolymerClassContainer().loadSequencesFromPdb(zemuEquilibratedPdbFileName, false,"", true,true);
                        // Here we delete all but the mutated chain
                        chromosome->updBiopolymerClassContainer().deleteAllNonMutatedBiopolymerClasses();                    
                        SimTK::CompoundSystem system;
                        vector<Displacement> tempDisplacementVector; tempDisplacementVector.clear();
                        chromosome->updBiopolymerClassContainer().initializeBiopolymers(system,false,true,false,false,false,false,true,0,tempDisplacementVector ,.150,.07);
                        std::string zemuEquilibratedTruncatedPdbFileName(singleMutantDirectory + std::string("/zemu-equilibrated-truncated.pdb"));
			std::ofstream zemuEquilibratedTruncatedPdbFile;
			zemuEquilibratedTruncatedPdbFile.open(zemuEquilibratedTruncatedPdbFileName.c_str(),  std::ofstream::out );
                        chromosome->updBiopolymerClassContainer().writeDefaultPdb(zemuEquilibratedTruncatedPdbFile);
                        zemuEquilibratedTruncatedPdbFile.close();

                        std::cout<<__FILE__<<":"<<__LINE__<<endl;

			jobFile<< mysqlConnection.runGromacsSimulationString(chromosome->getSingleMutantID(),singleMutantDirectory, computeIndividualWildType );
			//jobFile.close();
			

		}
                jobFile <<"echo \"update results set   endTime = NOW() where jobName = \\\""<< mysqlConnection.getJobID()  <<"\\\" and pdbId = \\\""<< mysqlConnection.getPdbId() <<"\\\" and  mutationString = \\\""<< chromosome->getMutationString()  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl; 
                jobFile <<"echo \"update results set experimentalDDG = "<< mysqlConnection.getExperimentalDDG() << " where jobName = \\\""<< mysqlConnection.getJobID()  <<"\\\" and pdbId = \\\""<< mysqlConnection.getPdbId() <<"\\\" and  mutationString = \\\""<< chromosome->getMutationString()  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl; 
                jobFile <<"echo \"update results set temperature = "<< mysqlConnection.getTemperatureInK() << " where jobName = \\\""<< mysqlConnection.getJobID()  <<"\\\" and pdbId = \\\""<< mysqlConnection.getPdbId() <<"\\\" and  mutationString = \\\""<< chromosome->getMutationString()  <<"\\\" ; \" | "<<mySQLExecutableAndParameters<<" ; " <<std::endl; 

               std::string leafJobCompleteEmailContents = "Dear User,\n\n \
\
Thank you for using the homologyScanner web server. Your contribution not only provides you with the DDG for your protein of choice, it also provides you with the PDB IDs of structurally related and possibly useful complexes, and lastly translates your mutation to the numbering system of those other complexes.\n\n\
\
If you are getting this message, the the effect of the  mutation you specified has been computed on a single PDB structure as named above. There may still be other PDB structures queued, and if so you will receive another email soon.\n\n\
\
This result has been added to the synopsis table. If the calculation has been done on any other homologs, these are available there as well. If there are any homologs still in the queue, their results will be added to the table when those are completed. The link to the table was sent to you in a prior email. You can also view the results in the structure viewer (click the View tab on biodesign.scilifelab.se , and select the values from the drop down lists.).\n\n Bye" ;

               jobFile <<"printf \"" <<leafJobCompleteEmailContents <<"\" | "<<" /usr/bin/mail -s \"Update: results computed for PDB ID: "<< mysqlConnection.getPdbId()<< "\" "<< mysqlConnection.emailAddress << ",samuelfloresc@gmail.com";
               //jobFile <<"/usr/bin/sendemail -m \"" + leafJobCompleteEmailContents + "\"  -u \"Update: results computed for PDB ID: " + mysqlConnection.getPdbId() +"\" -t " + mysqlConnection.emailAddress + ",samuelfloresc@gmail.com -f sam@pe1.scilifelab.se ";

                jobFile.flush();
		jobFile.close();
                struct stat st;
                if(stat(jobFileName .c_str() ,&st) == 0) { std::cout<<__FILE__<<":"<<__LINE__<<" Successfully located "<<jobFileName<<std::endl;} else {std::cout<<__FILE__<<":"<<__LINE__<<" Failed to find "<<jobFileName<<std::endl; exit(1);}
                if(st.st_size == 0)                      { std::cout<<__FILE__<<":"<<__LINE__<<" File  "<<jobFileName<<" is of size "<<st.st_size<<" . Exiting now. "<<std::endl;
                    exit(1);} else 
                {
                    std::cout<<__FILE__<<":"<<__LINE__<<"  File  "<<jobFileName<<" is of size "<<st.st_size<<" . Continuing. "<<std::endl;
                }
                  
                // We CD to the mutant directory before submitting the job. This is because the environment, including current directory, is copied to the session. That way we will get the slurm output in the mutant directory
		std::string submitJobString = //"sudo date  --set=\'20170728 1518\' ";
                    ////std::string("cd ") + std::string(singleMutantDirectory) + std::string("; ")
                    //std::string("chmod 777 ") + jobFileName + "; " 
                    //// Decided for a web server it's best not to separately queue foldx jobs. These run pretty fast now anyway, so we can do it this way even for desktop use. 
                    // Changed my mind, because now am encountering "Caught Killed" errors which I cannot understand or fix. I moved the sendemail command to the job file to ensure it is sent after the job is done and not before.
                    //+ jobFileName;
                    std::string("sbatch ") + jobFileName;
                int systemCallReturnValue = -11111;
                std::cout<<__FILE__<<":"<<__LINE__<<" issuing system call : >"<<submitJobString<<"< "<<std::endl; 
		systemCallReturnValue = system(submitJobString.c_str());	
                std::cout<<__FILE__<<":"<<__LINE__<<" Command returned : >"<<systemCallReturnValue<<"< "<<std::endl;

                if(st.st_size == 0)                      { std::cout<<__FILE__<<":"<<__LINE__<<" File  "<<jobFileName<<" is of size "<<st.st_size<<" . Exiting now. "<<std::endl;
                    exit(1);} else 
                {
                    std::cout<<__FILE__<<":"<<__LINE__<<"  File  "<<jobFileName<<" is of size "<<st.st_size<<" . Continuing. "<<std::endl;
                }
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                int attempts = 0;
                int sleepMicroseconds = 10000000; // 10 seconds
		/*
                // This loop can be reinstated if we change the submission system to one which requires multiple attempts.
                while ((systemCallReturnValue) && (attempts < 20)) {
		    if(st.st_size == 0)                      { std::cout<<__FILE__<<":"<<__LINE__<<" File  "<<jobFileName<<" is of size "<<st.st_size<<" . Exiting now. "<<std::endl;
		        exit(1);} else 
		    {
		        std::cout<<__FILE__<<":"<<__LINE__<<"  File  "<<jobFileName<<" is of size "<<st.st_size<<" . Continuing. "<<std::endl;
		    }
                    std::cout<<__FILE__<<":"<<__LINE__<<" issuing system call : >"<<submitJobString<<"< "<<std::endl; 
		    systemCallReturnValue = system(submitJobString.c_str());	
                    std::cout<<__FILE__<<":"<<__LINE__<<" Command returned : >"<<systemCallReturnValue<<"< "<<std::endl;
                    attempts ++;
                    if (systemCallReturnValue) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Sleeping for : >"<<sleepMicroseconds<<"< microseconds "<<std::endl;
                        usleep(sleepMicroseconds);  } 
                }
                if (systemCallReturnValue) {
                    std::cout<<__FILE__<<":"<<__LINE__<<" Command returned : >"<<systemCallReturnValue<<"< "<<std::endl;
                    std::cout<<__FILE__<<":"<<__LINE__<<" Number of attempts : >"<<attempts<<"< "<<std::endl;
                    std::cout<<__FILE__<<":"<<__LINE__<<" Exiting now."<<std::endl;
                    exit(systemCallReturnValue);
                } else {
                }
		*/
		std::cout<<__FILE__<<":"<<__LINE__<<" Done with job submission."<<std::endl;
	        delete wildTypeEquivalentChromosome;
		return; // This is not necessary. But I like to be parallel.	
	};

	std::string Breed::selectRandomParent(DBManager & mysqlConnection ) {
           std::cout<<__FILE__<<":"<<__LINE__<<" About to call mysql to retrieve a random sequence "<<std::endl;
           return mysqlConnection.retrieveSequenceAtRandom();        
	};
	/*void selectBreedingPopulation() {
	};*/
        void Breed::setGenerationSize(int myGenerationSize){ 
		std::cout<<__FILE__<<":"<<__LINE__<<"Obsolete"<<std::endl; exit(1);
		if (myGenerationSize < 1) {
			std::cout<<__FILE__<<":"<<__LINE__<<" Generation size must be >= 1"<<std::endl; exit(1);
		}
		generationSize = myGenerationSize;}	

        int Breed::getNumMutantsInDatabase (Chromosome & myChromosome, DBManager & mysqlConnection) {
	    //std::cout<<__FILE__<<":"<<__LINE__<<" Looking for any existing "<<myChromosome.getMutationString()<< " mutants with jobID "<<mysqlConnection.getJobID()<<std::endl;
	    //int identicalMutantsInDatabase ;
	    /*std::string numMutantsQuery = " select count(*) from results where mutationString = \"";
	    numMutantsQuery += myChromosome.getMutationString();
	    numMutantsQuery += "\" and jobName = \"";
	    numMutantsQuery += mysqlConnection.getJobID();
	    numMutantsQuery += "\"; ";
	    identicalMutantsInDatabase = mysqlConnection.queryValueAsInt(numMutantsQuery.c_str() );*/

	    return getNumMutantsInDatabase(myChromosome.getMutationString(),mysqlConnection);    
        }
        int Breed::getNumMutantsInDatabase (const std::string myMutationString, DBManager & mysqlConnection) {
	    std::cout<<__FILE__<<":"<<__LINE__<<" Looking for any existing "<< myMutationString<< " mutants with jobID "<<mysqlConnection.getJobID()<<std::endl;
	    int identicalMutantsInDatabase ;
	    std::string numMutantsQuery = " select count(*) from results where mutationString = \"";
	    numMutantsQuery +=  myMutationString;
	    numMutantsQuery += "\" and "; //jobName = \"";
            numMutantsQuery += mysqlConnection.jobIDAndPdbIdSelectionString();
	    //numMutantsQuery += mysqlConnection.getJobID();
	    //numMutantsQuery += "\" and pdbId = \'";
            //numMutantsQuery += mysqlConnection.getPdbId(); 
            numMutantsQuery += " ; ";
	    identicalMutantsInDatabase = mysqlConnection.queryValueAsInt(numMutantsQuery.c_str() );
	    return identicalMutantsInDatabase;
        }

        /*int Breed::getNumSequencesInDatabase (std::string sequence, DBManager & mysqlConnection) {
                std::cout<<__FILE__<<":"<<__LINE__<<" This is outdated .. I recommend using getNumMutantsInDatabase (Chromosome & myChromosome, DBManager & mysqlConnection) ."<<std::endl; exit(1);  
                std::cout<<__FILE__<<":"<<__LINE__<<" Looking for any existing sequences identical to "<<sequence<<" with jobID "<<mysqlConnection.getJobID()<<std::endl;
                int identicalSequencesInDatabase ;
		std::string numSequencesQuery = " select count(*) from results where sequence = \"";
		numSequencesQuery += sequence;
		numSequencesQuery += "\" and jobName = \"";
		numSequencesQuery += mysqlConnection.getJobID();
		numSequencesQuery += "\"; ";
		identicalSequencesInDatabase = mysqlConnection.queryValueAsInt(numSequencesQuery.c_str() );
		return identicalSequencesInDatabase;
        }*/
 
        int Breed::getNumSequencesInDatabase (Chromosome myChromosome, DBManager & mysqlConnection) {
                        std::cout<<__FILE__<<":"<<__LINE__<<" Looking for any existing sequences identical to "<<myChromosome.getSequences()<<" AND mutationString identical to "<<myChromosome.getMutationString()<<" AND with jobID "<<mysqlConnection.getJobID()<<std::endl;
                        int identicalSequencesInDatabase ;
			std::string numSequencesQuery = "select count(*) from results where sequence = \"";
			numSequencesQuery += myChromosome.getSequences();
			numSequencesQuery += "\" and mutationString = \"";
			numSequencesQuery += myChromosome.getMutationString();
			numSequencesQuery += "\" and "; //jobName = \"";
                        numSequencesQuery += mysqlConnection.jobIDAndPdbIdSelectionString(); 
			//numSequencesQuery += mysqlConnection.getJobID();
			numSequencesQuery += "; ";
			identicalSequencesInDatabase = mysqlConnection.queryValueAsInt(numSequencesQuery.c_str() );
			return identicalSequencesInDatabase;
        } 
       
	void Breed::spawnNewGeneration (DBManager & mysqlConnection ) {

		int spawnedChromosomes;
                spawnedChromosomes = 0;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		generationSize = mysqlConnection.queryValueAsInt(string(string("select generationSize from jobInfo where runID = \"") + mysqlConnection.getJobID()+ string("\" ;")  ).c_str());
		BiopolymerClassContainer wildTypeBiopolymerClassContainer;
		wildTypeBiopolymerClassContainer.loadSequencesFromPdb(mysqlConnection.getAlternateRenumberedStructureFile(), bool(false),"",true, true ); // read in sequences from last.1.pdb
		int myIndex1 = 0;
		int myIndex2 = 0 ;
		while (spawnedChromosomes < generationSize) {
                        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                        std::string randomParentSequence1 ;
                        std::string randomParentSequence2 ;
                        if (!(mysqlConnection.isNull("maximumParentEnergy"))) { // if maximumParentEnergy is not NULL, this is taken to mean that you want to exhaustively combine all mutants with energy <= maximumParentEnergy. 
				std::cout<<__FILE__<<":"<<__LINE__<<" Detected a non-NULL maximumParentEnergy.  Thus we will look only for parents with energy <=  "<<mysqlConnection.getMaximumParentEnergy () <<std::endl;
				std::cout<<__FILE__<<":"<<__LINE__<<" About to select first sequence at random from database .. Using index1 : "<<myIndex1<<std::endl;
				randomParentSequence1 = mysqlConnection.retrieveSequenceInOrder(myIndex1,"mutationString","asc");
                                if (myIndex1 >0) {
                                        std::cout<<__FILE__<<":"<<__LINE__<<" About to decrement myIndex1 from : "<<myIndex1<<std::endl;
					//myIndex1--; // retrieveSequenceInOrder increments the index.  we want to  myIndex1 to increment only when myIndex2 rolls over
                                        std::cout<<__FILE__<<":"<<__LINE__<<" to : "<<myIndex1<<std::endl;
				}
				myIndex2 ++; // its OK for this to start at 1, b.c. it must be greater than myIndex1
				
				std::cout<<__FILE__<<":"<<__LINE__<<" About to select second sequence at random from database .. Using index2 : "<<myIndex2<<std::endl;
				randomParentSequence2 = mysqlConnection.retrieveSequenceInOrder(myIndex2,"mutationString","asc");
                                if (myIndex2 == 0) {
                                        std::cout<<__FILE__<<":"<<__LINE__<<" About to increment myIndex1 from : "<<myIndex1<<std::endl;
					myIndex1 ++ ;
                                        std::cout<<__FILE__<<":"<<__LINE__<<" to : "<<myIndex1<<std::endl;
                                        std::cout<<__FILE__<<":"<<__LINE__<<" About to update    myIndex2 from : "<<myIndex2<<std::endl;
					myIndex2 = myIndex1 ; // will be incremented before retrieveSequenceInOrder(myIndex2 .. is called
                                        std::cout<<__FILE__<<":"<<__LINE__<<" to : "<<myIndex2<<std::endl;
				}
			} else {
				std::cout<<__FILE__<<":"<<__LINE__<<" About to select first sequence at random from database .."<<std::endl;
				randomParentSequence1 = selectRandomParent(mysqlConnection );
				std::cout<<__FILE__<<":"<<__LINE__<<" About to select second sequence at random from database .."<<std::endl;
				randomParentSequence2 = selectRandomParent(mysqlConnection );
				
			}
                        if (randomParentSequence1.compare(wildTypeBiopolymerClassContainer.getFormattedSequencesString()) == 0) {
 				std::cout<<__FILE__<<":"<<__LINE__<<" randomParentSequence1 = "<<randomParentSequence1<<", which is the same as the wild type sequence! Cannot continue."<<std::endl; continue;
			}
                        if (randomParentSequence2.compare(wildTypeBiopolymerClassContainer.getFormattedSequencesString()) == 0) {
 				std::cout<<__FILE__<<":"<<__LINE__<<" randomParentSequence2 = "<<randomParentSequence2<<", which is the same as the wild type sequence! Cannot continue."<<std::endl; continue;
			}
                        if (randomParentSequence1.compare(randomParentSequence2) == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" The two sequences : "<<randomParentSequence1<<" and "<<randomParentSequence2<<" are equal! Will not cross these two."<<std::endl; continue;} else
                        {std::cout<<__FILE__<<":"<<__LINE__<<" The two sequences : "<<randomParentSequence1<<" and "<<randomParentSequence2<<" are different -- it's OK to continue"<<std::endl; }
                        std::cout<<__FILE__<<":"<<__LINE__<<" About to create a new chromosome by crossing parent1 "<<randomParentSequence1<<" and parent2: "<<randomParentSequence2<<std::endl;
			Chromosome newChromosome(randomParentSequence1, randomParentSequence2, mysqlConnection.getMaxMutations(),wildTypeBiopolymerClassContainer);
			std::cout<<__FILE__<<":"<<__LINE__<<" child mutationString = "<<newChromosome.getMutationString()<<std::endl;
			if      ((newChromosome.getSequences()).compare(randomParentSequence1) == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" parentSequence1 and child sequence are identical! Cannot continue with submission"<<std::endl; continue; }
			else if ((newChromosome.getSequences()).compare(randomParentSequence2) == 0) {std::cout<<__FILE__<<":"<<__LINE__<<" parentSequence2 and child sequence are identical! Cannot continue with submission"<<std::endl; continue; }
			else {std::cout<<__FILE__<<":"<<__LINE__<<" parents differ from child. Continuing with submission."<<std::endl;}

			std::cout<<__FILE__<<":"<<__LINE__<<" You have "<<wildTypeBiopolymerClassContainer.getNumBiopolymers()<<" biopolymers"<<std::endl;
			if (wildTypeBiopolymerClassContainer.getNumBiopolymers() <1) {
				std::cout<<__FILE__<<":"<<__LINE__<<" You have "<<wildTypeBiopolymerClassContainer.getNumBiopolymers()<<" biopolymers .. something is wrong .."<<std::endl; exit(1);
			} 
                        std::cout<<__FILE__<<":"<<__LINE__<<" Done creating new chromosome by crossing parent1 "<<randomParentSequence1<<" and parent2: "<<randomParentSequence2<<std::endl;
			std::cout<<__FILE__<<":"<<__LINE__<<" newChromosome.getBiopolymerClassContainer().getNumBiopolymers() = "<<newChromosome.getBiopolymerClassContainer().getNumBiopolymers()<<std::endl;
			Chromosome wildTypeChromosome(mysqlConnection.getMaxMutations() ,wildTypeBiopolymerClassContainer); 
                        if (mysqlConnection.gromacsShouldRun()) {std::cout<<__FILE__<<":"<<__LINE__<<" You have requested a GROMACS MD run with simulation time = "<<mysqlConnection.getGromacsSimulationTime()<<" ns. Continuing with job submission."<<std::endl;}
			else if (getNumMutantsInDatabase(newChromosome, mysqlConnection ) ) {
			//else if (getNumSequencesInDatabase(newChromosome.getSequences(), mysqlConnection ) ) {
                            std::cout<<__FILE__<<":"<<__LINE__<<" The number of sequences like "<< newChromosome.getSequences()  <<" already in the database is not zero! We cannot submit this job."<<std::endl; 
                            // uncomment this to restore normal stopping behavior:
                            continue; 
                        } 
                        else {std::cout<<__FILE__<<":"<<__LINE__<<" The number of sequences like "<< newChromosome.getSequences()  <<" already in the database is exactly zero.  Continuing with job submission .."<<std::endl;}
                        if (newChromosome.numMutations() > mysqlConnection.getMaxMutations()  ){
				std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutations = "<<newChromosome.numMutations()<<" is greater than the maximum of "<<mysqlConnection.getMaxMutations()<<", we cannot continue with submission."<<std::endl; continue;
			}
			else if (newChromosome.numMutations() > 0) {
				std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutations = "<<newChromosome.numMutations()<<" which is >0, continuing with submission."<<std::endl;
			} else {
				// No mutations.  This seems to occur rarely, or perhaps never.
				std::cout<<__FILE__<<":"<<__LINE__<<" This contains no mutations! Will not submit."<<std::endl;
				continue;
			}
                        std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			if (newChromosome.validate()) {
                                std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome passed validation.  Continuing with submission. "<<std::endl;
				spawn (&newChromosome,mysqlConnection) ;
				spawnedChromosomes ++;
				std::cout<<__FILE__<<":"<<__LINE__<<" Have now spawned "<<spawnedChromosomes<<" chromosomes out of a total of "<< generationSize<<" for this generation"<<std::endl;
			} else {
				std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome failed validation for some reason.  Cannot continue with submission."<<std::endl;
			}
		}
	};

        int  Breed::loadSequenceTable(DBManager & mysqlConnection) {
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                BiopolymerClassContainer * wildTypeBiopolymerClassContainer = new BiopolymerClassContainer();
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                wildTypeBiopolymerClassContainer->loadSequencesFromPdb (mysqlConnection.getRenumberedWildTypeStructureFileName(),bool(false),"",true,true);
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                Chromosome * newChromosome = new Chromosome();
                // We are no longer using sequencesString's.  Let's just simplify and restrict to mutationString's.
                // newChromosome = Chromosome(0, wildTypeBiopolymerClassContainer); // first arg is maxMutations. This polymorphism only sets up a Chromosome based on the BiopolymerClassContainer, does not do any mutations. Actually should die if any mutations are somehow generated.
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                // newChromosome.populateSequenceTable(mysqlConnection); // For some reason calling populateSequenceTable on the copy led to a crash, we had to run it on the constructor call like this:
		// This induced a core dump:
		wildTypeBiopolymerClassContainer-> setOriginalSequencesFromCurrentSequences();
		wildTypeBiopolymerClassContainer->loadMutationVectorsFromSequence();
		newChromosome->initialize();
		newChromosome->biopolymerClassContainer = *wildTypeBiopolymerClassContainer;
		newChromosome->setMaxMutations(0);
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                // scine breeer now only sees renumbered structure files, populateSequenceTable is now called from homologyScanner -- using an original-numbered PDB file:
	        //	newChromosome->populateSequenceTable(mysqlConnection);
                // The above was once all done here: Chromosome(0, wildTypeBiopolymerClassContainer).populateSequenceTable(mysqlConnection); 
		// But pointerizing Chromosome and wildTypeBiopolymerClassContainer made this difficult.
                std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<< std::endl;  
                if (mysqlConnection.getSequenceOnly()){
                    std::cout << __FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" You have specified -SEQUENCE . So we will only update the sequence table with the information for this PDB ID. No calculation will be done.  Exiting without error."<< std::endl; return 0; }
		delete wildTypeBiopolymerClassContainer;
		/*
		*/    
        }


        // spawnedChromosomes is an integer that counts the number of chromosomes that have been spawned. Running this procedure results in this being incremented by 0 or 1.
        int  Breed::spawnOneMutant(DBManager & mysqlConnection, int & spawnedChromosomes, const std::string myMutationString) {
		if (mysqlConnection.gromacsShouldRun()) {std::cout<<__FILE__<<":"<<__LINE__<<" You have requested a GROMACS run. This job will proceed on that basis alone."<<std::endl;}
		else if (getNumMutantsInDatabase(myMutationString , mysqlConnection ) ) {
			std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutants like "<< myMutationString  <<" already in the database is not zero! Aborting job submission."<<endl;//We will submit anyway, since the wild type equivalent may not have been run."<<std::endl; 
                        //exit(1);
			return 1;
		} 
		else {std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutants like "<< myMutationString<<" already in the database is exactly zero.  Continuing with job submission .."<<std::endl;}
		BiopolymerClassContainer wildTypeBiopolymerClassContainer;
		wildTypeBiopolymerClassContainer.loadSequencesFromPdb (mysqlConnection.getRenumberedWildTypeStructureFileName(),bool(false),"",true,true);
		Chromosome * newChromosome = new Chromosome(myMutationString, wildTypeBiopolymerClassContainer);
		//SCF ok to here
                // We are no longer using sequencesString's.  Let's just simplify and restrict to mutationString's.
			//newChromosome = Chromosome(myMutationString, wildTypeBiopolymerClassContainer); // myMutationString is a mutationString
			std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
			if (!(newChromosome->allMutationsDifferFromWildType()))	{
				std::cout<<__FILE__<<":"<<__LINE__<<" Not all mutants in this chromosome differ from original residue type "<<std::endl;		
				std::cout<<__FILE__<<":"<<__LINE__<<" Rejecting this mutant, continuing with the next. "<<std::endl;		
			        //	continue; // We will have to see if "return" results in the correct behavior..
                                return 1;
				std::cout<<__FILE__<<":"<<__LINE__<<" If this line is printed, something is wrong!"<<std::endl;	exit(1);	
			}
			std::cout<<__FILE__<<":"<<__LINE__<<" getSingleMutantID() "<< newChromosome->getSingleMutantID()<<std::endl;	
			newChromosome->setMaxMutations(0); // maxMutations shouldn't matter since we're running from a list.
			std::cout<<__FILE__<<":"<<__LINE__<<" getSingleMutantID() "<< newChromosome->getSingleMutantID()<<std::endl;	
			
		std::cout<<__FILE__<<":"<<__LINE__<<" "<<newChromosome->getSequences()<<std::endl;
		// SCF put this here, so any old entries get updated. 19 Nov 2014
		Chromosome wildTypeChromosome = *newChromosome;
		wildTypeChromosome.revertToWildTypeEquivalent();
		std::string wildTypeStringQuery = "update results set wildTypeString = \'" + wildTypeChromosome. getSingleMutantID() + "\' where mutationString = \'" + newChromosome->getSingleMutantID() + "\' and  " + mysqlConnection.jobIDAndPdbIdSelectionString() + " ;";
		std::cout<<__FILE__<<":"<<__LINE__<<" About to issue : "<<wildTypeStringQuery<<std::endl;
		mysqlConnection.insertUpdateDelete(wildTypeStringQuery);
 
                /////////////////////////////////////////////////////////////
                // This duplicates the above and should be deleted soon:
		if (mysqlConnection.gromacsShouldRun()) {
                    //std::cout<<__FILE__<<":"<<__LINE__<<" You have requested a GROMACS run. This job will proceed on that basis alone."<<std::endl;
                }
		else if (getNumMutantsInDatabase(*newChromosome , mysqlConnection ) ) {
			std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutants like "<< newChromosome->getMutationString()  <<" already in the database is not zero! This should have been caught a few lines up.."<<endl;//We will submit anyway, since the wild type equivalent may not have been run."<<std::endl; 
               std::string leafJobCompleteEmailContents = "Dear User,\n\n \
\
Thank you for using the homologyScanner web server. Your contribution not only provides you with the DDG for your protein of choice, it also provides you with the PDB IDs of structurally related and possibly useful complexes, and lastly translates your mutation to the numbering system of those other complexes.\n\n\
\
If you are getting this message, we have found a prior existing calculation relevant to your request, for the single PDB structure as named above. There may still be other PDB structures queued, and if so you will receive another email soon.\n\n\
\
If useful, this result has been added to the synopsis table. If the calculation has been done on any other homologs, these are available there as well. If there are any homologs still in the queue, their results will be added to the table when those are completed. The link to the table was sent to you in a prior email. You can also view the results in the structure viewer (click the View tab on biodesign.scilifelab.se , and select the values from the drop down lists.).\n\n Bye" ;

               system(("/usr/bin/sendemail -m \"" + leafJobCompleteEmailContents + "\"  -u \"Update: prior computed results relevant to your request were found for PDB ID: " + mysqlConnection.getPdbId() +"\" -t " + mysqlConnection.emailAddress + ",samuelfloresc@gmail.com -f sam@pe1.scilifelab.se ").c_str());
                        exit(1);
			return 1;
		} 
		else {
                    //This has already been detected a few lines up. Eventually get rid of this.
                    //std::cout<<__FILE__<<":"<<__LINE__<<" The number of mutants like "<< newChromosome->getMutationString()<<" already in the database is exactly zero.  Continuing with job submission .."<<std::endl;
                }
                /////////////////////////////////////////////////////////////

		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		if (newChromosome->validate()) {
			std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome passed validation.  Continuing with submission. "<<std::endl;
			spawn (newChromosome,mysqlConnection) ;
		/*
	       */
			spawnedChromosomes ++;
			std::cout<<__FILE__<<":"<<__LINE__<<" Have now spawned "<<spawnedChromosomes<<" chromosomes. "<<std::endl;
		} else {
			std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome failed validation for some reason.  Cannot continue with submission."<<std::endl; exit(1);
		}
               std::cout<<__FILE__<<":"<<__LINE__<<" Confirming number spawned is : "<<spawnedChromosomes<<" chromosomes. "<<std::endl;
               /*std::string leafJobCompleteEmailContents = "Dear User,\n\n \
\
Thank you for using the homologyScanner web server. Your contribution not only provides you with the DDG for your protein of choice, it also provides you with the PDB IDs of structurally related and possibly useful complexes, and lastly translates your mutation to the numbering system of those other complexes.\n\n\
\
If you are getting this message, the the effect of the  mutation you specified has been computed on a single PDB structure as named above. There may still be other PDB structures queued, and if so you will receive another email soon.\n\n\
\
This result has been added to the synopsis table. If the calculation has been done on any other homologs, these are available there as well. If there are any homologs still in the queue, their results will be added to the table when those are completed. The link to the table was sent to you in a prior email. You can also view the results in the structure viewer (click the View tab on biodesign.scilifelab.se , and select the values from the drop down lists.).\n\n Bye" ;

               system(("/usr/bin/sendemail -m \"" + leafJobCompleteEmailContents + "\"  -u \"Update: results computed for PDB ID: " + mysqlConnection.getPdbId() +"\" -t " + mysqlConnection.emailAddress + ",samuelfloresc@gmail.com -f sam@pe1.scilifelab.se ").c_str()); */
               return 0;         
        }

        // This procedure spawns jobs from a specified list.  No search algorithm of any sort is employed.
	void Breed::spawnFromList    (DBManager & mysqlConnection , const char * listFileName,ListTypeEnum myListTypeEnum ) {
		int spawnedChromosomes;
                spawnedChromosomes = 0;
                std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
                std::ifstream inFile(listFileName,std::ifstream::in); 
		if (! inFile.good()){
		    std::cout<<__FILE__<<":"<<__LINE__<<" Unable to open sequence list file: "<<     listFileName<<std::endl;
		    exit(1);
		} else {
		    std::cout<<__FILE__<<":"<<__LINE__<<" Succeeded in opening sequence list file: "<<     listFileName<<std::endl;
                }
		bool finished = false;
		// Just took this BiopolymerClassContainer instantiation and initialization out of the while loop:
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
		std::cout<<__FILE__<<":"<<__LINE__<<std::endl;

                if (myListTypeEnum == sequencesString) 
                {
                    std::cout<<__FILE__<<":"<<__LINE__<< " myListTypeEnum == sequencesString is no longer supported. Now we only support 'residues' and 'mutationString'."<<std::endl; exit(1);
		} else if (!((myListTypeEnum == mutationString)  ||
			   (myListTypeEnum == residues      ))) {
                    std::cout<<__FILE__<<":"<<__LINE__<< " We only support myListTypeEnum = 'residues' or 'mutationString'. You have specified: "<<myListTypeEnum<<std::endl; exit(1);
                }
                

		while       (!(finished))
		{
			std::string inputLine ;
			std::string tempString;
			getline(inFile, tempString);
			if (inFile.eof()) break;
			std::stringstream u;
			u.str(tempString);
			inputLine = tempString; //u.str(); // if there are multiple whitespace-delimited strings, take only the first.
			std::cout<<__FILE__<<":"<<__LINE__<<" Read sequence from file : >"<<inputLine<<"<"<<std::endl;
			if (! (inFile.good())) {
				std::cout<<__FILE__<<":"<<__LINE__<<" End of file reached, or otherwise unable to continue reading "<<listFileName<<std::endl; finished = true; }   //make sure we don't read past EOF!                                   
			else if ((inFile.eof())) {
				std::cout<<__FILE__<<":"<<__LINE__<<" End of file reached, done reading "<<listFileName<<std::endl;   //make sure we don't read past EOF!                                   
				finished = true;
			} else {
				std::cout<<__FILE__<<":"<<__LINE__<<" File "<<listFileName<<" appears to not have reached EOF."<<std::endl;
			}


                        // aminoAcidSingleLetterCodes is defined in MMB's Utils.h .. it is a vector of single letter codes for all 20 canonical amino acids.
                        String aminoAcidSingleLetterCodes[20] = {String("A"),String("R"),String("N"),String("D"),String("C"),String("Q"),String("E"),String("G"),String("H"),String("I"),String("L"),String("K"),String("M"),String("F"),String("P"),String("S"),String("T"),String("W"),String("Y"),String("V")};
                        int numMutationsAtThisPosition = 1;
			if (myListTypeEnum == residues) { numMutationsAtThisPosition = 20;} 
			else                            { numMutationsAtThisPosition = 1 ;} 
                        // For myListTypeEnum == residues, generate all possible substitutions:
                        for (int i = 0; i <numMutationsAtThisPosition; i++) // This loop is nontrivial only for case myListTypeEnum == residues.  other cases, it runs only once.
                        {
				std::string modifiedInputLine = inputLine;
                                if (myListTypeEnum == residues) {
                                     	modifiedInputLine = modifiedInputLine + "-" + aminoAcidSingleLetterCodes[i];	
				}

                                std::cout<<__FILE__<<":"<<__LINE__<<" About to spawn mutation >"<<modifiedInputLine<<"< . "<<std::endl;
                                if (spawnOneMutant(mysqlConnection,spawnedChromosomes,modifiedInputLine))  // Call routine to run only the mutant specified by the modifiedInputLine mutation string.
                                {
                                    std::cout<<__FILE__<<":"<<__LINE__<<" spawnOneMutant returned 1.  If you are doing saturation mutagenesis, this could mean that the requested mutant is the same as wild type, so we are skipping for that reason. It is also possible that the mutant "<<modifiedInputLine<<" has already been submitted to the database. See messages above. A last possibility is some sort of error. "<<std::endl;
                                } else {
                                    std::cout<<__FILE__<<":"<<__LINE__<<" spawnOneMutant completed successfully."<<std::endl;
                                }
                                /*
                                // This section should be its own procedure:
                                // This was moved down from above. For saturation mutagenesis, it would be more efficient to uncomment it from up above:
				BiopolymerClassContainer wildTypeBiopolymerClassContainer;
				wildTypeBiopolymerClassContainer.loadSequencesFromPdb (mysqlConnection.getWildTypeStructureFileName(),bool(false));
                                //
				Chromosome newChromosome;
				if (myListTypeEnum == sequencesString) {
					newChromosome = Chromosome(modifiedInputLine, 0, wildTypeBiopolymerClassContainer); // modifiedInputLine is a sequencesString.  maxMutations shouldn't matter since we're running from a list.
				} else if ((myListTypeEnum == mutationString)  ||
				       (myListTypeEnum == residues      )) {
				    newChromosome = Chromosome(modifiedInputLine, wildTypeBiopolymerClassContainer); // modifiedInputLine is a mutationString
				    std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
				    if (!(newChromosome.allMutationsDifferFromWildType()))	{
					    std::cout<<__FILE__<<":"<<__LINE__<<" Not all mutants in this chromosome differ from original residue type "<<std::endl;		
					    std::cout<<__FILE__<<":"<<__LINE__<<" Rejecting this mutant, continuing with the next. "<<std::endl;		
					    continue;
					    std::cout<<__FILE__<<":"<<__LINE__<<" If this line is printed, something is wrong!"<<std::endl;	exit(1);	
				    }
				    std::cout<<__FILE__<<":"<<__LINE__<<" getSingleMutantID() "<< newChromosome.getSingleMutantID()<<std::endl;	
				    newChromosome.setMaxMutations(0); // maxMutations shouldn't matter since we're running from a list.
				    std::cout<<__FILE__<<":"<<__LINE__<<" getSingleMutantID() "<< newChromosome.getSingleMutantID()<<std::endl;	
					
				} else {
					std::cout<<__FILE__<<":"<<__LINE__<<" Unsupported list file type! "<<std::endl; exit(1);
				}
				std::cout<<__FILE__<<":"<<__LINE__<<" "<<newChromosome.getSequences()<<std::endl;
                                // SCF put this here, so any old entries get updated. 19 Nov 2014
                                Chromosome wildTypeChromosome = newChromosome;
                                //if (chromosome.biopolymerClassContainer.get
                                wildTypeChromosome.revertToWildTypeEquivalent();
                                std::string wildTypeStringQuery = "update results set wildTypeString = \'" + wildTypeChromosome.getSingleMutantID() + "\' where mutationString = \'" + newChromosome.getSingleMutantID() + "\' and jobName = \'" + mysqlConnection.getJobID() + "\' ;";
                                std::cout<<__FILE__<<":"<<__LINE__<<" About to issue : "<<wildTypeStringQuery<<std::endl;
                                mysqlConnection.insertUpdateDelete(wildTypeStringQuery);
				if (mysqlConnection.gromacsShouldRun()) {std::cout<<__FILE__<<":"<<__LINE__<<" You have requested a GROMACS run. This job will proceed on that basis alone."<<std::endl;}
				else if (getNumSequencesInDatabase(newChromosome.getSequences(), mysqlConnection ) ) {
					std::cout<<__FILE__<<":"<<__LINE__<<" The number of sequences like "<< newChromosome.getSequences()  <<" already in the database is not zero! Going on to the next mutant in the list or loop."<<endl;//We will submit anyway, since the wild type equivalent may not have been run."<<std::endl; 
                                        continue;
				} 
				else {std::cout<<__FILE__<<":"<<__LINE__<<" The number of sequences like "<< newChromosome.getSequences()  <<" already in the database is exactly zero.  Continuing with job submission .."<<std::endl;}
				std::cout<<__FILE__<<":"<<__LINE__<<std::endl;
				if (newChromosome.validate()) {
					std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome passed validation.  Continuing with submission. "<<std::endl;
					spawn (newChromosome,mysqlConnection) ;
					spawnedChromosomes ++;
					std::cout<<__FILE__<<":"<<__LINE__<<" Have now spawned "<<spawnedChromosomes<<" chromosomes out of a total of "<< generationSize<<" for this generation"<<std::endl;
				} else {
					std::cout<<__FILE__<<":"<<__LINE__<<" The spawned chromosome failed validation for some reason.  Cannot continue with submission."<<std::endl;
				}
                                // End part that can be sent to subroutine
                                */



			
                        } // of for numMutationsAtThisPosition
			} // of while
	};


