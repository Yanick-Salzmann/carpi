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