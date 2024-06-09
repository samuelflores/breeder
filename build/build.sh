#!/bin/bash
g++ ../src/BreederParameterReader.cpp ../src/Breed.cpp ../src/Chromosome.cpp ../src/DBManager.cpp ../src/MysqlConnection.cpp ../src/SQLite.cpp ../src/main.cpp -fPIC -shared -o ./libBreeder.so -I/home/samuel.flores/github/breeder/include -I/usr/local//include/ -isystem /usr/local//include/simbody -I/usr/local//include -I/usr/local//include/openmm -I/usr/local//include/openmm/reference -I /usr/local//seqan-library-2.2.0/include/ -I /home/samuel.flores/github/MMB/include/ -I /usr/include/mysql -I /usr/include -std=c++14 -D ENABLE_MYSQL -D USE_OPENMM -D USE_OPENMM_REALVEC
sudo mv libBreeder.so /usr/local/lib/

g++ -std=c++14 -o breeder -Wl,-rpath,/home/sflores/github/RNAToolbox/trunk/build/ -L/usr/local/MMB/lib/ -lSimTKmolmodel -L /usr/local/MMB/lib64/ -l SimTKsimbody -L //usr/lib/mysql/lib/ -L /usr/lib64 -L /home/samuel.flores/github/breeder/build -L /usr/local/lib -l Breeder -l MMB -l OpenMM -l sqlite3 -l mysqlclient -D USE_OPENMM -D USE_OPENMM_REALVEC
sudo mv breeder /usr/local//bin/breeder 
