$(() => {
    const layout = [
        ['\u00A7', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '^', '\u2190'],
        ['\u2b0c', 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', '\u00DC', '\u00A8'],
        ['\u21ea', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\u00D6', '\u00C4', '$'],
        ['\u21e7', '<', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '-', '\u21e7'],
    ];

    $('div.virtual-keyboard').each(function() {
        const kb = $(this);
        const wrapper = $('<div class="vkb-wrapper"></div>');
        for(let row_idx in layout) {
            const row = layout[row_idx];
            const row_wrapper = $('<div class="vkb-row-wrapper"></div>');
            for(let key_idx in row) {
                const key = row[key_idx];
                const cur_key = $('<span class="vkb-key"></span>');
                const cur_content = $('<div class="content"></div>');
                cur_content.text(key);
                cur_key.append(cur_content);
                row_wrapper.append(cur_key);
            }
            wrapper.append(row_wrapper);
        }

        kb.append(wrapper);
    })
});