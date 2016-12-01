$(document).ready(function(){
    "use strict";

    console.log("here")
    new QWebChannel(qt.webChannelTransport, function (channel) {
        console.log(channel);
        window.App = channel.objects.App;
        core_init();
        app_init();
    });
});

function core_init(){
    "use strict";

    var wxf, wyf, wwi, whi, mxi, myi;
    var lr = 0, tb = 0;
    var shiftW = 0, shiftH = 0;
    var container = $('#container'), layout, side;
    var min_width, min_height, max_width, max_height;

    var observer, observer_conf = { subtree: true, childList: true, attributes: true, characterData: true };

    var parser = _.memoize(function(margins){
        return _.reduce(margins, function(memo, num){
            return memo + ( _.isNaN(parseInt(num, 10)) ? 0 : parseInt(num, 10));
        }, 0);
    });

    observer = new WebKitMutationObserver(function(mutations, observer){
        mutations.forEach(function(mutation){
            if(mutation.target.id === container.attr("id") && mutation.type === "attributes"){

                lr = parser( _.values(container.css(['margin-left', 'margin-right'])) );
                tb = parser( _.values(container.css(['margin-top', 'margin-bottom'])) );

                var w = parseInt(container.css("width"), 10);
                var h = parseInt(container.css("height"), 10);

                App.resize(App.x, App.y, w + lr, h + tb);
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

    function betweenMinMaxWidth(width){
        if(!_.isNull(min_width) && width + lr <= min_width){
            return min_width;
        }else if(!_.isNull(max_width) && width + lr >= max_width){
            return max_width;
        }else{
            return width;
        }
    };

    function betweenMinMaxHeight(height){
        if(!_.isNull(min_height) && height + tb <= min_height){
            return min_height;
        }else if(!_.isNull(max_height) && height + tb >= max_height){
            return max_height;
        }else{
            return height;
        }
    };

    container.resizable({
        handles: 'n, e, s, w, ne, se, sw, nw', // 'all'

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

            var wxi = App.x;
            var wyi = App.y;

            wwi = App.width;
            whi = App.height;

            mxi = e.screenX;
            myi = e.screenY;

            switch(side){
                case "e":
                case "se":
                case "s":
                    //Fix to upper left corner
                    wxf = wxi;
                    wyf = wyi;
                    break;

                case "sw":
                case "w":
                    //Fix to upper right corner
                    wxf = wxi + wwi;
                    wyf = wyi;
                    break;

                case "nw":
                    //Fix to bottom right corner
                    wxf = wxi + wwi;
                    wyf = wyi + whi;
                    break;

                case "n":
                case "ne":
                    //Fix to bottom left corner
                    wxf = wxi;
                    wyf = wyi + whi;
                    break;
            }

            min_width = container.resizable("option", "minWidth");
            min_height = container.resizable("option", "minHeight");
            max_width = container.resizable("option", "maxWidth");
            max_height = container.resizable("option", "maxHeight");

            EventBus.dispatch("window_resize_start");

        },

        resize: function(e, ui){

            //wxi == initial window X
            //wyi == initial window Y
            //wxf == opposite X of the side that is being resized, eg: wxf of SE is NW
            //wyf == oppsoite Y of the side that is being resized

            //wwi == initial window width
            //whi == initial window height
            //wwc == current window width
            //whc == current window height

            //      wyi                                      wyc/wyf
            //  wxi /******wwi*******\               wxc/wxf /******wwc*******\
            //      *               *                        *                *
            //      *               *                        *              whc
            //      *             whi        --->            *                *
            //      *               *                        \****************/ mxc
            //      *               *                                       myc
            //      \***************/ mxi
            //                    myi

            var mxc = e.screenX;
            var myc = e.screenY;

            var wwc, whc;

            var wxc = wxf;
            var wyc = wyf;

            switch(side){
                case "e":
                    wwc = wwi - (mxi - mxc);
                    whc = whi;
                    break;

                case "se":
                    wwc = wwi - (mxi - mxc);
                    whc = whi - (myi - myc);
                    break;

                case "s":
                    wwc = wwi;
                    whc = whi - (myi - myc);
                    break;

                case "sw":
                    wwc = wwi - (mxc - mxi);
                    whc = whi - (myi - myc);

                    wxc = wxf - betweenMinMaxWidth(wwc);
                    break;

                case "w":
                    wwc = wwi - (mxc - mxi);
                    whc = whi;

                    wxc = wxf - betweenMinMaxWidth(wwc);
                    break;

                case "nw":
                    wwc = wwi - (mxc - mxi);
                    whc = whi - (myc - myi);

                    wxc = wxf - betweenMinMaxWidth(wwc);
                    wyc = wyf - betweenMinMaxHeight(whc);
                    break;

                case "n":
                    wwc = wwi;
                    whc = whi - (myc - myi);

                    wyc = wyf - betweenMinMaxHeight(whc);
                    break;

                case "ne":
                    wwc = wwi - (mxi - mxc);
                    whc = whi - (myc - myi);

                    wyc = wyf - betweenMinMaxHeight(whc);
                    break;

            }

            wwc = betweenMinMaxWidth(wwc);
            whc = betweenMinMaxHeight(whc);

            App.resize(wxc, wyc, wwc + lr, whc + tb);
            container.css({ left: "0px", top: "0px", width: wwc, height: whc });

            e.preventDefault();
            EventBus.dispatch("window_resize");
        },

        stop: function(e, ui){
            e.preventDefault();
            shiftW = shiftH = 0;
            observer.start();
            EventBus.dispatch("window_resize_stop");
        }
    });

    $("#minimize").on("mouseup", function(e){
        e.preventDefault();
        App.minimize();
        EventBus.dispatch("window_minimize");
    });

    $("#maximize").on("mouseup", function(e){
        e.preventDefault();
        container.css({ width: "", height: "" }); //Hack. Maybe jQuery layout should take care of that?
        App.maximize();
        EventBus.dispatch("window_maximize");
    });

    $("#quit").on("mouseup", function(e){
        e.preventDefault();
        if(!EventBus.hasEventListener("app_quit")){
            App.quit();
        }else{
            EventBus.dispatch("app_quit");
        }
    });

    $("#bar").mousedown(function(e){
        e.preventDefault();

        if(this !== e.target){
            return;
        }

        App.mousePressEvent(e.pageX, e.pageY);

        var f_mousemove = function(e){
            e.preventDefault();
            App.mouseMoveEvent(e.screenX, e.screenY);
            EventBus.dispatch("window_drag");
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

};
