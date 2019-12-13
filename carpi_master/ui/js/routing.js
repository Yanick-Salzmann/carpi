$(() => {
    function defer_until_coordinates_available() {
        return new Promise((resolve) => {
           const update = function() {
               const cur_pos = gps_get_coordinates();
               if(!cur_pos.fix) {
                   setTimeout(update, 1000);
                   return;
               }

               resolve(cur_pos);
           };

           update();
        });
    }

    window.onRouteSelected = function(target) {
        defer_until_coordinates_available().then(coord => {
            return here_api.calc_route([coord.lat, coord.lon], target);
        }).then(route => {
           switch_to_splash_section("map-slash-card")
        });
    }
});