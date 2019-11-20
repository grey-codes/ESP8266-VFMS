
function rgbToHex(rgb) { 
	var hex = Number(rgb).toString(16);
	if (hex.length < 2) {
		hex = "0" + hex;
	}
	return hex;
};

function hexToRgb(hex) {
	// Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
	var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
	hex = hex.replace(shorthandRegex, function(m, r, g, b) {
		return r + r + g + g + b + b;
	});

	var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
	return result ? {
		r: parseInt(result[1], 16),
		g: parseInt(result[2], 16),
		b: parseInt(result[3], 16)
	} : null;
}

function sliderControl(usergroup) {
	$.getJSON( "/led_info", function( data ) {
		if ((usergroup & (1|2|4)) == (1|2|4)) {
			$("#rgb").removeClass("btnhide");
			$("#label_rgb").removeClass("btnhide");
			var red = rgbToHex(data.red);
			var green = rgbToHex(data.green);
			var blue = rgbToHex(data.blue);
			$("#rgb").val("#"+red+green+blue);
		} else {
			if ((usergroup & 1) == 1) {
				$("#slider_blue").removeClass("btnhide");
				$("#label_blue").removeClass("btnhide");
				$("#slider_blue").val(data.blue);
			}

			if ((usergroup & 2) == 2) {
				$("#slider_green").removeClass("btnhide");
				$("#label_green").removeClass("btnhide");
				$("#slider_green").val(data.green);
			}

			if ((usergroup & 4) == 4) {
				$("#slider_red").removeClass("btnhide");
				$("#label_red").removeClass("btnhide");
				$("#slider_red").val(data.red);
			}
		}
	});
}

var usgroup;

function updateSliders() {
	$.getJSON( "/user_info", function( data ) {
		usgroup = data.group;
		sliderControl(usgroup);
	});
	$("#rgb").on( 'change', function( event ) {
		res = hexToRgb( $(this).val() );
		$.post("/led_set",{ "blue":res.b, "green":res.g, "red":res.r },function(data) {  })
	});
	$("#slider_blue").on( 'change', function( event ) {
		$.post("/led_set",{ "blue":$(this).val() },function(data) {  })
	});
	$("#slider_green").on( 'change', function( event ) {
		$.post("/led_set",{ "green":$(this).val() },function(data) {  })
	});
	$("#slider_red").on( 'change', function( event ) {
		$.post("/led_set",{ "red":$(this).val() },function(data) {  })
	});
	/*
	$("#bluebtn").click(function() {
		$.post("/led_set",{"led":"1"},function(data) { btnControl(usgroup); })
	});
	$("#greenbtn").click(function() {
		$.post("/led_set",{"led":"2"},function(data) { btnControl(usgroup); })
	});
	$("#redbtn").click(function() {
		$.post("/led_set",{"led":"4"},function(data) { btnControl(usgroup); })
	});
	*/
}