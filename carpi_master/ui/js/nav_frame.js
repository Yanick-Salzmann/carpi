$(() => {
    let plz_ch = null;
    const reader = new PlzCsvReader("data/plz_ch.csv");
    reader.then((info) => {
        plz_ch = info;
    });

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

    function updatePlzFilter() {
        $('#ch-post-code-input-target').val(plz_prefix);
        const container = $('#nav-wizard-step-addr-ch .item-recommendation');
        const len = plz_prefix.length;
        if (len > 4) {
            container.empty();
            return;
        }

        if (!len) {
            loadInitialPlzValues();
            return;
        }

        const search_plz = parseInt(plz_prefix);
        if (isNaN(search_plz)) {
            return;
        }

        container.empty();

        const dividend = Math.pow(10, 4 - len);

        let num_results = 0;
        for (let i = 0; i < plz_ch.length && num_results < 10; ++i) {
            const key = Math.floor(plz_ch[i].plz / dividend);
            if (key !== search_plz) {
                continue;
            }

            const row = plz_ch[i];
            const text = `${row.plz} ${row.city} (${row.state_abbrvtn})`;
            const parent = $('<div class="recommendation"></div>');
            parent.text(text);
            container.append(parent);
            ++num_results;
        }
    }

    const plz_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch .virtual-keyboard'), (key) => {
        if (key === '\b') {
            if (!plz_prefix.length) {
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

    plz_keyboard.setLayout(VirtualKeyboard.layouts.NUMBER_ONLY);

    window.on_show_nav_section = function () {

    }
});