$(() => {
    const here_config = get_env_value('HERE_APP_ID', 'HERE_APP_CODE');
    if (here_config['HERE_APP_ID']) {
        $('#here-api-app').val(here_config['HERE_APP_ID']);
    }

    if (here_config['HERE_APP_CODE']) {
        $('#here-api-code').val(here_config['HERE_APP_CODE']);
    }
});