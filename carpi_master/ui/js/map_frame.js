$(() => {
    let zoom_level = parseInt(localStorage.getItem("map.zoom"));
    if (isNaN(zoom_level)) {
        zoom_level = 21;
        localStorage.setItem('map.zoom', zoom_level.toString());
    }

    const env_config = get_env_value('HERE_APP_ID', 'HERE_APP_CODE');

    const map = L.map('leaflet-map-container');
    const cur_pos = L.circle([0, 0], {
        color: 'blue',
        fillColor: '#30f',
        fillOpacity: 0.5,
        radius: 2
    }).addTo(map);

    map.on('zoomend', () => {
        zoom_level = map.getZoom();
        localStorage.setItem('map.zoom', zoom_level.toString());
    });

    let is_initialized = false;
    let is_custom_position = false;

    $('#leaflet-map-container').mousedown(() => {
        is_custom_position = true;
    });

    $('#center-camera-map-button').click(() => {
        const position = gps_get_coordinates();
        map.setView([position.lat, position.lon], map.getZoom());
    });

    $('#follow-position-map-button').click(() => {
        is_custom_position = false;
        on_update();
    });

    $('#zoom-in-map-button').click(() => {
        zoom_level = Math.min(zoom_level + 1, 18);
        localStorage.setItem('map.zoom', zoom_level.toString());
        map.setZoom(zoom_level);
    });

    $('#zoom-out-map-button').click(() => {
        zoom_level = Math.max(zoom_level - 1, 0);
        localStorage.setItem('map.zoom', zoom_level.toString());
        map.setZoom(zoom_level);
    });

    function on_update() {
        const position = gps_get_coordinates();
        if (!is_custom_position) {
            map.setView([position.lat, position.lon], zoom_level);
        }

        cur_pos.setLatLng([position.lat, position.lon]);
    }

    function load_demo_route() {
        // https://route.api.here.com/routing/7.2/calculateroute.json?waypoint0=52.5160%2C13.3779&waypoint1=52.5206%2C13.3862&mode=fastest%3Bcar%3Btraffic%3Aenabled&app_id=YmpbtgZCcwDWK0hpeNhT&app_code=9BKI5apEtPvTBmhFOcXJrA&departure=now&legAttributes=shape
        let base_url = "https://route.api.here.com/routing/7.2/calculateroute.json";
        const wp0 = "47.3472889,7.8960178";
        const wp1 = "47.3937279,8.5093238";

        base_url += "?waypoint0=" + encodeURIComponent(wp0);
        base_url += "&waypoint1=" + encodeURIComponent(wp1);
        base_url += "&mode=" + encodeURIComponent("fastest;car;traffic:enabled");
        base_url += "&app_code=" + env_config["HERE_APP_CODE"];
        base_url += "&app_id=" + env_config["HERE_APP_ID"];
        base_url += "&departure=now&legAttributes=shape";

        $.get(base_url).done(data => {
            const shape = data.response.route[0].leg[0].shape
            const linePoints = shape.map(rt => [parseFloat(rt.split(",")[0]), parseFloat(rt.split(",")[1])]);
            L.polyline(linePoints, {color: 'blue'}).addTo(map)
        });
    }

    window.on_show_map_section = function () {
        if (!is_initialized) {
            is_initialized = true;

            L.tileLayer.here({appId: env_config['HERE_APP_ID'], appCode: env_config['HERE_APP_CODE'], scheme: 'hybrid.day', resource: 'traffictile', baseType: 'traffic', extra_params: [
                    { key: "min_traffic_congestion", value: "heavy" }
                ]})
                .addTo(map);

            gps_get_coordinates();
            load_demo_route();
        }

        setInterval(on_update, 500);
    }
});