$(() => {
    let current_route = null;
    let current_leg = null;

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

    function angle_to_bearing(angle) {
        angle %= 360;
        if(angle > 337.5 || angle <= 22.5) {
            return 'north';
        } else if(angle > 22.5 && angle <= 67.5) {
            return 'northeast';
        } else if(angle > 67.5 && angle <= 112.5) {
            return 'east';
        } else if(angle > 112.5 && angle <= 157.5) {
            return 'southeast';
        } else if(angle > 157.5 && angle <= 202.5) {
            return 'south';
        } else if(angle > 202.5 && angle <= 247.5) {
            return 'southwest';
        } else if(angle > 247.5 && angle <= 292.5) {
            return 'west';
        } else if(angle > 292.5 && angle <= 337.5) {
            return 'northwest';
        } else {
            return '<unknown>';
        }
    }

    function direction_to_string(direction, angle, capitalize) {
        if(capitalize === undefined) {
            capitalize = true;
        }

        const dir_text = ((direction) => {
            switch(direction) {
                case 'forward': return `Keep going ${angle_to_bearing(angle)} on`;
                case 'bearRight': return "Keep right";
                case 'lightRight': return "Slight right";
                case 'right': return "Right";
                case 'hardRight': return "Hard right";
                case 'uTurnRight': return "Right U-Turn";
                case 'uTurnLeft': return "Left U-Turn";
                case 'hardLeft': return "Hard left";
                case 'left': return "Left";
                case 'lightLeft': return 'Slight left';
                case 'bearleft': return 'Keep left';
                default: return direction;
            }
        })(direction);

        if(capitalize) {
            return dir_text;
        }

        dir_text[0] = dir_text[0].toLowerCase();
        return dir_text;
    }

    function format_maneuver(maneuver) {
        switch(maneuver.action) {
            case 'depart': {
                return direction_to_string(maneuver.direction, maneuver.startAngle) + ' ' + maneuver.nextRoadName;
            }

            default: {
                return "<not implemented>"
            }
        }
    }

    function update_active_maneuvre(initial) {
        if (!initial) {
            throw "TODO: Implement";
        }

        const maneuver = current_leg.maneuver[0];
        const maneuver_text = format_maneuver(maneuver);
        $('section.map-section .nav-instruction-container span.current-instruction').text(maneuver_text);
    }

    window.onRouteSelected = function (target) {
        defer_until_coordinates_available().then(coord => {
            return here_api.calc_route([coord.lat, coord.lon], target, true);
        }).then(route => {
            current_route = route.response.route[0];
            current_leg = current_route.leg[0];

            clear_all_marker_layers();
            put_route_poly(route);
            update_active_maneuvre(true);
            switch_to_splash_section("map-section");

            console.debug(current_leg);
        });
    }
});