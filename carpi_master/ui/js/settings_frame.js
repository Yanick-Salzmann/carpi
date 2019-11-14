$(() => {
    const settings = $('section.settings-section > ul > li');

    settings.click((e) => {
        const li_elem = $(e.target).parents('li');
        if (li_elem.hasClass('expanded')) {
            return;
        }

        settings.removeClass('expanded');
        li_elem.addClass('expanded');
    });
});