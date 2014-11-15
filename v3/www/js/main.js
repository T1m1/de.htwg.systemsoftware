$(document).ready(function () {
	
	// hash array with all functions
	var hash = {
		'#cpu': 'cgi-bin/cpu-info.sh',
		'#osinfo': 'cgi-bin/os-info.sh',
		'#devices': 'cgi-bin/device-info.sh',
		'#mem': 'cgi-bin/mem-info.sh',
		'#network': 'cgi-bin/net-info.sh',
		'#deleteTmp': 'cgi-bin/delete-tmp.sh',
		'#uptime': 'cgi-bin/uptime.sh'
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
	
	(function iniUptime() {
		request('#uptime');
		request('#osinfo');
		
		setInterval(request('#uptime'), 10000);
		setInterval(request('#osinfo'), 10000);
	})();
	
});
