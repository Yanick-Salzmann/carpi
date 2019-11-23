$(() => {
    let plz_ch = null;
    const reader = new CsvReader("data/plz_ch.csv");
    reader.then((info) => { plz_ch = info; });

    function switchToWizardStep(id) {
        $('section.nav-section .wizard-body-wrapper > div:not(.hidden)').addClass('hidden');
        $('#' + id).removeClass('hidden');
    }

    $('#nav-find-addr-ch').click(() => {
        switchToWizardStep('nav-wizard-step-addr-ch');
    });

    window.on_show_nav_section = function() {

    }
});