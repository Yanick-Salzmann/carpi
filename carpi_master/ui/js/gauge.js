$(() => {
    function draw_gauge(elem) {
        const ctx = elem.getContext('2d');
        const origin = 150;

        ctx.shadowBlur = 10;
        ctx.shadowColor = 'white';
        ctx.fillStyle = "white";
        ctx.strokeStyle = "white";
        ctx.lineWidth = 2;

        ctx.beginPath();
        ctx.arc(origin, origin, 120, 3 * Math.PI / 4, Math.PI/4);
        ctx.stroke();
        ctx.arc(origin, origin, 135, Math.PI/4, 3 * Math.PI / 4, true);
        ctx.stroke();
        ctx.arc(origin, origin, 120, 3 * Math.PI / 4, 3 * Math.PI / 4);
        ctx.stroke();

        const xofs = [0,   0,    0.3, -0.3, -0.7, -1.1,   -1.2,  -1.2];
        const yofs = [0.4, 0.45, 0.6,  0.7,  0.7, 0.45, 0.3, 0.4];
        const texts = [ '0', '1', '2', '3', '4', '5', '6', '7'];

        for(let i = 0; i < 8; ++i) {
            const grad = ((3 * Math.PI) / 14) * i + (3 * Math.PI / 4);
            ctx.beginPath();
            const sx = origin + 120 * Math.cos(grad);
            const sy = origin + 120 * Math.sin(grad);

            const dx = origin + 135 * Math.cos(grad);
            const dy = origin + 135 * Math.sin(grad);

            ctx.moveTo(sx, sy);
            ctx.lineTo(dx, dy);
            ctx.stroke();

            const textSize = 15;

            ctx.beginPath();
            ctx.font = textSize + 'px sans-serif'
            const text = texts[i];
            const dimension = ctx.measureText(text);

            const tx = origin + 110 * Math.cos(grad) + xofs[i] * dimension.width;
            const ty = origin + 110 * Math.sin(grad) + yofs[i] * textSize;

            ctx.fillText(text, tx, ty);
        }
    }

    $('canvas.gauge').each((_, value) => draw_gauge(value));
});