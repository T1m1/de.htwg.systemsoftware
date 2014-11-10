
var controller = (function () {

	var osInfo = "";
    var eventListener =
    {
        'init' 			: $.Callbacks(),
	'cpuSuccess'		: $.Callbacks(),
	'networkSuccess'	: $.Callbacks(),
	'deleteTmpSuccess'	: $.Callbacks(),
	'memSuccess'		: $.Callbacks(),
	'devicesSuccess'	: $.Callbacks(),
	'osInfoSuccess'		: $.Callbacks(),
	'upTimeSuccess'		: $.Callbacks(),
	'upTimeError'		: $.Callbacks(),
	'tmpInfoSuccess'	: $.Callbacks(),	
	'error'			: $.Callbacks(),
    };

	var cpuSuccess = function (resp) {
		$('#output').html(resp);
	};

	var networkSuccess = function (resp) {
		$('#output').html(resp);
	};

	var deleteTmpSuccess = function (resp) {
		$('#output').html("Delete Tmp was successfull!");
	};

	var otherSuccess = function (resp) {
		$('#output').html(resp);
	};

	var memSuccess = function (resp) {
		$('#output').html(resp);
	};

	var devicesSuccess = function (resp) {
		$('#output').html(resp);
	};

	var osInfoSuccess = function (resp) {
		osInfo = resp;
	};

	var upTimeSuccess = function (resp) {
		$('#info').html("OS: "+osInfo);
		$('#info').append("\n");
		$('#info').append("Uptime: "+resp);
		setInterval(function() {upTime();}, 60000);
	};

	var upTimeError = function (resp) {
		setInterval(function() {upTime();}, 60000);
	};

	var tmpInfoSuccess = function (resp) {
		$('#output').html("");
		if (resp == "\n") {
			alert("Nothing to delete");
			return;
		}
		if (confirm("Really delete the tmp folder?\n\n"+resp)) {
			ajaxRequest("cgi-bin/delete-tmp.sh", null, "deleteTmpSuccess", "error");
		}
	};

	var error = function (resp) {
		console.log(resp);
		$('#output').html(resp);
	};


    /* definition of functions to add callback functions. */
    var addEventListener = function (type, func) {
        eventListener[type].add(func);
    };
    
    /* function to send an ajax request and handle the return with callbacks. */
    var ajaxRequest = function (url, args, successCallback, failCallback) {
        var deferred = $.Deferred();

        /* request to server */
        request = $.ajax({
            url : url,
            type : "post",
            contentType : "text/html",
            data : JSON.stringify(args)
        });

        /* callback handler that will be called on success */
        request.done(function (response, textStatus, jqXHR){
            if (response.error) {
                eventListener[failCallback].fire(response);
                deferred.reject();
            } else {
                eventListener[successCallback].fire(response);
                deferred.resolve();
            }
        });

        /* callback handler that will be called on failure */
        request.fail(function (jqXHR, textStatus, errorThrown){
            /* fire the failed callback */
            var response = "Server request failed";
            eventListener[failCallback].fire(response);
            deferred.reject();
        });

        return deferred.promise();
    };
        
    var run = function () {
	addEventListener("cpuSuccess", cpuSuccess);
	addEventListener("networkSuccess", networkSuccess);
	addEventListener("deleteTmpSuccess", deleteTmpSuccess);
	addEventListener("memSuccess", memSuccess);
	addEventListener("devicesSuccess", devicesSuccess);
	addEventListener("osInfoSuccess", osInfoSuccess);
	addEventListener("upTimeSuccess", upTimeSuccess);
	addEventListener("upTimeSuccess", upTimeSuccess);
	addEventListener("upTimeError", upTimeError);
	addEventListener("tmpInfoSuccess", tmpInfoSuccess);
	addEventListener("error", error);
	osInfo();
	upTime();	
    };
    
	var cpu = function () {
		ajaxRequest("cgi-bin/cpu-info.sh", null, "cpuSuccess", "error");
	};

	var network = function () {
		ajaxRequest("cgi-bin/net-info.sh", null, "networkSuccess", "error");
	};

	var deleteTmp = function () {
		ajaxRequest("cgi-bin/tmp-info.sh", null, "tmpInfoSuccess", "error");
	};

	var mem = function () {
		ajaxRequest("cgi-bin/mem-info.sh", null, "memSuccess", "error");
	};

	var devices = function () {
		ajaxRequest("cgi-bin/devices-info.sh", null, "devicesSuccess", "error");
	};

	var osInfo = function () {
		ajaxRequest("cgi-bin/os-info.sh", null, "osInfoSuccess", "error");
	};

	var upTime = function () {
		ajaxRequest("cgi-bin/uptime.sh", null, "upTimeSuccess", "upTimeError");
	};
    
    /* Public methods and variables */
    return {
        addEventListener 	: addEventListener,
        run 			: run,
        cpu	 		: cpu,
	network			: network,
	deleteTmp		: deleteTmp,
	mem			: mem,
	devices			: devices,
    };
})();
