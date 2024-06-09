cmake_minimum_required(VERSION 2.6)
project(BREEDER )


SET(ENABLE_MYSQL FALSE CACHE BOOL
        "Allow connecting to mysql database")
#set(CMAKE_CXX_FLAGS " -Wall")
# was, for debug:
set(CMAKE_CXX_FLAGS "-g -Wall")
set(BREEDER_SOURCE_DIR "/bubo/home/h17/samuelf/svn/breeder" CACHE PATH "Directory for breeder source" )
set(MMB_SOURCE_DIR "/bubo/home/h17/samuelf/svn/RNAToolbox/trunk" CACHE PATH "Directory for MMB source")
set(MMB_LIB "/bubo/home/h17/samuelf/svn/RNAToolbox/trunk/build/libMMBlib.so" CACHE PATH "MMB binary")
set(MMB_LIB_D
"/bubo/home/h17/samuelf/svn/RNAToolbox/trunk/build/libMMBlib_d.so" CACHE PATH
"MMB debug binary")
set(CMAKE_BUILD_TYPE "Debug"       )
set(SimTK_SOURCE_DIR "/bubo/home/h17/samuelf/SimTK"       )
set(SimTK_INCLUDE_DIR
"/bubo/home/h17/samuelf/SimTK/include;/bubo/home/h17/samuelf/SimTK/include/simbody"       )
set(MySQL_INCLUDE_DIR "/usr/include/mysql"   CACHE PATH "Directory for mysql.h and other mysql C API headers") #/usr/include/mysql
set(SQLite_INCLUDE_DIR "/sw/apps/sqlite/3.8.5/tintin/include"   CACHE PATH "Directory for mysql.h and other mysql C API headers") #/usr/include/mysql
set(Boost_INCLUDE_DIR "/usr/include/boost"   CACHE PATH "Directory for boost") 
set(Seqan_INCLUDE_DIR "/home/samuelf/seqan-library-1.4.1/include/"   CACHE PATH "Directory for boost") 
set(MySQL_LIB "/usr/lib64/mysql/libmysqlclient.so" CACHE PATH "C API binary"  ) 
if (ENABLE_MYSQL)
    add_definitions(-DENABLE_MYSQL) 
    set (MYSQL_SOURCE  ${BREEDER_SOURCE_DIR}/src/MysqlConnection.cpp)
ELSE ()
    set(MySQL_LIB "")
    set (MYSQL_SOURCE  "")
ENDIF()
set(SQLite_LIB "/usr/lib64/libsqlite3.so" CACHE PATH "C API binary"  ) 
SET(SimTK_SHARED_LIBS 
                optimized SimTKmolmodel 
                optimized SimTKsimbody
                optimized SimTKmath
                optimized SimTKcommon CACHE STRING "simtk libraries")
# for some reason this never worked:
SET(SimTK_SHARED_LIBS_D 
                debug SimTKmolmodel_d 
                debug SimTKsimbody_d
                debug SimTKmath_d
                debug SimTKcommon_d CACHE STRING "simtk debug libraries")
# These extra libraries are only available as a shared, optimized.
IF(WIN32)
        SET(SimTK_GENERAL_LIBS SimTKlapack pthreadVC2 ws2_32)
ELSEIF(APPLE)
        SET(SimTK_GENERAL_LIBS lapack blas)
ELSE() #Linux
        SET(SimTK_GENERAL_LIBS lapack blas dl rt)
ENDIF()






#simbody
#SET (SimTK_INSTALL_DIR 
#ESET(SimTKsimbody $ENV{SimTK_INSTALL_DIR} CACHE PATH
SET(SimTKsimbody $SimTK_SOURCE_DIR CACHE PATH
    "Directory where SimTK simbody is installed, e.g. /usr/local/SimTK.")
IF(NOT SimTKsimbody)
    MESSAGE(FATAL_ERROR
"Expected SimTK_SOURCE_DIR environment var to be set, or set SimTKsimbody in CMake")
#"Expected SimTK_INSTALL_DIR environment var to be set, or set SimTKsimbody in CMake")
ENDIF(NOT SimTKsimbody)

SET(OpenMM_INCLUDE_DIR "" CACHE PATH
        "Where to find OpenMM")
SET(OpenMM_INSTALL_DIR "" CACHE PATH
        "OpenMM install directory. Used to find libs.")
#Add the OpenMM library
SET(OpenMM_SHARED_LIBS "/bubo/home/h17/samuelf/openmm/lib/libOpenMM.so" CACHE
PATH "OpenMM binary")
#        optimized OpenMM)
SET(OpenMM_SHARED_LIBS_D "/bubo/home/h17/samuelf/openmm/lib/libOpenMM_d.so"
CACHE PATH "OpenMM debug binary")
        #debug OpenMM_d)


