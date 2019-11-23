$(() => {
    const reader = new CsvReader("data/plz_ch.csv");
    reader.then((info) => { console.log(info); });

    window.on_show_nav_section = function() {

    }
});