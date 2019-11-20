$(() => {
    const env_config = get_env_value('HERE_APP_ID', 'HERE_APP_CODE');

    const map = L.map('leaflet-map-container');
    let is_initialized = false;

    function on_update() {
        const position = gps_get_coordinates();
        map.setView([position.lat, position.lon], 13);
    }

    window.on_show_map_section = function () {
        if (!is_initialized) {
            is_initialized = true;

            L.tileLayer.here({appId: env_config['HERE_APP_ID'], appCode: env_config['HERE_APP_CODE']})
                .addTo(map);

            gps_get_coordinates();
        }

        setInterval(on_update, 500);
    }
});