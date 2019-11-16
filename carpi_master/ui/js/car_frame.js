$(() => {
    function draw_indicators(ctx, origin, xofs, yofs, texts) {
        const count = xofs.length;

        for(let i = 0; i < count; ++i) {
            const grad = ((3 * Math.PI) / ((count - 1) * 2)) * i + (3 * Math.PI / 4);
            const textSize = 15;

            ctx.beginPath();
            ctx.font = textSize + 'px sans-serif';
            const text = texts[i];
            const dimension = ctx.measureText(text);

            const tx = origin + 110 * Math.cos(grad) + xofs[i] * dimension.width;
            const ty = origin + 110 * Math.sin(grad) + yofs[i] * textSize;

            ctx.fillText(text, tx, ty);
        }
    }

    function draw_sub_segments(ctx, origin, count, size_func) {
        for(let i = 0; i < count; ++i) {
            const grad = ((3 * Math.PI) / ((count - 1) * 2)) * i + (3 * Math.PI / 4);
            ctx.beginPath();
            const sx = origin + 120 * Math.cos(grad);
            const sy = origin + 120 * Math.sin(grad);

            const width = size_func(i);

            const dx = origin + (120 + width * 15) * Math.cos(grad);
            const dy = origin + (120 + width * 15) * Math.sin(grad);

            ctx.moveTo(sx, sy);
            ctx.lineTo(dx, dy);
            ctx.stroke();
        }
    }

    function draw_gauge_shape(ctx, origin) {
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
    }

    function draw_rpm_gauge(ctx) {
        const origin = 150;
        const xofs = [0,   0,    0.3, -0.3, -0.7, -1.1,   -1.2,  -1.2];
        const yofs = [0.4, 0.45, 0.6,  0.7,  0.7, 0.45, 0.45, 0.4];
        const texts = [ '0', '1', '2', '3', '4', '5', '6', '7'];

        draw_gauge_shape(ctx, origin);
        draw_sub_segments(ctx, origin, 70, index => {
            if((index % 10) === 0) {
                return 1;
            } else if((index % 5) === 0) {
                return 0.5;
            } else {
                return 0.2;
            }
        });
        draw_indicators(ctx, origin, xofs, yofs, texts);
    }

    function draw_speed_gauge(ctx) {
        const origin = 150;
        const xofs = [0.4,   0.3,  0.3,  0.3, 0.0, -0.5,   -0.6,  -1.0, -1.2, -1.2, -1.2, -1.2];
        const yofs = [0.4, 0.45, 0.4,  0.7,  0.7, 0.95, 0.95, 0.8, 0.6, 0.3, 0.2, 0.4];
        const texts = [ '0', '20', '40', '60', '80', '100', '120', '140', '160', '180', '200', '220'];

        draw_gauge_shape(ctx, origin);
        draw_sub_segments(ctx, origin, 110, index => {
            if((index % 10) === 0) {
                return 1;
            } else if((index % 5) === 0) {
                return 0.5;
            } else {
                return 0.2;
            }
        });
        draw_indicators(ctx, origin, xofs, yofs, texts);
    }

    window.on_show_car_section = () => {
        const rpm_gauge = $('#rpm-gauge')[0];
        const speed_gauge = $('#speed-gauge')[0];

        draw_rpm_gauge(rpm_gauge.getContext('2d'));
        draw_speed_gauge(speed_gauge.getContext('2d'));
    }
});