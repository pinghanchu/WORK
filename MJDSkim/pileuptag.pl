#!/usr/bin/perl
print "Please input [dataset]\n";
$numArgs = $#ARGV + 1;
if( $numArgs < 1){
    print "You miss arguments\n";
}elsif( $numArgs >1){
    print "You have too many arguments\n";
}
my $scriptpath = "/global/projecta/projectdirs/majorana/users/pchu/git/MJDSoftware/MJDSkim/";
my $search = $scriptpath."pileuptag";
print $search,"\n";
my $dataset = $ARGV[0];
my $subset= 0;
if($dataset == 0){
    $subset = 75;
}elsif( $dataset == 1){
    $subset = 51;
}elsif( $dataset == 2){
    $subset = 7;
}elsif( $dataset == 3){
    $subset = 24;
}elsif( $dataset == 4){
    $subset = 18;
}elsif( $dataset == 5){
    $subset = 112;
}
for(my $i=0;$i<=$subset;$i++){
    my $file = $dataset."_".$i;
    my $app  = "pileup_".$file.".csh";
    open(my $fh, ">", $app) or die "cannot open";#
    print $fh "#!/bin/tcsh\n";
    print $fh "$search $dataset $i 0\n";   
    close $fh;
    system("chmod 755 $app");
    system("qsub -l projectio=1 -cwd -o out.$file -e err.$file $app");
}
