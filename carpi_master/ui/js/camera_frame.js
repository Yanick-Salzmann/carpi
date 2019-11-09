$(() => {
    const canvas = document.querySelector('#video-canvas');
    const gl = canvas.getContext('webgl');
    let texture = null;
    let program = null;
    let uniform_texture = null;

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
        if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
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
            gl_FragColor = texture2D(video_texture, texCoord);
        }
        `);

        gl.attachShader(program, vs);
        gl.attachShader(program, fs);

        gl.linkProgram(program);
        if(!gl.getProgramParameter(program, gl.LINK_STATUS)) {
            console.error("Error linking program:", gl.getProgramInfoLog(program));
            throw "Error linking program";
        }

        uniform_texture = gl.getUniformLocation(program, "video_texture");
    }

    gl.clearColor(1.0, 0.5, 0.25, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT);

    setup_texture();
    setup_program();

    let fps = 30;
    let width = 480;
    let height = 360;

    on_resize(width, height);

    event_manager.submitTask('camera_parameters', (data) => {
        fps = data.fps;
        on_resize(data.width, data.height);
    });
});