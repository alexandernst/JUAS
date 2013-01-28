$(document).ready(function(){
    "use strict";

    var finalX, finalY;
    var lr = 0, tb = 0;
    var shiftW = 0, shiftH = 0;
    var layout, container = $('#container'), side;
    var min_width, min_height, max_width, max_height;

    var currentX = App.x, currentY = App.y; //Current X,Y of the window
    var currentW = App.width, currentH = App.height; //Current width/height of the window (including margin)

    var observer, observer_conf = { subtree: true, childList: true, attributes: true, characterData: true };

    var parser = _.memoize(function(margins){
        return _.reduce(margins, function(memo, num){
            return memo + (parseInt(num, 10) === NaN ? 0 : parseInt(num, 10));
        }, 0);
    });

    function calc_margin(){
        lr = parser( _.values(container.css(['margin-left', 'margin-right'])) );
        tb = parser( _.values(container.css(['margin-top', 'margin-bottom'])) );
    }
    calc_margin();


    observer = new WebKitMutationObserver(function(mutations, observer){
        mutations.forEach(function(mutation){
            if(mutation.target.id === container.attr("id") && mutation.type === "attributes"){
                calc_margin();

                var w = container.css("width");
                w = parseInt(w, 10);

                var h = container.css("height");
                h = parseInt(h, 10);

                currentW = w + lr;
                currentH = h + tb;

                App.resize(currentX, currentY, currentW, currentH);
            }
        });
    });

    observer.start = function(){
        observer.observe(document, observer_conf);
    };

    observer.stop = function(){
        observer.disconnect();
    };

    observer.start();

    layout = container.layout({
        applyDemoStyles: true,
        resizeWithWindowMaxDelay: 1
    });

    container.resizable({
        handles: 'n, e, s, w, ne, se, sw, nw', // 'all'
        minWidth: 300,
        minHeight: 300,
        maxWidth: 1250,
        maxHeight: 700,

        start: function(e, ui){
            observer.stop();

            //      /*******N*******\
            //      *NW           NE*
            //      *               *
            //      W               E
            //      *               *
            //      *SW           SE*
            //      \*******S*******/

            var handleClass = $(e.originalEvent.target).attr("class");
            side = handleClass.match(/(?:^| )ui-resizable-(.{1,2})(?: |$)/)[1];

            var startX = App.x;
            var startY = App.y;

            var startW = App.width;
            var startH = App.height;

            switch(side){
                case "e":
                case "se":
                case "s":
                    //Fix to upper left corner
                    finalX = startX;
                    finalY = startY;
                    break;

                case "sw":
                case "w":
                    //Fix to upper right corner
                    finalX = startX + startW;
                    finalY = startY;
                    break;

                case "nw":
                    //Fix to bottom right corner
                    finalX = startX + startW;
                    finalY = startY + startH;
                    break;

                case "n":
                case "ne":
                    //Fix to bottom left corner
                    finalX = startX;
                    finalY = startY + startH;
                    break;
            }

            min_width = container.resizable("option", "minWidth");
            min_height = container.resizable("option", "minHeight");
            max_width = container.resizable("option", "maxWidth");
            max_height = container.resizable("option", "maxHeight");
        },

        resize: function(e, ui){

            currentX = finalX;
            currentY = finalY;

            currentW = ui.size.width;
            currentH = ui.size.height;

            var diffW = ui.originalSize.width - currentW;
            var diffH = ui.originalSize.height - currentH;

            switch(side){
                case "e":
                case "se":
                case "s":

                    currentW += lr;
                    currentH += tb;

                    break;

                case "sw":

                    currentH = currentH + tb;

                    if(!_.isNull(min_width) && shiftW + currentW + lr <= min_width){
                        currentW = min_width;
                    }else if(!_.isNull(max_width) && shiftW + currentW + lr >= max_width){
                        currentW = max_width;
                    }else{
                        shiftW -= diffW;
                        currentW = ui.originalSize.width + lr + shiftW;
                    }

                    currentX -= currentW;

                    break;

                case "w":

                    currentH += tb;

                    if(!_.isNull(min_width) && shiftW + currentW + lr <= min_width){
                        currentW = min_width;
                    }else if(!_.isNull(max_width) && shiftW + currentW + lr >= max_width){
                        currentW = max_width;
                    }else{
                        shiftW -= diffW;
                        currentW = ui.originalSize.width + lr + shiftW;
                    }

                    currentX -= currentW;

                    break;

                case "nw":

                    if(!_.isNull(min_width) && shiftW + currentW + lr <= min_width){
                        currentW = min_width;
                    }else if(!_.isNull(max_width) && shiftW + currentW + lr >= max_width){
                        currentW = max_width;
                    }else{
                        shiftW -= diffW;
                        currentW = ui.originalSize.width + lr + shiftW;
                    }

                    if(!_.isNull(min_height) && shiftH + currentH + tb <= min_height){
                        currentH = min_height;
                    }else if(!_.isNull(max_height) && shiftH + currentH + tb >= max_height){
                        currentH = max_height;
                    }else{
                        shiftH -= diffH;
                        currentH = ui.originalSize.height + tb + shiftH;
                    }

                    currentX -= currentW;
                    currentY -= currentH;

                    break;

                case "n":

                    currentW += lr;

                    if(!_.isNull(min_height) && shiftH + currentH + tb <= min_height){
                        currentH = min_height;
                    }else if(!_.isNull(max_height) && shiftH + currentH + tb >= max_height){
                        currentH = max_height;
                    }else{
                        shiftH -= diffH;
                        currentH = ui.originalSize.height + tb + shiftH;
                    }

                    currentY -= currentH;

                    break;

                case "ne":

                    currentW = currentW + lr;

                    if(!_.isNull(min_height) && shiftH + currentH + tb <= min_height){
                        currentH = min_height;
                    }else if(!_.isNull(max_height) && shiftH + currentH + tb >= max_height){
                        currentH = max_height;
                    }else{
                        shiftH -= diffH;
                        currentH = ui.originalSize.height + tb + shiftH;
                    }

                    currentY -= currentH;

                    break;
            }

            App.resize(currentX, currentY, currentW, currentH);

            container.width(currentW - lr);
            container.height(currentH - tb);

            container.css("left", "0px");
            container.css("top", "0px");

            e.preventDefault();

        },

        stop: function(e, ui){
            e.preventDefault();
            shiftW = shiftH = 0;
            observer.start();
        }
    });

    $("#minimize").on("mouseup", function(e){
        e.preventDefault();
        App.minimize();
	});

    $("#maximize").on("mouseup", function(e){
        e.preventDefault();
        App.maximize();
	});

    $("#quit").on("mouseup", function(e){
        e.preventDefault();
        App.quit();
    });

    $("#bar").mousedown(function(e){
        e.preventDefault();

        if(this !== e.target){
            return;
        }

        App.mousePressEvent(e.pageX, e.pageY);

        var f_mousemove = function(e){
            e.preventDefault();
            currentX = e.screenX - e.pageX;
            currentY = e.screenY - e.pageY;
            App.mouseMoveEvent(e.screenX, e.screenY);
        };

        $(document).on("mousemove", f_mousemove);

        $(document).one("mouseup", function(e){
            e.preventDefault();
            $(document).off("mousemove", f_mousemove);
        });

        $("#bar").one("ondragstart", function(e){
            e.preventDefault();
            return false;
        });
    });

});
