var tds = document.getElementsByTagName('td');
var over = 0;

function mOver(classname) {
	over = 1;
	var jobs = document.getElementsByClassName(classname);
	for (var i = 0; i < tds.length; ++i) {
		tds[i].style.opacity = 0.3;
	}
	for (var i = 0; i < jobs.length; ++i) {
		jobs[i].style.opacity = 1.0;
	}
}

function mOut() {
	over = 0;
	setTimeout(function() {
		if (over == 1)
			return;
		for (var i = 0; i < tds.length; ++i) {
			tds[i].style.opacity = 1.0;
		}
	}, 500);
}