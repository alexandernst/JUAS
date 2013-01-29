$(document).ready(function(){

    //We'll find those two vars useful
    var container = $('#container');
    var layout = container.data("layout");

    //App let's us access the C++ side of the application
    App.createTrayIcon("html/img/trayicon.png");

});
