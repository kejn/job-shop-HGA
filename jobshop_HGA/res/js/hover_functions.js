var tds = document.getElementsByTagName('td');
var over = 0;
var clicked = 0;

function mOver(classname) {
	if (!clicked) {
		over = 1;
		var jobs = document.getElementsByClassName(classname);
		for (var i = 0; i < tds.length; ++i) {
			tds[i].style.opacity = 0.3;
		}
		for (var i = 0; i < jobs.length; ++i) {
			jobs[i].style.opacity = 1.0;
		}
	}
}

function mOut() {
	if (!clicked) {
		over = 0;
		setTimeout(function() {
			if (over == 1)
				return;
			for (var i = 0; i < tds.length; ++i) {
				tds[i].style.opacity = 1.0;
			}
		}, 500);
	}
}

function cpath() {
	if (!clicked) {
		clicked = 1;
		for (var i = 0; i < tds.length; ++i) {
			tds[i].style.opacity = 0.3;
		}
		var jobs = document.getElementsByClassName('cpath');
		for (var i = 0; i < jobs.length; ++i) {
			jobs[i].style.opacity = 1.0;
		}
	} else {
		clicked = 0;
	}
}