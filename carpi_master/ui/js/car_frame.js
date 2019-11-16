$(() => {
    const canvas_origin = 150;

    function draw_pin_root(ctx, origin) {
        ctx.beginPath();
        ctx.shadowBlur = 10;
        ctx.shadowColor = '#111';
        ctx.fillStyle = '#111';
        ctx.arc(origin, origin, 20, 0, Math.PI * 2);
        ctx.fill();
    }

    function draw_pin(ctx, origin, cur_val, max_val) {
        const pct = cur_val / max_val;
        const grad = (3 * Math.PI / 2.0) * pct + (3 * Math.PI / 4);
        const grad_l = grad - Math.PI / 2.0;
        const grad_r = grad + Math.PI / 2.0;

        const bx = origin - 10 * Math.cos(grad);
        const by = origin - 10 * Math.sin(grad);

        const ex = origin + 130 * Math.cos(grad);
        const ey = origin + 130 * Math.sin(grad);

        const blx = origin + 5 * Math.cos(grad_l);
        const bly = origin + 5 * Math.sin(grad_l);

        const brx = origin + 5 * Math.cos(grad_r);
        const bry = origin + 5 * Math.sin(grad_r);

        ctx.beginPath();
        ctx.moveTo(bx, by);
        ctx.lineTo(blx, bly);
        ctx.lineTo(ex, ey);
        ctx.lineTo(brx, bry);
        ctx.lineTo(bx, by);

        ctx.fillStyle = 'red';
        ctx.shadowColor = 'red';
        ctx.fill();
    }

    function draw_indicators(ctx, origin, xofs, yofs, texts) {
        const count = xofs.length;

        for (let i = 0; i < count; ++i) {
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
        for (let i = 0; i < count; ++i) {
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
        ctx.arc(origin, origin, 120, 3 * Math.PI / 4, Math.PI / 4);
        ctx.stroke();
        ctx.arc(origin, origin, 135, Math.PI / 4, 3 * Math.PI / 4, true);
        ctx.stroke();
        ctx.arc(origin, origin, 120, 3 * Math.PI / 4, 3 * Math.PI / 4);
        ctx.stroke();
    }

    function draw_rpm_gauge(ctx, rpm) {
        ctx.clearRect(0, 0, 300, 300);
        const xofs = [0, 0, 0.3, -0.3, -0.7, -1.1, -1.2, -1.2];
        const yofs = [0.4, 0.45, 0.6, 0.7, 0.7, 0.45, 0.45, 0.4];
        const texts = ['0', '1', '2', '3', '4', '5', '6', '7'];

        draw_gauge_shape(ctx, canvas_origin);
        draw_sub_segments(ctx, canvas_origin, 70, index => {
            if ((index % 10) === 0) {
                return 1;
            } else if ((index % 5) === 0) {
                return 0.5;
            } else {
                return 0.2;
            }
        });
        draw_indicators(ctx, canvas_origin, xofs, yofs, texts);
        draw_pin_root(ctx, canvas_origin);
        draw_pin(ctx, canvas_origin, rpm, 7000);
    }

    function draw_speed_gauge(ctx, speed) {
        ctx.clearRect(0, 0, 300, 300);
        const xofs = [0.4, 0.3, 0.3, 0.3, 0.0, -0.5, -0.6, -1.0, -1.2, -1.2, -1.2, -1.2];
        const yofs = [0.4, 0.45, 0.4, 0.7, 0.7, 0.95, 0.95, 0.8, 0.6, 0.3, 0.2, 0.4];
        const texts = ['0', '20', '40', '60', '80', '100', '120', '140', '160', '180', '200', '220'];

        draw_gauge_shape(ctx, canvas_origin);
        draw_sub_segments(ctx, canvas_origin, 110, index => {
            if ((index % 10) === 0) {
                return 1;
            } else if ((index % 5) === 0) {
                return 0.5;
            } else {
                return 0.2;
            }
        });
        draw_indicators(ctx, canvas_origin, xofs, yofs, texts);
        draw_pin_root(ctx, canvas_origin);
        draw_pin(ctx, canvas_origin, speed, 220);

        ctx.beginPath();
    }

    const rpm_gauge = $('#rpm-gauge')[0];
    const speed_gauge = $('#speed-gauge')[0];

    const ctx_rpm = rpm_gauge.getContext('2d');
    const ctx_speed = speed_gauge.getContext('2d');

    function update_rpm(rpm) {
        draw_rpm_gauge(ctx_rpm, Math.min(Math.max(0, rpm), 7000));
    }

    function update_speed(speed) {
        draw_speed_gauge(ctx_speed, Math.min(Math.max(0, speed), 220));
        const text = (speed.toFixed(2));
        ctx_speed.beginPath();
        const dimension = ctx_speed.measureText(text);
        const dim_static = ctx_speed.measureText("km/h");
        ctx_speed.fillStyle = 'white';
        ctx_speed.shadowColor = 'white';
        ctx_speed.fillText(text, 150 - dimension.width / 2.0, 240);
        ctx_speed.fillText("km/h", 150 - dim_static.width / 2.0, 260);
    }

    let last_interval = null;

    const times  = [0, 1000, 1500, 1800, 2000, 2400, 2700, 2800, 3200, 3500, 4000];
    const rpm_values = [0, 1800, 2400, 2300, 2100, 1500, 1400,  1300,  1500, 750, 0];
    const speed_values = [0, 80, 120, 100, 90, 130, 170, 200, 180, 140, 90, 0];

    window.on_show_car_section = () => {
        if(last_interval) {
            clearInterval(last_interval);
        }

        update_rpm(0);
        update_speed(0);

        let start_time = (new Date()).getTime();

        setInterval(() => {
            const diff = ((new Date()).getTime() - start_time) % 4000;
            let index = times.length - 2;

            for(let i = 0; i < (times.length - 1); ++i) {
                if(times[i] <= diff && times[i + 1] >= diff) {
                    index = i;
                    break;
                }
            }

            const start = times[index];
            const end = times[index + 1];

            const rv0 = rpm_values[index];
            const rv1 = rpm_values[index + 1];
            const sv0 = speed_values[index];
            const sv1 = speed_values[index + 1];

            const fac = (diff - start) / (end - start);

            update_rpm(rv0 + fac * (rv1 - rv0));
            update_speed(sv0 + fac * (sv1 - sv0));
        }, 10);
    }
});