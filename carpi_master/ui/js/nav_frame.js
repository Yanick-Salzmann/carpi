$(() => {
    let is_post_code_input = true;

    let plz_ch = null;

    let unique_streets = [];
    let street_map = {};

    let cur_street_numbers = [];

    let plz_prefix = '';
    let city_prefix = '';
    let address_prefix = '';
    let number_prefix = '';

    new PlzCsvReader("data/plz_ch.csv").then(info => plz_ch = info);

    function switchToWizardStep(id) {
        $('section.nav-section .wizard-body-wrapper > div:not(.hidden)').addClass('hidden');
        $('#' + id).removeClass('hidden');
    }


    function streetRecommendations(values) {
        const container = $('#nav-wizard-step-addr-ch-street .item-recommendation');
        container.empty();

        for (let i in [...Array(Math.min(10, values.length))]) {
            const row = values[i];
            const text = row.street;
            const parent = $('<div class="recommendation"></div>');
            parent.click(() => {
                onStreetSelected(row);
            });
            parent.text(text);
            container.append(parent);
        }
    }

    function updateStreetKeyboard(keys) {
        street_keyboard.filterEnabledKeys(key => {
            return key === '\b' || keys.indexOf(key.toLowerCase()) >= 0;
        });
    }

    function onCitySelected(plz) {
        const active_addresses = ch_get_addresses(plz);
        street_map = {};
        for (let i = 0; i < active_addresses.length; ++i) {
            const addr = active_addresses[i];
            if (addr.street in street_map) {
                street_map[addr.street].push(addr);
            } else {
                street_map[addr.street] = [addr];
            }
        }

        unique_streets = _.uniq(active_addresses, addr => addr.street);
        unique_streets = _.sortBy(unique_streets, addr => addr.street);

        address_prefix = '';
        updateStreetKeyboard(_.uniq(unique_streets.map(addr => addr.street[0].toLowerCase())));

        streetRecommendations(unique_streets);

        switchToWizardStep("nav-wizard-step-addr-ch-street");
    }

    function updateStreetNumberKeyboard(addresses) {
        const available = _.uniq(addresses.filter(addr => addr.number.length > number_prefix.length)
            .map(addr => addr.number[number_prefix.length].toLowerCase()));

        street_num_keyboard.filterEnabledKeys(key => {
            return key === '\b' || available.indexOf(key.toLowerCase()) >= 0;
        });
    }

    function onStreetSelected(address) {
        switchToWizardStep("nav-wizard-step-addr-ch-street-number");
        cur_street_numbers = _.sortBy(street_map[address.street], addr => addr.number);
        updateStreetNumberRecommendations(cur_street_numbers);
        updateStreetNumberKeyboard(cur_street_numbers);
    }

    function updateStreetNumberRecommendations(elements) {
        const container = $('#nav-wizard-step-addr-ch-street-number .item-recommendation');
        container.empty();

        for (let i in [...Array(Math.min(50, elements.length))]) {
            const row = elements[i];
            const parent = $('<div class="recommendation"></div>');
            parent.text(row.number);
            container.append(parent);
        }
    }

    function onStreetNumKeyPressed(key) {
        if (key === '\b') {
            if (!number_prefix) {
                return;
            }

            number_prefix = number_prefix.substr(0, number_prefix.length - 1);
        } else {
            number_prefix += key;
        }

        $('#ch-street-num-input-target').val(number_prefix);
        const addrs = cur_street_numbers.filter(addr => addr.number.toLowerCase().startsWith(number_prefix.toLowerCase()));
        updateStreetNumberRecommendations(addrs);
        updateStreetNumberKeyboard(addrs);
    }

    function onStreetKeyPressed(key) {
        if (key === '\b') {
            if (!address_prefix) {
                return;
            }

            address_prefix = address_prefix.substr(0, address_prefix.length - 1);
        } else {
            address_prefix += key;
        }

        $('#ch-street-input-target').val(address_prefix);

        const addrs = unique_streets.filter(addr => addr.street.toLowerCase().startsWith(address_prefix.toLowerCase()));
        updateStreetKeyboard(_.uniq(addrs.filter(addr => addr.street.length > address_prefix.length).map(addr => addr.street[address_prefix.length].toLowerCase())));
        streetRecommendations(addrs);
    }

    function loadInitialRecommendations(elements) {
        for (let i in [...Array(10).keys()]) {
            const row = elements[i];
            const text = `${row.plz} ${row.city} (${row.state_abbrvtn})`;

            const container = $('#nav-wizard-step-addr-ch-plz .item-recommendation');
            const parent = $('<div class="recommendation"></div>');
            parent.click(() => {
                onCitySelected(row.plz);
            });
            parent.text(text);
            container.append(parent);
        }
    }

    function loadInitialPlzValues() {
        loadInitialRecommendations(plz_ch);
    }

    function loadInitialCityValues() {
        const cities = [...plz_ch].filter(val => !isNaN(val.plz)).sort((a, b) => {
            if (a && b && a.city && b.city) {
                return a.city.localeCompare(b.city);
            }

            if (!a && b) {
                return -1;
            }

            if (a && !b) {
                return 1;
            }

            if (a.city && !b.city) {
                return 1;
            }

            if (!a.city && b.city) {
                return -1;
            }

            return 0;
        });

        loadInitialRecommendations(cities);
    }

    function defaultNumberFiltering() {
        plz_keyboard.filterEnabledKeys(key => key === '\b' || !isNaN(parseInt(key)));
    }

    function backspaceOnlyFiltering() {
        plz_keyboard.filterEnabledKeys(key => key === '\b');
    }

    function updatePlzFilter() {
        $('#ch-post-code-input-target').val(plz_prefix);
        const container = $('#nav-wizard-step-addr-ch-plz .item-recommendation');
        const len = plz_prefix.length;
        if (len > 4) {
            backspaceOnlyFiltering();
            container.empty();
            return;
        }

        if (!len) {
            container.empty();
            loadInitialPlzValues();
            defaultNumberFiltering();
            return;
        }

        const search_plz = parseInt(plz_prefix);
        if (isNaN(search_plz)) {
            return;
        }

        container.empty();

        const dividend = Math.pow(10, 4 - len);

        let next_plz_chars = [];

        let num_results = 0;
        for (let i = 0; i < plz_ch.length; ++i) {
            const key = Math.floor(plz_ch[i].plz / dividend);
            if (key !== search_plz) {
                continue;
            }

            const row = plz_ch[i];
            if (num_results < 10) {
                const text = `${row.plz} ${row.city} (${row.state_abbrvtn})`;
                const parent = $('<div class="recommendation"></div>');
                parent.click(() => {
                    onCitySelected(row.plz);
                });
                parent.text(text);
                container.append(parent);
                ++num_results;
            }

            if (len < 4) {
                const next_num = Math.floor((plz_ch[i].plz * 10) / dividend) % 10;
                if (next_plz_chars.indexOf(next_num) < 0) {
                    next_plz_chars.push(next_num);
                }
            }
        }

        if (len >= 4) {
            backspaceOnlyFiltering();
        } else {
            plz_keyboard.filterEnabledKeys(key => {
                if (key === '\b') {
                    return true;
                }

                const num_key = parseInt(key);
                if (isNaN(num_key)) {
                    return false;
                }

                return next_plz_chars.indexOf(num_key) >= 0;
            })
        }
    }

    function updateCityFilter() {

    }

    function updateKeyPlz(key) {
        if (key === '\b') {
            if (!plz_prefix.length) {
                loadInitialPlzValues();
                return;
            }

            plz_prefix = plz_prefix.substr(0, plz_prefix.length - 1);
            updatePlzFilter();
            return;
        }

        const num = parseInt(key);
        if (isNaN(num)) {
            return;
        }

        plz_prefix += key;
        updatePlzFilter();
    }

    function updateKeyCity(key) {
        if (key === '\b') {
            if (!city_prefix) {
                loadInitialCityValues();
            }
        }
    }

    function format_distance(distance) {
        let unit = "m";
        let digits = 0;
        if(distance >= 1000) {
            distance /= 1000;
            unit = "km";
            digits = 2;
        }

        return distance.toFixed(digits) + unit;
    }

    function searchForPoi() {
        const target = $('#poi-search-input-target');
        if (!target.val()) {
            return;
        }

        here_api.fetch_pois(target.val()).then(results => {
            const target = $('#nav-wizard-step-poi-result > ul');
            target.empty();
            results.forEach(result => {
                const elem = $('<li></li>');
                const img = $('<img>');
                img.attr('src', result.icon ? result.icon : 'https://download.vcdn.cit.data.here.com/p/d/places2_stg/icons/categories/35.icon');
                elem.append(img);
                elem.append('<span></span>');
                let text = result.title;
                if(result.vicinity) {
                    text += ', ' + result.vicinity.replace('\n', ', ');
                }

                text += ' - ' + format_distance(result.distance);
                elem.find('span').text(text);
                const new_elem = elem.appendTo(target);
                const text_elem = new_elem.find('span');
                let font_size = 30;
                while(text_elem.width() > target.width() - 30 && font_size > 10) {
                    console.log(text_elem.width(), target.width());
                    font_size -= 5;
                    text_elem.attr('style', 'font-size: ' + font_size + 'px');
                }
            });

            switchToWizardStep('nav-wizard-step-poi-result');
        });
    }

    const plz_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch-plz .virtual-keyboard'), (key) => {
        if (is_post_code_input) {
            updateKeyPlz(key);
        } else {
            updateKeyCity(key);
        }
    });

    const street_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch-street .virtual-keyboard'), (key) => {
        onStreetKeyPressed(key);
    });

    const street_num_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch-street-number .virtual-keyboard'), key => {
        onStreetNumKeyPressed(key);
    });

    const poi_keyboard = new VirtualKeyboard($('#nav-wizard-step-poi-search .virtual-keyboard'), key => {
        const target = $('#poi-search-input-target');

        const cur_text = target.val();
        if (key !== '\b') {
            target.val(cur_text + key);
        } else {
            if (!cur_text) {
                return;
            }

            target.val(cur_text.substr(0, cur_text.length - 1));
        }
    });

    plz_keyboard.setLayout(VirtualKeyboard.layouts.NUMBER_ONLY);
    street_keyboard.setLayout(VirtualKeyboard.layouts.REGULAR);
    street_num_keyboard.setLayout(VirtualKeyboard.layouts.REGULAR);
    poi_keyboard.setLayout(VirtualKeyboard.layouts.REGULAR);

    defaultNumberFiltering();

    $('#plz-ch-input-post-or-city-toggle').click(() => {
        is_post_code_input = !is_post_code_input;
        $('#plz-ch-input-post-or-city-toggle').text(is_post_code_input ? "Post Code" : "City");
    });

    $('#nav-find-addr-ch').click(() => {
        switchToWizardStep('nav-wizard-step-addr-ch-plz');
        loadInitialPlzValues();
    });

    $('#nav-find-poi').click(() => {
        switchToWizardStep('nav-wizard-step-poi-search');
    });

    $('#poi-search-trigger').click(searchForPoi);

    $('#nav-back-button').click(() => {
        const visible_step = $('section.nav-section .wizard-body-wrapper > div:not(.hidden)');
        if(visible_step.length === 0) {
            return;
        }

        const cur_step = $(visible_step.eq(0));
        const previous = cur_step.attr('data-previous-step');
        if(!previous) {
            return;
        }

        switchToWizardStep(previous);
    });

    window.on_show_nav_section = function () {

    }
});