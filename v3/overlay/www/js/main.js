$(document).ready(function () {
	
	// hash array with all functions
	var hash = {
		'#cpu': 'cgi-bin/cpu-info.sh',
		'#osinfo': 'cgi-bin/os-info.sh',
		'#devices': 'cgi-bin/device-info.sh',
		'#mem': 'cgi-bin/mem-info.sh',
		'#network': 'cgi-bin/net-info.sh',
		'#deleteTmp': 'cgi-bin/delete-tmp.sh',
		'#uptime': 'cgi-bin/uptime.sh',
		'#ledOn': 'cgi-bin/ledOn.sh',
		'#ledOff': 'cgi-bin/ledOff.sh',
		'#ledStatus': 'cgi-bin/ledStatus.sh',
		'#switchOn': 'cgi-bin/switchOn.sh',
		'#switchOff': 'cgi-bin/switchOff.sh',
		'#switchStatus': 'cgi-bin/switchStatus',
		'#reboot': 'cgi-bin/reboot.sh'
	};
	
	// click event on buttons (a - tag)
	$('a').on('click', function (event) {
		var current = event.currentTarget.hash;
		if(!current || !hash[current]) {
			event.preventDefault();
			return;
		}
		
		$.ajax({
			url: hash[current],
			dataType: "html",
			success: function(data) {
				replaceHtml(data, '#output');
			}
		});
	});
	
	
	// show new values
	var replaceHtml = function(data, elemId) {
		$(elemId).html(data);
	};
	
	var request = function(key) {
		$.ajax({
			url: hash[key],
			dataType: "html",
			success: function(data) {
				replaceHtml(data, key);
			}
		});
	};




	function updateGpio(key, data) {
		if (data == 0) {
			$(key).toggles({on: true});
		} else {
			$(key).toggles({off: true});
		}
	}

	function updateGpioStatus(data, key) {
		if (key == '#ledStatus') {
			updateGpio('.led', data);
		}
		if (key == '#switchStatus') {
			updateGpio('.switch', data);
		}
	}

	var gpioStatus = function (key) {
		$.ajax({
			url: hash[key],
			dataType: "html",
			success: function (data) {
				updateGpioStatus(data, key);
			}
		});
	}

	var changeStatus = function (value) {
		$.ajax({
			url: hash[value],
			dataType: "html"
		});
	}

	$('.led').on('toggle', function (e, active) {
		if (active) {
			changeStatus('#ledOn');
		} else {
			changeStatus('#ledOff');
		}
	});

	$('.led').toggles();
	$('.switch').toggles();
	

	
	(function iniUptime() {
		request('#uptime');
		request('#osinfo');
		
		setInterval(request('#uptime'), 10000);
		setInterval(request('#osinfo'), 10000);
	})();



	(function initGpio() {
		gpioStatus('#ledStatus');
		gpioStatus('#switchStatus');

		setInterval(gpioStatus('#ledStatus'), 500);
		setInterval(gpioStatus('#switchStatus'),500);
	})();
	
});
