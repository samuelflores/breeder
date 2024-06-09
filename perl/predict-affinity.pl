#!/usr/bin/perl 
use Time::localtime;
print "syntax ./predict-affinity.pl  <main directory>  <mutated sequence in single letter code> \n";
#if (! ($ARGV[0])) {exit (1);}
if (! ($ARGV[1])) {exit (1);}
if ( ($ARGV[2])) {exit (1);}

$firstResidue = 124;
$lastResidue = 167;
$includeRadius = 1.2;
$mainDirectory = $ARGV[0];
$sequence = $ARGV[1];
chdir("$mainDirectory") || die ("predict-affinity.pl : couldn't chdir to $mainDirectory.  Perhaps the directory has not been created?\n");

$jobName = $sequence; #// ""; #$mutationChain[0]."_".$mutationPosition[0]."_".$mutantResidueType[0];


$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
print "You are starting this job on $year-$month-$day.\n";

system("rm -Rf $mainDirectory/$jobName");
system("mkdir $mainDirectory/$jobName");
if (!((-e "$mainDirectory/commands.except-chains.dat")  &&
      (-e "$mainDirectory/parameters.csv")  &&
      (-e "$mainDirectory/last.1.pdb")  &&
      (-e "$mainDirectory/runfile.txt")  &&
      (-e "$mainDirectory/rotabase.txt")  &&
      (-e "$mainDirectory/commands.except-chains.dat")) )  {
    
    print "Unable to find a required file in $mainDirectory.\n"; exit(1);
}

system("cat $mainDirectory/commands.except-chains.dat > $mainDirectory/$jobName/commands.$jobName.dat");
#system("echo \"loadSequencesFromPdb \" >> $mainDirectory/$jobName/commands.$jobName.dat");
system("echo \"protein C 124 $sequence\" >>  $mainDirectory/$jobName/commands.$jobName.dat");
system("echo \"protein D 238 PSVFLFPPKPKDTLMISRTPEVTCVVVDVSHEDPQVKFNWYVDGVQVHNAKTKPREQQYNSTYRVVSVLTVLHQNWLDGKEYKCKVSNKALPAPIEKTISKAKGQPREPQVYTLPPSREEMTKNQVSLTCLVKGFYPSDIAVEWESNGQPENNYKTTPPVLDSDGSFFLYSKLTVDKSRWQQGNVFSCSVMHEALHNHYTQKSLSL\" >>  $mainDirectory/$jobName/commands.$jobName.dat");

#system("echo \"constrainRigidSegments true\" >> $mainDirectory/$jobName/commands.$jobName.dat");
system(" echo \"removeRigidBodyMomentum FALSE\" >> $mainDirectory/$jobName/commands.$jobName.dat");
system("echo \"mobilizer Rigid\" >> $mainDirectory/$jobName/commands.$jobName.dat"); # rigidifies all chains in system.
$start = -1111;
$end   = -1111;


print "check 2\n";
system("echo \"mobilizer Rigid\" >> $mainDirectory/$mainDirectory/$jobName/commands.$jobName.dat");


system("cd $mainDirectory/$jobName");
chdir("$mainDirectory/$jobName");
#system(" /Users/Sam/svn/RNAToolbox/trunk/build/MMB   ");# && die ("Unable to run RNABuilder\n");

system("echo \"use samuelf; update results set  status = \\\"running\\\" where sequence = \\\"$sequence\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys ");

$outputStructureFile = "$jobName.pdb";
system("cp $mainDirectory/last.1.pdb   $mainDirectory/$jobName/");
system("cp $mainDirectory/parameters.csv   $mainDirectory/$jobName/");
system("cp $mainDirectory/rotabase.txt     $mainDirectory/$jobName/");
system("cd \"$mainDirectory/$jobName\" ; echo  \"$mainDirectory/$jobName\";  export LD_LIBRARY_PATH=/bubo/home/h17/samuelf/svn/RNAToolbox/trunk/distributions/Installer.2_11.CentOS64 ;  /bubo/home/h17/samuelf/svn/RNAToolbox/trunk/distributions/Installer.2_11.CentOS64/MMB.2_11.CentOS64   -c  $mainDirectory/$jobName/commands.$jobName.dat -D $mainDirectory/$jobName   ");# && die ("Unable to run RNABuilder\n");
system("cp $mainDirectory/$jobName/last.2.pdb $mainDirectory/$jobName/$outputStructureFile");
system("echo \"$outputStructureFile\" > $mainDirectory/$jobName/batch.txt");
system("mv trajectory.2.pdb trajectory.$jobName.pdb");

system("/bubo/home/h17/samuelf/local/FoldX.linux64 -runfile $mainDirectory/runfile.txt > $mainDirectory/$jobName/foldx.$jobName.out ");
$foldx_energy = `grep Total $mainDirectory/$jobName/foldx.$jobName.out | grep = | tail -1 | awk \'\{print \$3\}\' `;
chomp($foldx_energy);
system("grep Total $mainDirectory/$jobName/foldx.$jobName.out | grep = | tail -1 | awk \'\{print \$3\}\' > $mainDirectory/$jobName/foldx_energy.$jobName");
#system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\" where sequence = \\\"$sequence\\\";\"   > $mainDirectory/$jobName/$jobName.mysql");
$tm=localtime;
my ($day,$month,$year)=($tm->mday,$tm->mon,$tm->year);
$year += 1900;
$month += 1;
system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\", jobName = \\\"$mainDirectory\\\" , date = \\\"$year-$month-$day\\\"  where sequence = \\\"$sequence\\\";\" > $mainDirectory/$jobName/$jobName.mysql");
system("echo \"use samuelf; update results set foldx_energy = $foldx_energy, status = \\\"completed\\\", jobName = \\\"$mainDirectory\\\" , date = \\\"$year-$month-$day\\\"  where sequence = \\\"$sequence\\\";\" | /usr/bin/mysql -h mysql.uppmax.uu.se -u samuelf -poirfy235k4ys ");
