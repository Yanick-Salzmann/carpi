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
});