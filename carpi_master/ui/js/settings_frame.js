$(() => {
    window.obd_connected = false;

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
        const deck = $('#bluetooth-device-deck');
        deck.empty();

        reload_button.css({
            display: 'none'
        });

        const progress = $('section.settings-section #obd-bluetooth-settings > .card-body > h4 > span.bluetooth-loader');
        progress.css({
            display: 'inline'
        });

        event_manager.submitTask('bluetooth_search').then((resp) => {

            if (!resp) {
                progress.css({
                    display: 'none'
                });

                reload_button.css({
                    display: 'initial'
                });

                return;
            }

            resp.forEach(dev => {
                const card = $('<div class="card"></div>');
                const header = $('' +
                    '<div class="card-header">' +
                    '   <span class="device-text"></span>' +
                    '   <span class="bluetooth-loader float-right"><i class="fas fa-spinner"></i></span>' +
                    '   <span class="device-status float-right"><i class="fa fa-check"></i><i class="fa fa-times"></i></span>' +
                    '</div>'
                );
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
                    });

                    event_manager.submitTask('obd_connect', {address: dev.address}).then((status) => {
                        new_card.find('span.device-status')
                            .removeClass(['connected-device', 'disconnected-device'])
                            .addClass(status.success ? 'connected-device' : 'disconnected-device');

                        new_card.find('.card-header .bluetooth-loader').css({display: 'none'});
                        window.obd_connected = status.success;
                        localStorage.setItem('obd.last_device', dev.address);
                    });
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

    window.on_show_settings_sections = () => {
        scan_devices();
    };

    reload_button.click(() => {
        scan_devices();
    });
});