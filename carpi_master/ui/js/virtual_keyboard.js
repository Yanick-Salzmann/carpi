$(() => {
    const regular_layout = [
        ['\u00A7', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '^', '\u2190'],
        ['\u2b0c', 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', '\u00DC', '\u00A8'],
        ['\u21ea', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\u00D6', '\u00C4', '$'],
        ['\u21e7', '<', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '-', '\u21e7'],
    ];

    const number_layout = [
        ['\u2190', '/', '*', '-'],
        ['7', '8', '9', '+'],
        ['4', '5', '6', ' '],
        ['1', '2', '3', ' '],
        ['0', '.', ',', ' ']
    ];

    const special_key_mapping = {
        '\u2190': '\b'
    };

    window.VirtualKeyboard = class VirtualKeyboard {
        static layouts = {
            REGULAR: 0,
            NUMBER_ONLY: 1
        };

        constructor(element, callback) {
            this.kb = element;
            this.callback = callback;
            this.setLayout(VirtualKeyboard.layouts.REGULAR);
        }

        filterEnabledKeys(filter) {
            const keys = this.kb.find('span.vkb-key');
            for (let i = 0; i < keys.length; ++i) {
                const key = $(keys.eq(i));
                let key_char = key.find('div.content').text();
                if(key_char in special_key_mapping) {
                    key_char = special_key_mapping[key_char];
                }

                const isEnabled = filter(key_char);
                if (isEnabled) {
                    key.removeClass('disabled');
                } else {
                    if (!key.hasClass('disabled')) {
                        key.addClass('disabled');
                    }
                }
            }
        }

        setLayout(layout) {
            switch (layout) {
                case VirtualKeyboard.layouts.REGULAR: {
                    this.activateLayout(regular_layout, 14);
                    break;
                }

                case VirtualKeyboard.layouts.NUMBER_ONLY: {
                    this.activateLayout(number_layout, 4);
                    break;
                }

                default: {
                    throw "Invalid layout specified, must be 0 (REGULAR) or 1 (NUMBER_ONLY)";
                }
            }
        }

        activateLayout(layout, width) {
            this.kb.empty();

            const wrapper = $('<div class="vkb-wrapper"></div>');
            for (let row_idx = 0; row_idx < layout.length; ++row_idx) {
                const row = layout[row_idx];
                const row_wrapper = $('<div class="vkb-row-wrapper"></div>');
                for (let key_idx = 0; key_idx < row.length; ++key_idx) {
                    const key = row[key_idx];
                    const cur_key = $('<span class="vkb-key keys-' + width + '"></span>');
                    const cur_content = $('<div class="content"></div>');
                    cur_key.click(() => {
                        if (key === '\u2190') {
                            this.callback('\b');
                        } else {
                            this.callback(key);
                        }
                    });
                    cur_content.text(key);
                    cur_key.append(cur_content);
                    row_wrapper.append(cur_key);
                }
                wrapper.append(row_wrapper);
            }

            this.kb.append(wrapper);
        }
    }
});