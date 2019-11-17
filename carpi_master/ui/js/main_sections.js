$(() => {
    function hide_all_sections() {
        $('body > section').css({
            display: 'none'
        });
    }

    function show_splash_sections() {
        $('body > section.splash-section').css({
            display: 'block'
        });
    }

    $('#settings-splash-card').click(() => {
        hide_all_sections();
        $('body > section.settings-section').css({
            display: 'block'
        });

        on_show_settings_sections();
    });

    $('#settings-back-button, #engine-back-button').click(() => {
        hide_all_sections();
        show_splash_sections();
    });

    $('#engine-splash-card').click(() => {
        hide_all_sections();
        $('body > section.engine-section').css({
            display: 'block'
        });

        on_show_car_section();
    });
});