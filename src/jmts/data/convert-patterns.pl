#!/usr/bin/perl -wT


#
# JMLib - Portable JuggleMaster Library
# Version 2.0                                                                  
# (C) Per Johan Persson 2000-2002, Gary Briggs 2003-2005
#                                                                              
# Based on JuggleMaster Version 1.60                                           
# Copyright (c) 1995-1996 Ken Matsuoka                                         
#                                                                              
# You may redistribute and/or modify JMLib under the terms of the              
# Modified BSD License as published in various places online or in the         
# COPYING.jmlib file in the package you downloaded.                            
#                                                                              
# This program is distributed in the hope that it will be useful, but          
# WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of          
# MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the              
# Modified BSD License for more details.
#



# This is another of those pieces of code that's reserving me a spot in hell.

# To use this, copy patterns.jm to the current dir, and run it. It'll
# output patterns.cs

my $numgroups = -1; # My skin burns
my $numcurrpatts = 0;
my $currstyle = "Normal";

open (INPUTFILE, '<', "patterns.jm") || die ("Need input file patterns.jm\n");
open (OUTPUTFILE, '>', "patterns.cs") || die ("Couldn't open output file patterns.cs\n");

print OUTPUTFILE '$jmpatterns = new ScriptObject(JMPatterns);';
print OUTPUTFILE "\n";
while(<INPUTFILE>) {
	if(/^\/\[\s*(.*?)\s*\]$/) {
		if($numcurrpatts != 0) {
			print OUTPUTFILE '  $jmpatterns.groups['.$numgroups.'].numpatts = '.$numcurrpatts.';';
			print OUTPUTFILE "\n";
			$numcurrpatts = 0;
		}
		$currstyle = "Normal";
		$numgroups++;
		print OUTPUTFILE ' $jmpatterns.groups['.$numgroups.'] = new ScriptObject(JMPatternGroups);';
		print OUTPUTFILE "\n";
		print OUTPUTFILE ' $jmpatterns.groups['.$numgroups.'].name = "'.$1.'";';
		print OUTPUTFILE "\n";
	} elsif(/^\%(.*)$/) {
		$currstyle = $1;
	} elsif(/^([\[\(0-9a-zA-Z]\S+)\s+(.*)$/) {
		$site = $1;
		$name = $2;
		$name =~ s/\"/\\\"/g;
		print OUTPUTFILE '   $jmpatterns.groups['.$numgroups.'].pattern['.$numcurrpatts.'] = new ScriptObject(JMPatt);';
		print OUTPUTFILE "\n";
		print OUTPUTFILE '   $jmpatterns.groups['.$numgroups.'].pattern['.$numcurrpatts.'].site = "'.$site.'";';
		print OUTPUTFILE "\n";
		if(length($name) > 0) {
			print OUTPUTFILE '   $jmpatterns.groups['.$numgroups.'].pattern['.$numcurrpatts.'].name = "'.$name.'";';
		} else {
			print OUTPUTFILE '   $jmpatterns.groups['.$numgroups.'].pattern['.$numcurrpatts.'].name = "'.$site.'";';
		}
		print OUTPUTFILE "\n";
		print OUTPUTFILE '   $jmpatterns.groups['.$numgroups.'].pattern['.$numcurrpatts.'].style = "'.$currstyle.'";';
		print OUTPUTFILE "\n";
		$numcurrpatts++;
	}
}
print OUTPUTFILE '  $jmpatterns.groups['.$numgroups.'].numpatts = '.$numcurrpatts.';';
print OUTPUTFILE "\n";

print OUTPUTFILE ' $jmpatterns.numgroups = '.($numgroups+1).';';
print OUTPUTFILE "\n\n\n";



my $style_data = "";
my $style_length = 0;
my $num_styles = 0;

seek(INPUTFILE,0,0);

print OUTPUTFILE '$jmstyles = new ScriptObject(JMStyles);';
print OUTPUTFILE "\n";

while(<INPUTFILE>) {
	if(/^\%(.*)$/) {
		if(length($style_data) > 0) {
			print OUTPUTFILE '  $jmstyles.style['.$num_styles.'] = new ScriptObject(JMStyles);';
			print OUTPUTFILE "\n";
			print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].name = "'.$currstyle.'";';
			print OUTPUTFILE "\n";
			print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].data = "'.$style_data.'";';
			print OUTPUTFILE "\n";
			print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].length = "'.$style_length.'";';
			print OUTPUTFILE "\n";
			$num_styles++;
		}
		$currstyle = $1;
		$style_length = 0;
		$style_data = "";
	} elsif(/^\{\s*(\-?\d+)\s*,\s*(\-?\d+)\s*\}\s*{\s*(\-?\d+)\s*,\s*(\-?\d+)\s*\}$/) {
		$style_length++;
		$style_data .= $1.' '.$2.' '.$3.' '.$4.' ';
	} elsif(/^\{\s*(\-?\d+)\s*,\s*(\-?\d+)\s*\}$/) {
		$style_length++;
		$style_data .= $1.' '.$2.' '.$1.' '.$2.' ';
	}
}


if(length($style_data) > 0) {
	print OUTPUTFILE '  $jmstyles.style['.$num_styles.'] = new ScriptObject(JMStyles);';
	print OUTPUTFILE "\n";
	print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].name = "'.$currstyle.'";';
	print OUTPUTFILE "\n";
	print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].data = "'.$style_data.'";';
	print OUTPUTFILE "\n";
	print OUTPUTFILE '    $jmstyles.style['.$num_styles.'].length = "'.$style_length.'";';
	print OUTPUTFILE "\n";
	$num_styles++;
}
print OUTPUTFILE '  $jmstyles.numstyles = '.$num_styles.';';
print OUTPUTFILE "\n";

close(OUTPUTFILE);
close(INPUTFILE);
