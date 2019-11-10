$(() => {
    const canvas = document.querySelector('#video-canvas');
    const gl = canvas.getContext('webgl');
    let texture = null;
    let program = null;
    let uniform_texture = null;
    let vertex_buffer = null;
    let index_buffer = null;

    function on_resize(width, height) {
        canvas.setAttribute('width', width.toString());
        canvas.setAttribute('height', height.toString());

        gl.viewport(0, 0, width, height);
    }

    function setup_texture() {
        texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 2, 2, 0, gl.RGBA, gl.UNSIGNED_BYTE, Uint8Array.of(
            0, 0, 0xFF, 0xFF,
            0, 0xFF, 0, 0xFF,
            0xFF, 0, 0, 0xFF,
            0x3F, 0x7F, 0xFF, 0xFF));
    }

    function compile_shader(shader, code) {
        gl.shaderSource(shader, code);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            const error_log = gl.getShaderInfoLog(shader);
            console.error("Error compiling shader:", error_log);
            throw "Error compiling shader";
        }
    }

    function setup_program() {
        program = gl.createProgram();
        const vs = gl.createShader(gl.VERTEX_SHADER);
        const fs = gl.createShader(gl.FRAGMENT_SHADER);

        compile_shader(vs, `   
        precision lowp float;
             
        attribute vec2 position0;
        attribute vec2 texCoord0;
        
        varying vec2 texCoord;
        
        void main() {
            gl_Position = vec4(position0, 0.0, 1.0);
            texCoord = texCoord0;
        }
        `);

        compile_shader(fs, `
        precision lowp float;
        
        varying vec2 texCoord;
        
        uniform sampler2D video_texture;
        
        void main() {
            gl_FragColor = texture2D(video_texture, texCoord).bgra;
        }
        `);

        gl.attachShader(program, vs);
        gl.attachShader(program, fs);

        gl.linkProgram(program);
        if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            console.error("Error linking program:", gl.getProgramInfoLog(program));
            throw "Error linking program";
        }

        uniform_texture = gl.getUniformLocation(program, "video_texture");

        gl.useProgram(program);
    }

    function setup_data() {
        vertex_buffer = gl.createBuffer();
        index_buffer = gl.createBuffer();

        gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
        gl.bufferData(gl.ARRAY_BUFFER,
            Float32Array.of(
                -1, -1, 0, 0,
                1, -1, 1, 0,
                1, 1, 1, 1,
                -1, 1, 0, 1
            ),
            gl.STATIC_DRAW
        );

        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, index_buffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, Uint8Array.of(0, 1, 2, 0, 2, 3), gl.STATIC_DRAW);

        gl.activeTexture(gl.TEXTURE0);
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.uniform1i(uniform_texture, 0);
    }

    function setup_bindings() {
        const pos_attrib = gl.getAttribLocation(program, "position0");
        const texcoord_attrib = gl.getAttribLocation(program, "texCoord0");

        gl.enableVertexAttribArray(pos_attrib);
        gl.enableVertexAttribArray(texcoord_attrib);

        gl.vertexAttribPointer(pos_attrib, 2, gl.FLOAT, false, 16, 0);
        gl.vertexAttribPointer(texcoord_attrib, 2, gl.FLOAT, false, 16, 8);
    }

    function on_frame() {
        gl.clear(gl.COLOR_BUFFER_BIT);
        gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_BYTE, 0);

        requestAnimationFrame(on_frame);
    }

    function fetch_frame() {
        setInterval(() => {
            const frame_data = fetch_raw_frame();
            const arr = new Uint8Array(frame_data);
            const slice = arr.slice(0, arr.length);
            gl.bindTexture(gl.TEXTURE_2D, texture);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 480, 368, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(frame_data));
        }, 0);
    }

    gl.clearColor(1.0, 0.5, 0.25, 1.0);

    setup_texture();
    setup_program();
    setup_data();
    setup_bindings();

    event_manager.submitTask('camera_parameters').then((data) => {
        console.log("Resize", data.width, 'x', data.height);
        event_manager.submitTask('camera_capture').then(() => {});
        on_resize(data.width, data.height);
        on_frame();
        fetch_frame();
    });

});