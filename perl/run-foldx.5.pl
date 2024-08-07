#!/usr/bin/perl 
use Time::localtime;
use File::Basename;

# example:
#                 0                                           1       2            3 4                                5                                                                                         6  7                   8  9        10 11  12                    13 14

print "syntax ./run-foldx.pl  <main directory>  <mutation string C:RRR:M where C = chain, RRR is residue ID (with insertion code), M is mutant residue type. concatenate mutations with '.', , e.g. C:130:L.C:149:T>   < job ID > <computeIndividualWildType> <foldx executable> <sql execution string .. 2 space-delimited elements, e.g. mysql --defaults-extra-file=/home/samuelf/svn/breeder/singleMutantFiles/option-file.mysql> <chains in mutated subunit (no spaces)> <last stage (int)> <PDB ID> <temperature> \n";

#print "syntax ./predict-affinity.pl  <main directory>  <mutated sequence in single letter code> <job ID> \n";
#//if (($ARGV[22] eq "")) {
#//	print "Not enough parameters provided!\n";
#//	exit (1);}
$complexString = "NOT-SET";
$temperature = -11111.11111;
$i = 0;
while ($i < 21){
    print "Processing arguments ",$ARGV[$i]," , ", $ARGV[$i+1],"\n";	
    if ($ARGV[$i] eq "-mainDirectory") {
        $mainDirectory = $ARGV[$i+1];	    
	print "mainDirectory is now set to $mainDirectory \n";
    }
    elsif ($ARGV[$i] eq "-mutationString") {
        $mutationString= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-jobName") {
        $jobName= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-computeIndividualWildType") {
        $computeIndividualWildType= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-foldxExecutable") {
        $foldxExecutable= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-SQLExecuteString") {
        $SQLExecuteString= "$ARGV[$i+1] $ARGV[$i+2]";
	print "SQLExecuteString is now set to $SQLExecuteString \n";
        print "This expects a double variable. incrementing i from ",$i;
        $i = $i+1;
        # to compensate for the fact that this is actually two arguments, e.g. -SQLExecuteString  /usr/bin/mysql --defaults-extra-file=/home/samuel.flores/github/breeder/singleMutantFiles/option-file.mysql 	
        print " to ", $i,"\n";
    }
    elsif ($ARGV[$i] eq "-complexString") {
        $complexString= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-myLastStage") {
        $myLastStage= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-myPdbId") {
        $myPdbId= $ARGV[$i+1]	    
    }
    elsif ($ARGV[$i] eq "-temperature") {
        $temperature= $ARGV[$i+1];    
        print "temperature just set to $temperature\n";
    }
    //elsif ($ARGV[$i] eq "-computeIndividualWildType") {
    //    $computeIndividualWildType= $ARGV[$i+1]	    
    //}	
    //elsif ($ARGV[$i] eq "-complexString") {
    //    $complexString = $ARGV[$i+1]	    
    //} 
    elsif ($ARGV[$i] eq "") {
	print "Not enough parameters provided!\n";
	exit (1);
    }
    else {
	print "Unable to parse argument : ",$ARGV[$i];
        exit(1);
    }
    print "incrementing i from ",$i;
    $i = $i+2;
    print " to ", $i,"\n";
}

