function btnControl(usergroup) {
	$.getJSON( "/led_info", function( data ) {
		if (usergroup & 1) {

			if ((data.status & 1) == 0) {
				$("#bluebtn").attr('class', 'button');
			} else {
				$("#bluebtn").attr('class', 'button button2');	
			}
			
		} else {
				$("#bluebtn").attr('class', 'btnhide');
		}


		if (usergroup & 2) {

			if ((data.status & 2) == 0) {
				$("#greenbtn").attr('class', 'button');
			} else {
				$("#greenbtn").attr('class', 'button button2');
			}
			
		} else {
				$("#greenbtn").attr('class', 'btnhide');
		}

		if (usergroup & 4) {
			if ((data.status & 4) == 0) {
				$("#redbtn").attr('class', 'button');
			} else {
				$("#redbtn").attr('class', 'button button2');
			}
			
		} else {
				$("#redbtn").attr('class', 'btnhide');
		}
	});
}

var usgroup;

function updateButtons() {
	$.getJSON( "/user_info", function( data ) {
		usgroup = data.group;
		btnControl(usgroup);
	});
	$("#bluebtn").click(function() {
		$.post("/led_set",{"led":"1"},function(data) { btnControl(usgroup); })
	});
	$("#greenbtn").click(function() {
		$.post("/led_set",{"led":"2"},function(data) { btnControl(usgroup); })
	});
	$("#redbtn").click(function() {
		$.post("/led_set",{"led":"4"},function(data) { btnControl(usgroup); })
	});
}