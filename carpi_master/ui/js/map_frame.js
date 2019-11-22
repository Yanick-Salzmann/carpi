$(() => {
    let zoom_level = parseInt(localStorage.getItem("map.zoom"));
    if (isNaN(zoom_level)) {
        zoom_level = 21;
        localStorage.setItem('map.zoom', zoom_level.toString());
    }

    const defaultLayers = here_platform.createDefaultLayers();

    const map = new H.Map(document.getElementById('leaflet-map-container'),
        defaultLayers.vector.normal.map,
        {
            zoom: 10,
            center: { lng: 13.4, lat: 52.51 }
        }
    );

    /*const cur_pos = new H.map.Circle({lat: 0, lng: 0}, 2, {
        strokeColor: 'blue',
        fillColor: 'rgba(0, 48, 255, 0.5)'
    });

    map.addObject(cur_pos);*/

    /*map.addEventListener('mapviewchangeend', () => {
        zoom_level = map.getZoom();
        localStorage.setItem('map.zoom', zoom_level.toString());
    });

    let is_custom_position = false;

    $('#leaflet-map-container').mousedown(() => {
        is_custom_position = true;
    });

    $('#center-camera-map-button').click(() => {
        const position = gps_get_coordinates();
        map.setCenter({lat: position.lat, lng: position.lon});
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
            //map.setCenter({lat: position.lat, lng: position.lon});
        }

        //cur_pos.setCenter({lat: position.lat, lng: position.lon});
    }*/

    window.on_show_map_section = function () {
        gps_get_coordinates();
        //setInterval(on_update, 500);
    }
});