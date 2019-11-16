$(() => {
    function draw_gauge(elem) {
        const ctx = elem.getContext('2d');
        ctx.shadowBlur = 10;
        ctx.shadowColor = 'white';
        ctx.fillStyle = "white";
        ctx.strokeStyle = "white";
        ctx.lineWidth = 2;

        ctx.beginPath();
        ctx.arc(150, 150, 120, 3 * Math.PI / 4, Math.PI/4);
        ctx.stroke();
        ctx.arc(150, 150, 135, Math.PI/4, 3 * Math.PI / 4, true);
        ctx.stroke();
        ctx.arc(150, 150, 120, 3 * Math.PI / 4, 3 * Math.PI / 4);
        ctx.stroke();
    }

    $('canvas.gauge').each((_, value) => draw_gauge(value));
});