class CsvReader {
    constructor(path) {
        this.rows = [];

        this.promise = new Promise((resolve, reject) => {
            $.get(path).done(data => {
                this.process_lines(data);
                resolve(this.rows);
            }).fail(() => {
                reject();
            })
        });
    }

    then(success, error) {
        this.promise.then(success, error);
    }

    process_lines(data) {
        const lines = data.split(/$/m);
        console.log(lines.length);
    }
}