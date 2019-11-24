$(() => {
    let plz_ch = null;
    const reader = new CsvReader("data/plz_ch.csv");
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

    const plz_keyboard = new VirtualKeyboard($('#nav-wizard-step-addr-ch .virtual-keyboard'), (key) => {
        console.log(key);
    });

    window.on_show_nav_section = function () {

    }
});