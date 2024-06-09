#!/usr/bin/perl 
use Time::localtime;
use File::Basename;

# example:
#                 0                                           1       2            3 4                                5                                                                                         6  7                   8  9        10 11  12                    13 14
 # ./run-foldx.pl /home/samuelf/projects/1FC2.domainZ/C-123-A C-123-K 1FC2.domainZ 1 /home/samuelf/local/foldx4/foldx /sw/easybuild/software/Compiler/GCC/4.9.3-binutils-2.25/MySQL/5.6.26-clientonly/bin/mysql -h rembrandt.bmc.uu.se -u mmbadmin -D mmb -pMMB37xYstRhggY78432 C 2 1FC2

print "syntax ./run-foldx.pl  <main directory>  <mutation string C:RRR:M where C = chain, RRR is residue ID (with insertion code), M is mutant residue type. concatenate mutations with '.', , e.g. C:130:L.C:149:T>   < job ID > <computeIndividualWildType> <foldx executable> <sql execution string .. 2 space-delimited elements, e.g. mysql --defaults-extra-file=/home/samuelf/svn/breeder/singleMutantFiles/option-file.mysql> <chains in mutated subunit (no spaces)> <last stage (int)> <PDB ID> <temperature> \n";

#print "syntax ./run-foldx.pl  <main directory>  <mutation string C:RRR:M where C = chain, RRR is residue ID (with insertion code), M is mutant residue type. concatenate mutations with '.', , e.g. C:130:L.C:149:T>   < job ID > <computeIndividualWildType>\n"; 
#print "syntax ./predict-affinity.pl  <main directory>  <mutated sequence in single letter code> <job ID> \n";
if (($ARGV[8 ] eq "")) {
	print "Not enough parameters provided!\n";
	exit (1);}

