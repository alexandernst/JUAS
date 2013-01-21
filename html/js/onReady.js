var DEBUG = true;

$(document).ready(function(){

    var layout;
    var shiftW = 0, shiftH = 0; //Used for resizing
    var startX, startY, startW, startH; //Used every time a resize is started
    var currentX = App.x, currentY = App.y;
    var currentW = App.width, currentH = App.height;
    var container = $('#container');

    //TODO: change l, r, t and b if they're changed programatically
    var l = parseInt(container.css('padding-left')) + parseInt(container.css('margin-left'));
    var r = parseInt(container.css('padding-right')) + parseInt(container.css('margin-right'));
    var t = parseInt(container.css('padding-top')) + parseInt(container.css('margin-top'));
    var b = parseInt(container.css('padding-bottom')) + parseInt(container.css('margin-bottom'));

    layout = container.layout({
		applyDemoStyles: true,
        resizeWithWindowMaxDelay: 1
	});

    container.resizable({
		handles: 'n, e, s, w, ne, se, sw, nw', // 'all' ?
        minWidth: 300,
        minHeight: 300,

		start: function(e, ui){
			handleTarget = $(e.originalEvent.target);
            startX = e.screenX - e.pageX;
            startY = e.screenY - e.pageY;
            startW = ui.size.width;
            startH = ui.size.height;
		},

		resize: function(e, ui){

            var side = "";                                              //	/*******N*******\
            var sides = ["e", "se", "s", "sw", "w", "nw", "n", "ne"];   //	*NW			  NE*
            _.each(sides, function(value, index){                       //	*				*
                if(handleTarget.hasClass("ui-resizable-" + value)){     //	W				E
                    side = value.toUpperCase();                         //	*				*
                }                                                       //	*SW			  SE*
            });                                                         //	\*******S*******/

            currentX = e.screenX - e.pageX;
            currentY = e.screenY - e.pageY;

            currentW = ui.size.width;
            currentH = ui.size.height;

            //TODO: Handle max width/height
            //TODO: Handle null values
            var min_width = container.resizable("option", "minWidth");
            var min_height = container.resizable("option", "minHeight");

            //In case we hit the min width/height we'll use those
            var shiftW_bkp = shiftW;
            var shiftH_bkp = shiftH;
            var currentX_bkp = currentX;
            var currentY_bkp = currentY;

            switch(side){
                case "E":
                case "SE":
                case "S":
                    currentW = currentW + l + r;

                    currentH = currentH + t + b;

                    break;

                case "SW":
                    currentX += ui.originalSize.width - currentW;
                    shiftW += currentW - ui.originalSize.width;
                    currentW = (ui.originalSize.width + l + r) + shiftW;

                    currentH = currentH + t + b;

                    if(currentW - l - r < min_width){
                        currentX = currentX_bkp;
                        shiftW = shiftW_bkp;
                        currentW = min_width;
                    }

                    break;

                case "W":
                case "NW":
                case "N":
                    currentX += ui.originalSize.width - currentW;
                    shiftW += currentW - ui.originalSize.width;
                    currentW = (ui.originalSize.width + l + r) + shiftW;

                    currentY += ui.originalSize.height - currentH;
                    shiftH += currentH - ui.originalSize.height;
                    currentH = (ui.originalSize.height + t + b) + shiftH;

                    if(currentW - l - r < min_width){
                        currentX = currentX_bkp;
                        shiftW = shiftW_bkp;
                        currentW = min_width;
                    }

                    if(currentH - t - b < min_height){
                        currentY = currentY_bkp;
                        shiftH = shiftH_bkp;
                        currentH = min_height;
                    }

                    break;

                case "NE":
                    currentW = currentW + l + r;

                    currentY += ui.originalSize.height - currentH;
                    shiftH += currentH - ui.originalSize.height;
                    currentH = (ui.originalSize.height + t + b) + shiftH;

                    if(currentH - t - b < min_height){
                        currentY = currentY_bkp;
                        shiftH = shiftH_bkp;
                        currentH = min_height;
                    }

                    break;
            }

            App.jsResize(currentX, currentY, currentW, currentH);

            container.width(currentW - l - r);
            container.height(currentH - t - b);

            container.css("left", "0px");
            container.css("top", "0px");

            e.preventDefault();
            //TODO: dispatch only if changed
            EventBus.dispatch("window_resize", e, ui);

        },

        stop: function(e, ui){
            e.preventDefault();

            //TODO: dispatch only if changed
            EventBus.dispatch("window_resized", e, ui);

            shiftW = shiftH = 0;
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

        startX = e.screenX - e.pageX;
        startY = e.screenY - e.pageY;

        App.mousePressEvent(e.pageX, e.pageY);

        var f_mousemove = function(e){
            e.preventDefault();

            //TODO: dispatch only if changed
            EventBus.dispatch("window_drag", e);

            //TODO: dispatch only if changed
            EventBus.dispatch("window_move", e);

            currentX = e.screenX - e.pageX;
            currentY = e.screenY - e.pageY;
            App.mouseMoveEvent(e.screenX, e.screenY);
        }

        $(document).on("mousemove", f_mousemove);

        $(document).one("mouseup", function(e){
            e.preventDefault();

            //TODO: dispatch only if changed
            EventBus.dispatch("window_moved", e);

            $(document).off("mousemove", f_mousemove);
        });

        $("#bar").one("ondragstart", function(e){
            e.preventDefault();
            return false;
        });
	});

    function debug_window_resize(e, ui){
        e = e.target;
        //console.log("Size change from %s:%s to %s:%s", ui.originalSize.width + l + r, ui.originalSize.height + t + b, currentW, currentH);
    }

    function debug_window_resized(e, ui){
        console.log("Size changed from %s:%s to %s:%s", startW, startH, currentW, currentH);
    }

    function debug_window_drag(e){
        e = e.target;
        oldMouseX = e.screenX - (e.screenX - e.pageX - currentX);
        oldMouseY = e.screenY - (e.screenX - e.pageX - currentY);
        //console.log("Mouse drag from %s:%s to %s:%s", oldMouseX, oldMouseY, e.screenX, e.screenY);
    }

    function debug_window_move(e){
        e = e.target;
        newX = e.screenX - e.pageX;
        newY = e.screenY - e.pageY;
        //console.log("Window position change from %s:%s to %s:%s", currentX, currentY, newX, newY);
    }

    function debug_window_moved(e){
        console.log("Window position changed from %s:%s to %s:%s", startX, startY, currentX, currentY);
    }

    if(DEBUG){
        EventBus.addEventListener("window_resize", debug_window_resize);
        EventBus.addEventListener("window_resized", debug_window_resized);

        EventBus.addEventListener("window_drag", debug_window_drag);

        EventBus.addEventListener("window_move", debug_window_move);
        EventBus.addEventListener("window_moved", debug_window_moved);
    }

});
