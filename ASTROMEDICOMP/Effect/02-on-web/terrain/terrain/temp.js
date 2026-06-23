"use strict";

const NUM_ROWS = 113;
const NUM_COLUMNS = 113;

// Attribute Locations
const AMC_ATTRIBUTE_POSITION = 0;
const AMC_ATTRIBUTE_TEXCORD = 2;

const terrain = {
    vao: null,
    vbo_position: null,
    vbo_TexCoordRectangle: null,
    numVertices: 0
};

function initializeTerrain(gl) {
    const positions = [];
    const texCoords = [];

    for (let i = 0; i < NUM_ROWS; i++) {
        for (let j = 0; j < NUM_COLOUMS; j++) {
            // Calculate coordinates
            let x0 = (i / NUM_ROWS) - 0.5;
            let y0 = (j / NUM_COLOUMS) - 0.5;
            let x1 = ((i + 1.0) / NUM_ROWS) - 0.5;
            let y1 = ((j + 1.0) / NUM_COLOUMS) - 0.5;

            let u0 = i / NUM_ROWS;
            let v0 = j / NUM_COLOUMS;
            let u1 = (i + 1.0) / NUM_ROWS;
            let v1 = (j + 1.0) / NUM_COLOUMS;

            // Define 2 triangles for 1 quad (Counter-Clockwise)
            // Triangle 1: a, b, c
            // Triangle 2: a, c, d

            // Positions (x, y, z)
            const pA = [x0, y1, 0.0], pB = [x0, y0, 0.0], pC = [x1, y0, 0.0], pD = [x1, y1, 0.0];
            positions.push(...pA, ...pB, ...pC, ...pA, ...pC, ...pD);

            // TexCoords (u, v)
            const tA = [u0, v1], tB = [u0, v0], tC = [u1, v0], tD = [u1, v1];
            texCoords.push(...tA, ...tB, ...tC, ...tA, ...tC, ...tD);
        }
    }

    terrain.numVertices = positions.length / 3;

    // Create VAO (WebGL 2 syntax)
    terrain.vao = gl.createVertexArray();
    gl.bindVertexArray(terrain.vao);

    // Position Buffer
    terrain.vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, terrain.vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
    gl.vertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    // TexCoord Buffer
    terrain.vbo_TexCoordRectangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, terrain.vbo_TexCoordRectangle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(texCoords), gl.STATIC_DRAW);
    gl.vertexAttribPointer(AMC_ATTRIBUTE_TEXCORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(AMC_ATTRIBUTE_TEXCORD);

    gl.bindVertexArray(null);
}

function drawTerrain(gl) {
    if (terrain.vao) {
        gl.bindVertexArray(terrain.vao);
        gl.drawArrays(gl.TRIANGLES, 0, terrain.numVertices);
        gl.bindVertexArray(null);
    }
}

function updateTerrain() {
    // Logic for animation/updates
}

function uninitializeTerrain(gl) {
    if (terrain.vbo_TexCoordRectangle) {
        gl.deleteBuffer(terrain.vbo_TexCoordRectangle);
        terrain.vbo_TexCoordRectangle = null;
    }
    if (terrain.vbo_position) {
        gl.deleteBuffer(terrain.vbo_position);
        terrain.vbo_position = null;
    }
    if (terrain.vao) {
        gl.deleteVertexArray(terrain.vao);
        terrain.vao = null;
    }
}