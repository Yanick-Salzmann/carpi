$(() => {
    function draw_gauge(elem) {
        const ctx = elem.getContext('2d');
        const origin = 150;
        const inner = 120;
        const outer = 135;

        ctx.shadowBlur = 10;
        ctx.shadowColor = 'white';
        ctx.fillStyle = "white";
        ctx.strokeStyle = "white";
        ctx.lineWidth = 2;

        ctx.beginPath();
        ctx.arc(origin, origin, inner, 3 * Math.PI / 4, Math.PI/4);
        ctx.stroke();
        ctx.arc(origin, origin, outer, Math.PI/4, 3 * Math.PI / 4, true);
        ctx.stroke();
        ctx.arc(origin, origin, inner, 3 * Math.PI / 4, 3 * Math.PI / 4);
        ctx.stroke();

        for(let i = 0; i < 7; ++i) {
            const grad = ((3 * Math.PI) / 14) * i + (3 * Math.PI / 4);
            ctx.beginPath();
            const sx = origin + inner * Math.cos(grad);
            const sy = origin + inner * Math.sin(grad);

            const dx = origin + outer * Math.cos(grad);
            const dy = origin + outer * Math.sin(grad);

            ctx.moveTo(sx, sy);
            ctx.lineTo(dx, dy);
            ctx.stroke();
        }
    }

    $('canvas.gauge').each((_, value) => draw_gauge(value));
});