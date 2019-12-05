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
        calc_route(from, to) {
            let base_url = "https://route.api.here.com/routing/7.2/calculateroute.json";
            const wp0 = encodeURIComponent(`${from[0]},${from[1]}`);
            const wp1 = encodeURIComponent(`${to[0]},${to[1]}`;

            base_url += "?waypoint0=" + wp0;
            base_url += "&waypoint1=" + wp1;
            base_url += "&mode=" + encodeURIComponent("fastest;car;traffic:enabled");
            base_url += "&app_code=" + encodeURIComponent(app_code);
            base_url += "&app_id=" + encodeURIComponent(app_id);
            base_url += "&departure=now&legAttributes=shape";

            return new Promise((resolve, reject) => {
                $.get(base_url).done(data => {
                    resolve(data);
                }).fail(() => {
                    reject();
                })
            });
        }

        reverse_geocode(position) {
            return new Promise((resolve, reject) => {
               let url = 'https://reverse.geocoder.api.here.com/6.2/reversegeocode.json?prox=' + position.lat + ',' + position.lon + ',50' +
                   '&mode=retrieveAddresses&maxResults=1&gen=9&app_id=' + encodeURIComponent(app_id) +
                   "&app_code=" + encodeURIComponent(app_code);

               $.get(url)
                   .done(data => {
                       resolve(data.Response.View[0].Result);
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