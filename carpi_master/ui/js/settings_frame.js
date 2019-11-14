$(() => {
    const env_config = get_env_value('HERE_APP_ID', 'HERE_APP_CODE', 'TOMTOM_API_KEY');
    if (env_config['HERE_APP_ID']) {
        $('#here-api-app').val(env_config['HERE_APP_ID']);
    }

    if (env_config['HERE_APP_CODE']) {
        $('#here-api-code').val(env_config['HERE_APP_CODE']);
    }

    if (env_config['TOMTOM_API_KEY']) {
        $('#tomtom-api-key').val(env_config['TOMTOM_API_KEY']);
    }

    const reload_button = $('section.settings-section span.bluetooth-refresh');

    function scan_devices() {
        reload_button.css({
            display: 'none'
        });

        const progress = $('section.settings-section #obd-bluetooth-settings > .card-body > h4 > span.bluetooth-loader');
        progress.css({
            display: 'inline'
        });

        event_manager.submitTask('bluetooth_search').then((resp) => {
            const deck = $('#bluetooth-device-deck');
            deck.empty();
            if(!resp) {
                return;
            }

            resp.forEach(dev => {
                const card = $('<div class="card"></div>');
                const header = $('<div class="card-header"><span class="device-text"></span><span class="bluetooth-loader float-right"><i class="fas fa-spinner"></i></span></div>');
                header.find('.device-text').text(dev.name);
                const body = $('<div class="card-body"><p class="card-text"></p></div>');
                body.find('p').text(dev.address);
                card.append(header);
                card.append(body);
                const new_card = card.appendTo(deck);
                new_card.click(() => {
                    deck.find('div.card .card-header .bluetooth-loader').css({
                        display: 'none'
                    });

                    new_card.find('.card-header .bluetooth-loader').css({
                        display: 'inline'
                    })
                })
            });

            progress.css({
                display: 'none'
            });

            reload_button.css({
                display: 'initial'
            })
        })
    }

    function hide_all_sections() {
        $('body > section').css({
            display: 'none'
        });
    }

    function show_settings_sections() {
        $('body > section.settings-section').css({
            display: 'block'
        });

        scan_devices();
    }

    function show_splash_sections() {
        $('body > section.splash-section').css({
            display: 'block'
        });
    }

    scan_devices();

    reload_button.click(() => {
        scan_devices();
    });

    $('#settings-splash-card').click(() => {
        hide_all_sections();
        show_settings_sections();
    });

    $('#settings-back-button').click(() => {
        hide_all_sections();
        show_splash_sections();
    })
});