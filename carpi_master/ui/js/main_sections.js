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

    $('#map-splash-card').click(() => {
        hide_all_sections();
        $('body > section.map-section').css({
            display: 'block'
        });

        on_show_map_section();
    });

    $('#navigation-splash-card').click(() => {
        hide_all_sections();
        $('body > section.nav-section').css({
            display: 'block'
        });

        on_show_nav_section();
    });

    const visibility_callbacks = {
        "nav-section": () => on_show_nav_section(),
        "map-section": () => on_show_map_section(),
        "engine-section": () => on_show_car_section(),
        "settings-section": () => on_show_settings_sections()
    };

    window.switch_to_splash_section = function (section) {
        hide_all_sections();
        $('body > section.' + section).css({
            display: 'block'
        });

        const callback = visibility_callbacks[section];
        if (callback) {
            callback();
        }
    }
});