print "mainDirectory set to $mainDirectory\n";
print "mutationString set to $mutationString\n";
#$jobName = $ARGV[2];
print "jobName set to $jobName\n";
#$computeIndividualWildType = $ARGV[3];
print "computeIndividualWildType set to $computeIndividualWildType\n";
#$foldxExecutable = $ARGV[4];
print "foldxExecutable set to $foldxExecutable\n";
#$SQLExecuteString =  "$ARGV[5]   $ARGV[6] ";
print "SQLExecuteString set to $SQLExecuteString\n";
#print "SQLExecuteString = $SQLExecuteString \n";
#$complexString = $ARGV[ 7];
print "complexString set to $complexString\n";
#$myLastStage = $ARGV[ 8];
print "myLastStage set to $myLastStage\n";
#$myPdbId = $ARGV[ 9];
print "myPdbId set to $myPdbId\n";
if (length($myPdbId) != 4){
    print "run-foldx : pdb ID >$myPdbId< is not 4 characters long\n";
    exit(1);
}
#$temperature = $ARGV[10];
print "temperature set to $temperature\n";
if (($temperature < 273) || ($temperature > 373)){
    print "run-foldx : temperature of $temperature is highly suspicious.\n"; 
    exit(1);
}
print "computeIndividualWildType is set to $computeIndividualWildType\n";
$singleMutantDirectory = "$mainDirectory/$mutationString";
chdir("$mainDirectory") || die ("predict-affinity.pl : couldn't chdir to $mainDirectory.  Perhaps the directory has not been created?\n");
if ($computeIndividualWildType){
 	$correspondingMutant = fileparse($mainDirectory);
print "the corresponding mutant to the wildtype $mutationString is $correspondingMutant \n";
}
if ($complexString eq "NOT-SET"){
    print "run-foldx : -complexString set to ",$complexString,". Exiting now. \n"; 
    exit(1);
} else {
    print "run-foldx : -complexString set to ",$complexString,". \n";
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
# settings of  --repair_Interface are ALL, ONLY (repair only interface), and NONE. Trying ALL first, will later do ONLY to see if I can save a bit of compute time.
# The "$foldxExecutable --command=RepairPDB --pdb=last.".$myLastStage.".pdb --repair_Interface=ALL" command outputs last_Repair.pdb .
# The "$foldxExecutable --command=Optimize --pdb=last_Repair.pdb" outputs Optimized_last_Repair.pdb .
# I couldn't get it to include $myLastStage as part of the file name because of the '.' .
#$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable --command=RepairPDB --pdb=last.".$myLastStage.".pdb --repair_Interface=ALL > $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=Optimize --pdb=last_Repair.pdb >> $singleMutantDirectory/foldx.$mutationString.out ; $foldxExecutable --command=AnalyseComplex --pdb=Optimized_last_Repair.pdb --analyseComplexChains=$complexString >> $singleMutantDirectory/foldx.$mutationString.out ";
#modified to use the cyxToCys PDB:
# set to  --repair_Interface=ONLY and skipping Optimize:
# Overrode a lot of recent changes to match SKEMPIMN-0ps :
# this program differs from run-foldx.SKEMPIMN.pl  only in that it uses $cyxToCysPdb rather than last.$myLastStage.pdb as a starting point:
print "############################\n";
print "# Part 1 : Here we compute  DDG\n";
print "############################\n";
$runFoldXCommand = "cd $singleMutantDirectory; $foldxExecutable --command=AnalyseComplex --pdb=$cyxToCysPdb --analyseComplexChains=$complexString  >> $singleMutantDirectory/foldx.$mutationString.out ";
print "About to issue runFoldXCommand = $runFoldXCommand \n";
system($runFoldXCommand);
print "done with runFoldXCommand  \n";
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
$mysqlString = "echo \"update results set foldx_energy_wild_type = $foldx_energy, status = \\\"WT completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\" and mutationString = \\\"$correspondingMutant\\\" and complexString  = \\\"$complexString\\\"    ;\" | $SQLExecuteString  >  $singleMutantDirectory/$mutationString.mysql ";
}
else{
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
$mysqlString = "echo \"update results set foldx_energy = $foldx_energy, status = \\\"completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\"   and mutationString = \\\"$mutationString\\\" and complexString  = \\\"$complexString\\\" ;\" | $SQLExecuteString  &>  $singleMutantDirectory/$mutationString.mysql ";
}
print $mysqlString,"\n";
system($mysqlString);

print "############################\n";
print "# Part 2 : Here we compute monomer stability, rather than DDG\n";
print "############################\n";

$runFoldXStabilityCommand = "cd $singleMutantDirectory; $foldxExecutable --command=Stability      --pdb=$cyxToCysPdb  >> $singleMutantDirectory/foldx_stability.$mutationString.out ";
print "About to issue runFoldXStabilityCommand = $runFoldXStabilityCommand \n";
system($runFoldXStabilityCommand);
print "done with runFoldXStabilityCommand  \n";
# the -a flag tells grep to treat the file as text. Probably there is a NUL character somewhere that otherwise made it treated as binary
$foldx_stability = `grep -a Total $singleMutantDirectory/foldx_stability.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' `;
chomp($foldx_stability);
if (length($foldx_stability) == 0) {
	print "foldx_stability has zero length!\n";
	exit(1);	
}
system("grep -a Total $singleMutantDirectory/foldx_stability.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' > $singleMutantDirectory/foldx_stability.stability.$mutationString");
$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
if ($computeIndividualWildType){
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
$mysqlString = "echo \"update results set foldx_stability_wild_type = $foldx_stability, status = \\\"WT completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\" and mutationString = \\\"$correspondingMutant\\\" and complexString  = \\\"$complexString\\\" ;\" | $SQLExecuteString  >  $singleMutantDirectory/$mutationString.mysql ";
}
else{
# Turns out on akka for some reason the &> redirect does not work. Leads instead to output being dumped to what looks like stdout (or maybe stderr). Using just > redirect instead.
$mysqlString = "echo \"update results set foldx_stability = $foldx_stability, status = \\\"completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and pdbId =  \\\"$myPdbId\\\"   and mutationString = \\\"$mutationString\\\" and complexString  = \\\"$complexString\\\"  ;\" | $SQLExecuteString  &>  $singleMutantDirectory/$mutationString.mysql ";
}
print $mysqlString,"\n";
system($mysqlString);
print "Done with run-foldx script \n";

