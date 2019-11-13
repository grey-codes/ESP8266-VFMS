function btnControl(usergroup) {
	if (usergroup & 1) {

		//if (output0State == "off") {
			$("#bluebtn").attr('class', 'button');
		//} else {
		//	$("#bluebtn").attr('class', 'button2');	
		//}
		
	} else {
			$("#bluebtn").attr('class', 'btnhide');
	}


	if (usergroup & 2) {

		//if (output4State == "off") {
			$("#greenbtn").attr('class', 'button');
		//} else {
		//	$("#greenbtn").attr('class', 'button2');
		//}
		
	} else {
			$("#greenbtn").attr('class', 'btnhide');
	}

	if (usergroup & 4) {
		//if (output5State == "off") {
			$("#redbtn").attr('class', 'button');
		//} else {
		//	$("#redbtn").attr('class', 'button2');
		//}
		
	} else {
			$("#redbtn").attr('class', 'btnhide');
	}
}

function updateButtons() {
	$.getJSON( "/user_info", function( data ) {
		btnControl(data.group);
	})
}