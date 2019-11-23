class CsvReader {
    constructor(path) {
        this.promise = new Promise((resolve, reject) => {
            $.get(path).done(data => {
                console.log(data);
            }).fail(() => {
                reject();
            })
        });

    }


    then(success, error) {
        this.promise.then(success, error);
    }
}