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
        lines.shift(); // remove header
        lines.forEach(line => {
           this.rows.push(this.get_tokens(line));
        });
    }

    get_tokens(line) {
        const parts = line.split(',');
        return {
            plz: parseInt(parts[0]),
            city: parts[1],
            state: parts[2],
            state_abbrvtn: parts[5],
            country: parts[6]
        }
    }
}