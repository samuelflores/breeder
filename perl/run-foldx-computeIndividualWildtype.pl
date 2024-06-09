#!/usr/bin/perl 
use Time::localtime;
use File::Basename;
print "syntax ./run-foldx.pl  <main directory>  <mutation string C:RRR:M where C = chain, RRR is residue ID (with insertion code), M is mutant residue type. concatenate mutations with '.', , e.g. C:130:L.C:149:T>   < job ID >\n"; 
#print "syntax ./predict-affinity.pl  <main directory>  <mutated sequence in single letter code> <job ID> \n";
if (! ($ARGV[0])) {
	print "Not enough parameters provided!\n";
	exit (1);}
if (! ($ARGV[1])) {
	print "Not enough parameters provided!\n";
	exit (1);}
if (! ($ARGV[2])) {
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
$singleMutantDirectory = "$mainDirectory/$mutationString";
chdir("$mainDirectory") || die ("predict-affinity.pl : couldn't chdir to $mainDirectory.  Perhaps the directory has not been created?\n");
my $correspondingMutantForThisWildtype = fileparse($mainDirectory);
print "the corresponding mutant to the wildtype $mutationString is $correspondingMutantForThisWildtype \n";

#$jobName = $sequence; #// ""; #$mutationChain[0]."_".$mutationPosition[0]."_".$mutantResidueType[0];
$runFoldXCommand = "cd $singleMutantDirectory; /bubo/home/h17/samuelf/local/FoldX.linux64 -runfile $singleMutantDirectory/runfile.txt &> $singleMutantDirectory/foldx.$mutationString.out ";
print $runFoldXCommand,"\n";
system($runFoldXCommand);

#system("/bubo/home/h17/samuelf/local/FoldX.linux64 -runfile $mainDirectory/runfile.txt > $singleMutantDirectory/foldx.$mutationString.out ");
$foldx_energy = `grep Total $singleMutantDirectory/foldx.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' `;
chomp($foldx_energy);
if (length($foldx_energy) == 0) {
	print "foldx_energy has zero length!\n";
	exit(1);	
}
system("grep Total $singleMutantDirectory/foldx.$mutationString.out | grep = | tail -1 | awk \'\{print \$3\}\' > $singleMutantDirectory/foldx_energy.$mutationString");
#system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\" where mutationString = \\\"$mutationString\\\";\"   > $singleMutantDirectory/$mutationString.mysql");
$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
$mysqlString = "echo \"use samuelf; update results set foldx_energy_wild_type = $foldx_energy, status = \\\"completed\\\" , date = \\\"$year-$month-$day\\\"  where jobName = \\\"$jobName\\\" and mutationString = \\\"$correspondingMutantForThisWildtype\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys &>  $singleMutantDirectory/$mutationString.mysql ";
print $mysqlString,"\n";
system($mysqlString);
#system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\", jobName = \\\"$mainDirectory\\\" , date = \\\"$year-$month-$day\\\"  where mutationString = \\\"$mutationString\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys ");