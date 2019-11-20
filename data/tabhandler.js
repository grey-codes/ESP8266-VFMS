function openForm(formName) {
	$(".tabcontent").css("display","none");
	$(".tablinks").removeClass("active");
	$("#"+formName).css("display","block");
	$("#"+formName+" Tab").addClass("active");
} 