$(() => {
    let is_post_code_input = true;

    let plz_ch = null;

    new PlzCsvReader("data/plz_ch.csv").then(info => plz_ch = info);

    function switchToWizardStep(id) {
        $('section.nav-section .wizard-body-wrapper > div:not(.hidden)').addClass('hidden');
        $('#' + id).removeClass('hidden');
    }

    function onCitySelected(plz) {
        console.log(ch_get_addresses(plz));
    }

    function loadInitialRecommendations(elements) {
        for(let i in [...Array(10).keys()]) {
            const row = elements[i];
            const text = `${row.plz} ${row.city} (${row.state_abbrvtn})`;

            const container = $('#nav-wizard-step-addr-ch .item-recommendation');
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
            if(a && b && a.city && b.city) {
                return a.city.localeCompare(b.city);
            }

            if(!a && b) {
                return -1;
            }

            if(a && !b) {
                return 1;
            }

            if(a.city && !b.city) {
                return 1;
            }

            if(!a.city && b.city) {
                return -1;
            }

            return 0;
        });

        loadInitialRecommendations(cities);
    }

    $('#nav-find-addr-ch').click(() => {
        switchToWizardStep('nav-wizard-step-addr-ch-plz');
        loadInitialPlzValues();
    });

    let plz_prefix = '';
    let city_prefix = '';

    function defaultNumberFiltering() {
        plz_keyboard.filterEnabledKeys(key => key === '\b' || !isNaN(parseInt(key)));
    }

    function backspaceOnlyFiltering() {
        plz_keyboard.filterEnabledKeys(key => key === '\b');
    }

    function updatePlzFilter() {
        $('#ch-post-code-input-target').val(plz_prefix);
        const container = $('#nav-wizard-step-addr-ch .item-recommendation');
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
            if(!city_prefix) {
                loadInitialCityValues();
            }
        }
    }

    const plz_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch-plz .virtual-keyboard'), (key) => {
        if(is_post_code_input) {
            updateKeyPlz(key);
        } else {
            updateKeyCity(key);
        }
    });

    plz_keyboard.setLayout(VirtualKeyboard.layouts.REGULAR);
    defaultNumberFiltering();

    $('#plz-ch-input-post-or-city-toggle').click(() => {
        is_post_code_input = !is_post_code_input;
        $('#plz-ch-input-post-or-city-toggle').text(is_post_code_input ? "Post Code" : "City");
    });

    window.on_show_nav_section = function () {

    }
});