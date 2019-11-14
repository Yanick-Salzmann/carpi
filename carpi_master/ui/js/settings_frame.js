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

    function scan_devices() {
        const progress = $('section.settings-section span.bluetooth-loader');
        progress.css({
            display: 'inline'
        });

        event_manager.submitTask('bluetooth_search').then((resp) => {
            const deck = $('#bluetooth-device-deck');
            deck.empty();

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
        })
    }

    scan_devices();
});