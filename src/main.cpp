#include <sys/types.h>
#include "DBManager.h"           
#include "SQLite.h"           
#include "MysqlConnection.h"
#include <sys/stat.h>
#include "Breed.h"
#include "BreederParameterReader.h"
#include <iostream>
#include "ResidueStretchContainer.h"
//#include "/home/samuelf/svn/RNAToolbox/trunk/include/ResidueStretchContainer.h"



int main(int num_args, char *args[]){ 
    BreederParameterReader * breederParameterReader = new BreederParameterReader();    
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    breederParameterReader->setDefaults ();
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    breederParameterReader->printUsage();
    breederParameterReader->readParameters (num_args, args);
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    breederParameterReader->validate ();  
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    breederParameterReader->updateDatabase();
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    return breederParameterReader->runBreeder();
    std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<std::endl;
    delete breederParameterReader;
};