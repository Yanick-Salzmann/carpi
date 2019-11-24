class ChAddressesCsvReader {
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
        lines.forEach(line => this.rows.push(this.get_tokens(line)));
    }

    get_tokens(line) {
        const parts = line.split(';');
        // EGID;EDID;GDEKT;GDENR;GDENAME;STRNAME;DEINR;PLZ4;PLZZ;PLZNAME;GKODE;GKODN;STRSP
        return {
            street: parts[5],
            number: parts[6],
            plz: parts[7],
            city: parts[9],
            east: parts[10],
            north: parts[11]
        }
    }
}