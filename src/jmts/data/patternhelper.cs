
function JMPatterns::OnDelete(%this) {
	for(%i=0;%i<%this.numgroups;%i++) {
		%this.groups[%i].delete();
	}
}

function JMPatternGroups::OnDelete(%this) {
	for(%i=0;%i<%this.numpatts;%i++) {
		%this.pattern[%i].delete();
	}
}

function JMStyles::OnDelete(%this) {
	for(%i=0;%i<%this.numstyles;%i++) {
		%this.style[%i].delete();
	}
}


// Example code:
// %i = $jmstyles.FindStyle("Windmill");
// if(%i > -1) {
//   echo("Style Name:" SPC $jmstyles.GetStyleName(%i));
//   echo("Style Data:" SPC $jmstyles.GetStyleData(%i));
//   echo("Style Length:" SPC $jmstyles.GetStyleLength(%i));
// }

function JMStyles::FindStyle(%this, %name) {
	for(%i=0;%i<%this.numstyles;%i++) {
		if(stricmp(%this.style[%i].name, %name) == 0) {
			return %i;
		}
	}
	return -1;
}

function JMStyles::GetStyleName(%this, %index) {
	return %this.style[%index].name;
}

function JMStyles::GetStyleData(%this, %index) {
	return %this.style[%index].data;
}

function JMStyles::GetStyleLength(%this, %index) {
	return %this.style[%index].length;
}


function JMPatterns::NumGroups(%this) {
	return %this.numgroups;
}

function JMPatterns::FindGroup(%this, %name) {
	for(%i=0;%i<%this.numgroups;%i++) {
		if(stricmp(%this.groups[%i].name, %name) == 0) {
			return %i;
		}
	}
	return -1;
}

function JMPatterns::GetGroupName(%this, %index) {
	return %this.groups[%index].name;
}

function JMPatterns::GetGroupNumPatterns(%this, %index) {
	return %this.groups[%index].numpatts;
}

function JMPatterns::GetPatternName(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].name;
}

function JMPatterns::GetPatternStyle(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].style;
}

function JMPatterns::GetPatternSite(%this, %groupindex, %patternindex) {
	return %this.groups[%groupindex].pattern[%patternindex].site;
}
