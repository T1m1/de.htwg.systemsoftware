var eventHandle = (function () {

    $( "a.cpu" ).on({
        click: function() {
            controller.cpu();
        }
    });
    
    $( "a.network" ).on({
        click: function() {
            controller.network();
        }
    });

    $( "a.deleteTmp" ).on({
        click: function() {
            controller.deleteTmp();
        }
    });

    $( "a.mem" ).on({
        click: function() {
            controller.mem();
        }
    });

    $( "a.devices" ).on({
        click: function() {
            controller.devices();
        }
    });
    
    /* Public methods and variables */
    return {
    };
})();
