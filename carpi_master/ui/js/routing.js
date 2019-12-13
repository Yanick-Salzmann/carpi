$(() => {
    function defer_until_coordinates_available() {
        return new Promise((resolve) => {
            const update = function () {
                const cur_pos = gps_get_coordinates();
                if (!cur_pos.fix) {
                    setTimeout(update, 1000);
                    return;
                }

                resolve(cur_pos);
            };

            update();
        });
    }

    function clear_all_marker_layers() {
        main_map.eachLayer(layer => {
            if (layer._path) {
                main_map.removeLayer(layer);
            }
        });
    }

    function put_route_poly(route) {
        const leg = route.response.route[0].leg[0];
        const route_points = leg.shape.map(point => [parseFloat(point.split(',')[0]), parseFloat(point.split(',')[1])]);
        const line = L.polyline(route_points, {color: 'rgba(0, 255, 255, 0.5)'});

        line.addTo(main_map);
    }

    window.onRouteSelected = function (target) {
        defer_until_coordinates_available().then(coord => {
            return here_api.calc_route([coord.lat, coord.lon], target);
        }).then(route => {
            clear_all_marker_layers();
            put_route_poly(route);
            switch_to_splash_section("map-section")
        });
    }
});