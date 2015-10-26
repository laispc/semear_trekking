#!/usr/bin/perl

#This program shows how to calculate rotation angle
#Lines 34-41 should be used in our robot

use Math::Trig;
use Math::Trig ':pi';

my $point = {1=>[1,0],
			 2=>[1,1],
			 3=>[0,1],
			 4=>[-1,1],
			 5=>[-1,0],
			 6=>[-1,-1],
			 7=>[0,-1],
			 8=>[1,-1]
			 };		#coords of the points

my $xc = 0;	my $yc = 0;		#initial position
my $ph = 0;				#[deg] compass along boarder for calibration

my $a = 359;			#[deg] compass at current position

my $g;					#[deg] rotation angle
my $L;					#distance to control point
foreach (1..8) {

	my $xp = $point->{$_}[0];
	my $yp = $point->{$_}[1];
	my $dx = $xp - $xc;
	my $dy = $yp - $yc;
	
	my $w;
	$w = atan($dy/$dx)*180/pi if $dx!=0;
	$w = 90 if ($dx>-1e-10 and $dx<1e-10 and $yp>=0);
	$w = -90 if ($dx>-1e-10 and $dx<1e-10 and $yp<0); 
	$g = -($a-$ph-$w) if (($dx>=0 and $dy>=0) or ($dx>=0 and $dy<=0));	# w in I or IV
	$g = -($a-$ph-$w-180) if (($dx<0 and $dy>=0) or ($dx<0 and $dy<0));	# w in II or III
	$g=$g-360 if $g>180;
	$g=$g+360 if $g<-180;
	$L=sqrt($dx**2+$dy**2);
	print "$_) dx=$dx, dy=$dy, w=$w, a=$a: \t g=$g, L=$L $/";
}
