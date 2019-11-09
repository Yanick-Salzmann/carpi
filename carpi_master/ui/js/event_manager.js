$(() => {
    class EventManager {
        constructor() {
            this.event_handlers = {};

            sendCefRequest({
                request: "EventHandlerInit",
                persistent: true,
                onSuccess: (data) => {
                    const element = JSON.parse(data);
                    const type = element.type;
                    const body = element.body;
                    const handlers = this.event_handlers[type];
                    if (handlers) {
                        handlers.forEach(handler => handler(body));
                    }
                }
            })
        }

        registerEvent(type, callback) {
            let handlers = this.event_handlers[type];
            if (!handlers) {
                handlers = [];
            }

            handlers.push(callback);
            this.event_handlers[type] = handlers;
        }

        submitTask(type, payload) {
            return new Promise((resolve, reject) => {
                sendCefRequest({
                    request: JSON.stringify({type: type, request: payload}),
                    onSuccess: (data) => {
                        const response = JSON.parse(data);
                        if (response.type !== type) {
                            console.warn("Event type in response not matching: " + response.type + " vs " + type);
                        }

                        resolve(response.body);
                    },
                    onFailure: (error_code, error_string) => {
                        reject({code: error_code, error: error_string});
                    }
                })
            });
        }
    }

    window.event_manager = new EventManager();
});