$(() => {
    let is_post_code_input = true;

    let plz_ch = null;
    let ch_addresses = null;

    new PlzCsvReader("data/plz_ch.csv").then(info => plz_ch = info);
    new ChAddressesCsvReader("data/CH_addresses.csv").then(info => ch_addresses = info);

    function switchToWizardStep(id) {
        $('section.nav-section .wizard-body-wrapper > div:not(.hidden)').addClass('hidden');
        $('#' + id).removeClass('hidden');
    }

    function loadInitialPlzValues() {
        for (let i in [...Array(10).keys()]) {
            const row = plz_ch[i];
            const text = `${row.plz} ${row.city} (${row.state_abbrvtn})`;

            const container = $('#nav-wizard-step-addr-ch .item-recommendation');
            const parent = $('<div class="recommendation"></div>');
            parent.text(text);
            container.append(parent);
        }
    }

    $('#nav-find-addr-ch').click(() => {
        switchToWizardStep('nav-wizard-step-addr-ch');
        loadInitialPlzValues();
    });

    let plz_prefix = '';

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

        let next_chars = [];

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
                parent.text(text);
                container.append(parent);
                ++num_results;
            }

            if (len < 4) {
                const next_num = Math.floor((plz_ch[i].plz * 10) / dividend) % 10;
                if (next_chars.indexOf(next_num) < 0) {
                    next_chars.push(next_num);
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

                return next_chars.indexOf(num_key) >= 0;
            })
        }
    }

    const plz_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch .virtual-keyboard'), (key) => {
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
    });

    plz_keyboard.setLayout(VirtualKeyboard.layouts.REGULAR);
    defaultNumberFiltering();

    $('#plz-ch-input-post-or-city-toggle').click(() => {
        is_post_code_input = !is_post_code_input;
        $('#plz-ch-input-post-or-city-toggle').text(is_post_code_input ? "Post Code" : "City");
    });

    window.on_show_nav_section = function () {
        console.log(ch_addresses);
    }
});