$(() => {
    let zoom_level = parseInt(localStorage.getItem("map.zoom"));
    if(isNaN(zoom_level)) {
        zoom_level = 21;
        localStorage.setItem('map.zoom', zoom_level.toString());
    }

    const env_config = get_env_value('HERE_APP_ID', 'HERE_APP_CODE');

    const map = L.map('leaflet-map-container');
    const cur_pos = L.circle([0,0], {
        color: 'blue',
            fillColor: '#30f',
            fillOpacity: 0.5,
            radius: 2
    }).addTo(map);
    let is_initialized = false;

    function on_update() {
        const position = gps_get_coordinates();
        map.setView([position.lat, position.lon], 21);
        cur_pos.setLatLng([position.lat, position.lon]);
    }

    window.on_show_map_section = function () {
        if (!is_initialized) {
            is_initialized = true;

            L.tileLayer.here({appId: env_config['HERE_APP_ID'], appCode: env_config['HERE_APP_CODE'], scheme: 'hybrid.day.transit'})
                .addTo(map);

            gps_get_coordinates();
        }

        setInterval(on_update, 500);
    }
});