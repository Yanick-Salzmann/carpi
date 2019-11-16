$(() => {
    function draw_gauge(elem) {
        const ctx = elem.getContext('2d');
        ctx.shadowBlur = 5;
        ctx.shadowColor = 'white';
        ctx.fillStyle = "white";
        ctx.strokeStyle = "white";
        ctx.lineWidth = 1;

        ctx.beginPath();
        ctx.arc(150, 150, 120, 3 * Math.PI / 4, Math.PI/4);
        ctx.stroke();
        ctx.arc(150, 150, 130, Math.PI/4, 3 * Math.PI / 4, true);
        ctx.stroke();
        ctx.arc(150, 150, 120, 3 * Math.PI / 4, 3 * Math.PI / 4);
        ctx.stroke();
    }

    $('canvas.gauge').each(cvs => draw_gauge(cvs));
});