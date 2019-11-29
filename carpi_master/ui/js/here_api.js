$(() => {
    const app_code = get_env_value('HERE_APP_CODE');
    const app_id = get_env_value('HERE_APP_ID');

    gps_get_coordinates();

    class HereApi {
        fetch_pois(search_text) {
            const cur_pos = gps_get_coordinates();

            return new Promise((resolve, reject) => {
                $.get('https://places.cit.api.here.com/places/v1/autosuggest?at=' +
                    cur_pos.lat + ',' + cur_pos.lon +
                    '&q=' + encodeURIComponent(search_text) +
                    '&app_id=' + encodeURIComponent(app_id) +
                    '&app_code=' + encodeURIComponent(app_code))
                    .done((data) => {
                        resolve(data);
                    })
                    .fail(err => {
                        reject(err);
                    })
            });
        }
    }

    window.here_api = new HereApi();
});