#check for MMB
IF(NOT MMB_LIB)
        MESSAGE(FATAL_ERROR
"Expected MMB_LIB environment var to be set, or set
MMB_LIB in CMake")
ENDIF(NOT MMB_LIB)

IF(NOT MMB_SOURCE_DIR)
        MESSAGE(FATAL_ERROR
"Expected MMB_SOURCE_DIR environment var to be set, or set
MMB_SOURCE_DIR in CMake")
ENDIF(NOT MMB_SOURCE_DIR)


# Check for SQLite
IF(NOT SQLite_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR
"Expected SQLite_INCLUDE_DIR environment var to be set, or set
SQLite_INCLUDE_DIR in CMake")
ENDIF(NOT SQLite_INCLUDE_DIR)
IF(NOT SQLite_LIB)
        MESSAGE(FATAL_ERROR
"Expected SQLite_LIB environment var to be set, or set
SQLite_LIB in CMake")
ENDIF(NOT SQLite_LIB)
# Check for MySQL
IF(NOT MySQL_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR
"Expected MySQL_INCLUDE_DIR environment var to be set, or set
MySQL_INCLUDE_DIR in CMake")
ENDIF(NOT MySQL_INCLUDE_DIR)
IF((NOT MySQL_LIB ) AND ENABLE_MYSQL )
        MESSAGE(FATAL_ERROR
"Expected MySQL_LIB environment var to be set, or set
MySQL_LIB in CMake")
ENDIF() #(NOT MySQL_LIB)
SET(SimTK $SimTK_SOURCE_DIR CACHE PATH
#SET(SimTK $SimTK_SOURCE_DIR$ENV{SimTK_INSTALL_DIR} CACHE PATH
#SET(SimTK "/bubo/home/h17/samuelf/SimTK" CACHE PATH
        "Directory where SimTK is installed, e.g. /usr/local/SimTK.")
IF(NOT SimTK)
        MESSAGE(FATAL_ERROR
"Expected $SimTK_SOURCE_DIR  environment var to be set, or set SimTK in CMake")
#"Expected SimTK_INSTALL_DIR environment var to be set, or set SimTK in CMake")
ENDIF(NOT SimTK)
SET(LIB64)
#IF( UNIX AND NOT APPLE  )
IF (UNIX AND NOT APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( LIB64 64 CACHE PATH  "If this should be 64, make sure it does so here")
#ENDIF(UNIX AND NOT APPLE  )
ENDIF (UNIX AND NOT APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 8 )
    SET( LIB64 64 CACHE PATH  "If this should be 64, make sure it does so here")
# On Mac, build 32 bit binaries.
IF(APPLE)
    SET(CMAKE_OSX_ARCHITECTURES "i386" CACHE STRING
            "The processor architectures to build for" )
ENDIF(APPLE)
# On Mac, use Xcode by default
IF(APPLE)
    SET(CMAKE_GENERATOR "Xcode" CACHE STRING "Since you on a Mac, we assume
you want to use Xcode"  )
ELSEIF(UNIX)
    SET(CMAKE_GENERATOR "Unix Makefiles" CACHE STRING "Since you on Unix, we
assume you want to use a Makefile" )
ENDIF()




INCLUDE_DIRECTORIES(
	${SimTK}/include
	${SimTK}/include/simbody
	${MMB_SOURCE_DIR}/include 
   	${MySQL_INCLUDE_DIR}
   	${SQLite_INCLUDE_DIR}
   	${Boost_INCLUDE_DIR}
   	${Seqan_INCLUDE_DIR}
	${BREEDER_SOURCE_DIR}/include 
        ${OpenMM_INCLUDE_DIR}
	)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(breeder ${BREEDER_SOURCE_DIR}/src/main.cpp 
    ${BREEDER_SOURCE_DIR}/src/Chromosome.cpp
    // trip some sort of error. because this cmake does not create a library!
    //${BREEDER_SOURCE_DIR}/src/BreederParameterReader.cpp
    ${BREEDER_SOURCE_DIR}/src/DBManager.cpp      
    ${BREEDER_SOURCE_DIR}/src/SQLite.cpp         
    ${MYSQL_SOURCE}
    #${BREEDER_SOURCE_DIR}/src/MysqlConnection.cpp         
    #${BREEDER_SOURCE_DIR}/src/MysqlConnection.cpp
    ${BREEDER_SOURCE_DIR}/src/Breed.cpp 
    )

LINK_DIRECTORIES(${SimTKsimbody}/lib${LIB64} ${SimTK}/lib${LIB64}
#/bubo/home/h17/samuelf/SimTK/lib64
${CMAKE_BINARY_DIR} ${OpenMM_INSTALL_DIR}/lib )
TARGET_LINK_LIBRARIES(breeder

#${SimTK_SHARED_LIBS_D}
${SimTK_SHARED_LIBS}
#${MMB_LIB_D}
${MMB_LIB}
${MySQL_LIB}
${SQLite_LIB}
#${OpenMM_SHARED_LIBS_D}
${OpenMM_SHARED_LIBS}
)

    # Install files
    INSTALL(TARGETS breeder
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    )
