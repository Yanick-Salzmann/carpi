$(() => {
    let plz_ch = null;
    const reader = new CsvReader("data/plz_ch.csv");
    reader.then((info) => { plz_ch = info; });

    $('#nav-find-addr-ch').click(() => {

    });

    window.on_show_nav_section = function() {

    }
});