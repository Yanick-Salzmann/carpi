$(() => {
    const app_code = get_env_value('HERE_APP_CODE');
    const app_id = get_env_value('HERE_APP_ID');

    gps_get_coordinates();

    function to_radian(val) {
        return (val * Math.PI) / 180.0;
    }

    function get_distance(fr, to) {
        // From: https://www.movable-type.co.uk/scripts/latlong.html

        const R = 6371e3;

        const phi1 = to_radian(fr.lat);
        const phi2 = to_radian(to.lat);
        const dphi = to_radian(to.lat - fr.lat);
        const dlambda = to_radian(to.lon - fr.lon);

        const a = Math.sin(dphi / 2.0) * Math.sin(dphi / 2.0) +
            Math.cos(phi1) * Math.cos(phi2) *
            Math.sin(dlambda / 2.0) * Math.sin(dlambda / 2.0);

        const c = 2.0 * Math.atan2(Math.sqrt(a), Math.sqrt(1.0 - a));

        return R * c;
    }

    class HereApi {
        reverse_geocode(position) {
            return new Promise((resolve, reject) => {
               let url = 'https://reverse.geocoder.api.here.com/6.2/reversegeocode.json?prox=' + position.lat + ',' + position.lon + ',50' +
                   '&mode=retrieveAddresses&maxResults=1&gen=9&app_id=' + encodeURIComponent(app_id) +
                   "&app_code=" + encodeURIComponent(app_code);

               $.get(url)
                   .done(data => {
                       resolve(data.Response.View.Result);
                   })
                   .fail(err => reject(err));
            });
        }

        fetch_pois(search_text) {
            const cur_pos = gps_get_coordinates();

            return new Promise((resolve, reject) => {
                let url = 'https://places.cit.api.here.com/places/v1/discover/search?';
                if (cur_pos.fix) {
                    url += 'at=' + cur_pos.lat + ',' + cur_pos.lon + '&';
                }

                url += 'q=' + encodeURIComponent(search_text) +
                    '&app_id=' + encodeURIComponent(app_id) +
                    '&app_code=' + encodeURIComponent(app_code) +
                    '&tf=plain';

                $.get(url)
                    .done((data) => {
                        resolve(data.results.items.map(result => {
                            result.distance = get_distance(cur_pos, {lat: result.position[0], lon: result.position[1]});
                            return result;
                        }));
                    })
                    .fail(err => {
                        reject(err);
                    })
            });
        }
    }

    window.here_api = new HereApi();
});