#$firstResidue = 124;
#$lastResidue = 167;
#$includeRadius = 1.2;
$mainDirectory = $ARGV[0];
print "mainDirectory set to $mainDirectory\n";
$mutationString = $ARGV[1];
print "mutationString set to $mutationString\n";
$jobName = $ARGV[2];
print "jobName set to $jobName\n";
$computeIndividualWildType = $ARGV[3];
$foldxExecutable = $ARGV[4];
$SQLExecuteString =  "$ARGV[5]   $ARGV[6] ";#    $ARGV[7]   $ARGV[8]   $ARGV[9]   $ARGV[10]   $ARGV[11]   $ARGV[12] ";
print "SQLExecuteString = $SQLExecuteString \n";
$chainsInMutatedSubunit = $ARGV[ 7];
$myLastStage = $ARGV[ 8];
$myPdbId = $ARGV[ 9];
if (length($myPdbId) != 4){
    print "run-foldx : pdb ID >$myPdbId< is not 4 characters long\n";
    exit(1);
}
$temperature = $ARGV[10];
if (($temperature < 273) || ($temperature > 373)){
    print "run-foldx : temperature of $temperature is highly suspicious.\n"; 
    exit(1);
}
#$SQLExecutable = $ARGV[5];
#$database        = $ARGV[6];
print "computeIndividualWildType is set to $computeIndividualWildType\n";
$singleMutantDirectory = "$mainDirectory/$mutationString";
chdir("$mainDirectory") || die ("predict-affinity.pl : couldn't chdir to $mainDirectory.  Perhaps the directory has not been created?\n");
if ($computeIndividualWildType){
 	$correspondingMutant = fileparse($mainDirectory);
print "the corresponding mutant to the wildtype $mutationString is $correspondingMutant \n";
}
# not needed. we are now doing the C to X substitution in MMB. I believe 'sed' command was not understood, probably needs full path.
#system("cd $singleMutantDirectory; sed s/CYX/CYS/ last.2.pdb > temp.pdb; echo 'temp.pdb' > batch.txt");
# Turns out this is still needed. We need to translate back to CYS so FoldX understands:
$cyxToCysStem = "lastCyxToCys";
$cyxToCysPdb = "$cyxToCysStem.pdb";
$cyxToCysRepairPdb = $cyxToCysStem."_Repair.pdb";
$cyxToCysOptimizedPdb = "Optimized_".$cyxToCysRepairPdb;
$cyxToCysCommand = "cd $singleMutantDirectory; sed s/CYX/CYS/ last.$myLastStage.pdb > $cyxToCysPdb";
print "$cyxToCysCommand \n";
system($cyxToCysCommand);
#
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
#$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable -runfile $singleMutantDirectory/runfile.txt > $singleMutantDirectory/foldx.$mutationString.out ";
# settings of  --repair_Interface are ALL, ONLY (repair only interface), and NONE. Trying ALL first, will later do ONLY to see if I can save a bit of compute time.
# The "$foldxExecutable --command=RepairPDB --pdb=last.".$myLastStage.".pdb --repair_Interface=ALL" command outputs last_Repair.pdb .
# The "$foldxExecutable --command=Optimize --pdb=last_Repair.pdb" outputs Optimized_last_Repair.pdb .
# I couldn't get it to include $myLastStage as part of the file name because of the '.' .
#$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable --command=RepairPDB --pdb=last.".$myLastStage.".pdb --repair_Interface=ALL > $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=Optimize --pdb=last_Repair.pdb >> $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=AnalyseComplex --pdb=Optimized_last_Repair.pdb --analyseComplexChains=$chainsInMutatedSubunit >> $singleMutantDirectory/foldx.$mutationString.out ";
#modified to use the cyxToCys PDB:
# set to  --repair_Interface=ONLY and skipping Optimize:
# Overrode a lot of recent changes to match SKEMPIMN-0ps :
$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable --command=RepairPDB --pdb=last.$myLastStage.pdb --repair_Interface=ALL     > $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable  --command=Optimize --pdb=last_Repair.pdb >> $singleMutantDirectory/foldx.$mutationString.out ;   $foldxExecutable --command=AnalyseComplex --pdb=Optimized_last_Repair.pdb --analyseComplexChains=$chainsInMutatedSubunit   >> $singleMutantDirectory/foldx.$mutationString.out ";
#$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable --command=RepairPDB --pdb=$cyxToCysPdb --repair_Interface=ALL > $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=Optimize --pdb=$cyxToCysRepairPdb >> $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=AnalyseComplex --pdb=$cyxToCysOptimizedPdb --analyseComplexChains=$chainsInMutatedSubunit >> $singleMutantDirectory/foldx.$mutationString.out "u
print "check 1\n";
print "runFoldXCommand = $runFoldXCommand \n";
#exit;
system($runFoldXCommand);
# the -a flag tells grep to treat the file as text. Probably there is a NUL character somewhere that otherwise made it treated as binary
$foldx_energy = `grep -a Total $singleMutantDirectory/foldx.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' `;
chomp($foldx_energy);
if (length($foldx_energy) == 0) {
	print "foldx_energy has zero length!\n";
	exit(1);	
}
system("grep -a Total $singleMutantDirectory/foldx.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' > $singleMutantDirectory/foldx_energy.$mutationString");
$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
if ($computeIndividualWildType){
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
$mysqlString = "echo \"update results set foldx_energy_wild_type = $foldx_energy, status = \\\"WT completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\" and mutationString = \\\"$correspondingMutant\\\";\" | $SQLExecuteString  >  $singleMutantDirectory/$mutationString.mysql ";
#$mysqlString = "echo \"update results set foldx_energy_wild_type = $foldx_energy, status = \\\"WT completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and mutationString = \\\"$correspondingMutant\\\";\" | $SQLExecutable $database  &>  $singleMutantDirectory/$mutationString.mysql ";
}
else{
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
$mysqlString = "echo \"update results set foldx_energy = $foldx_energy, status = \\\"completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\"   and mutationString = \\\"$mutationString\\\";\" | $SQLExecuteString  &>  $singleMutantDirectory/$mutationString.mysql ";
}
#$fileLocked = 1;
#while ($fileLocked) {
    # If $database is locked for updating, then $fileLocked > 0.  Check this now: 
    #$fileLocked = `fuser -vm $database  2>&1 | grep F | wc -l`;
    #chomp($fileLocked);
    # If $fileLocked > 0, then we will issue a warning and wait a bit before trying again.  Otherwise, we will run mysqlString and continue.
    #if ($fileLocked > 0) {
    #    print "run-foldx.pl : The database file $database appears to be locked. Waiting a bit before trying again.\n";
    #    print "output of fuser -vm $database :\n";
    #    print `fuser -vm $database`,"\n";;
    #    sleep(60); # number of seconds to sleep
    #} else {
    #    print "run-foldx.pl : The database file $database appears to be ready for writing.  Writing now and then moving on..\n";
    #}
    print $mysqlString,"\n";
    system($mysqlString);
    #system($mysqlString); # Am having a recurring problem of all execution proceeding perfectly except results are not logged to results table. Let's see if calling this twice helps.
#}
