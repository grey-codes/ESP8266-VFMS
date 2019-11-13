function btnControl() {
const USERGROUP = 3;	
if (USERGROUP & 1) {

	if (output0State = "off") {
		$("#bluebtn").attr('class', 'button');
	} else {
		$("#bluebtn").attr('class', 'button2');
		
	}
	
} else {
		$("#bluebtn").attr('class', 'btnhide');
}


if (USERGROUP & 2) {

	if (output4State = "off") {
		$("#greenbtn").attr('class', 'button');
	} else {
		$("#greenbtn").attr('class', 'button2');
		
	}
	
} else {
		$("#greenbtn").attr('class', 'btnhide');
}

if (USERGROUP & 4) {
	if (output5State = "off") {
		$("#redbtn").attr('class', 'button');
	} else {
		$("#redbtn").attr('class', 'button2');
		
	}
	
} else {
		$("#redbtn").attr('class', 'btnhide');
}

}