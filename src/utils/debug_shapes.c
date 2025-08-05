#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <glad.h>
#include <HandmadeMath.h>

#include "utils/debug_shapes.h"

#include "gfx/shader.h"
#include "gfx/sprite_renderer.h"

#include "state.h"

#define MAX_DEBUG_LINES 2000
#define DEBUG_LINE_VERTEX_COUNT (MAX_DEBUG_LINES * 2)

typedef struct DebugLine {
    HMM_Vec2 start;
    HMM_Vec2 end;
    HMM_Vec3 color;
} DebugLine;

typedef struct DebugShapesState {
    unsigned int shaderProgram;
    unsigned int VAO;
    unsigned int VBO;
    DebugLine lines[MAX_DEBUG_LINES];
    int lineCount;
    int initialized;
} DebugShapesState;

static DebugShapesState debugState = {0};

// Simple debug line vertex shader
static const char* debugVertexShader = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "out vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * vec4(aPos, 0.0, 1.0);\n"
    "    vertexColor = aColor;\n"
    "}\n";

// Simple debug line fragment shader
static const char* debugFragmentShader = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec3 vertexColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(vertexColor, 1.0);\n"
    "}\n";

static unsigned int compileShader(const char* source, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Debug shapes shader compilation failed: %s\n", infoLog);
    }
    
    return shader;
}

static unsigned int createDebugShaderProgram() {
    unsigned int vertexShader = compileShader(debugVertexShader, GL_VERTEX_SHADER);
    unsigned int fragmentShader = compileShader(debugFragmentShader, GL_FRAGMENT_SHADER);
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Debug shapes shader program linking failed: %s\n", infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

void initDebugShapes() {
    if (debugState.initialized) {
        return;
    }
    
    debugState.shaderProgram = createDebugShaderProgram();
    debugState.lineCount = 0;
    
    // Generate VAO and VBO
    glGenVertexArrays(1, &debugState.VAO);
    glGenBuffers(1, &debugState.VBO);
    
    glBindVertexArray(debugState.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, debugState.VBO);
    
    // Allocate buffer for maximum number of lines
    glBufferData(GL_ARRAY_BUFFER, DEBUG_LINE_VERTEX_COUNT * 5 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    
    // Position attribute (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color attribute (3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    debugState.initialized = 1;
    
    printf("Debug shapes initialized\n");
}

void cleanupDebugShapes() {
    if (!debugState.initialized) {
        return;
    }
    
    glDeleteVertexArrays(1, &debugState.VAO);
    glDeleteBuffers(1, &debugState.VBO);
    glDeleteProgram(debugState.shaderProgram);
    
    debugState.initialized = 0;
}

static void addDebugLine(HMM_Vec2 start, HMM_Vec2 end, HMM_Vec3 color) {
    if (debugState.lineCount >= MAX_DEBUG_LINES) {
        printf("Warning: Maximum debug lines reached (%d)\n", MAX_DEBUG_LINES);
        return;
    }
    
    debugState.lines[debugState.lineCount].start = start;
    debugState.lines[debugState.lineCount].end = end;
    debugState.lines[debugState.lineCount].color = color;
    debugState.lineCount++;
}

void drawDebugCircle(HMM_Vec2 center, float radius, int segments, HMM_Vec3 color) {
    if (!debugState.initialized) {
        printf("Warning: Debug shapes not initialized. Call initDebugShapes() first.\n");
        return;
    }
    
    if (segments < 3) {
        segments = 3;
    }
    
    float angleStep = 2.0f * HMM_PI / segments;
    
    for (int i = 0; i < segments; i++) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;
        
        HMM_Vec2 point1 = {
            center.X + cosf(angle1) * radius,
            center.Y + sinf(angle1) * radius
        };
        
        HMM_Vec2 point2 = {
            center.X + cosf(angle2) * radius,
            center.Y + sinf(angle2) * radius
        };
        
        addDebugLine(point1, point2, color);
    }
}

void drawDebugSquare(HMM_Vec2 center, float size, HMM_Vec3 color) {
    drawDebugRect(center, size, size, color);
}

void drawDebugRect(HMM_Vec2 center, float width, float height, HMM_Vec3 color) {
    if (!debugState.initialized) {
        printf("Warning: Debug shapes not initialized. Call initDebugShapes() first.\n");
        return;
    }
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    // Calculate the four corners
    HMM_Vec2 topLeft = { center.X - halfWidth, center.Y - halfHeight };
    HMM_Vec2 topRight = { center.X + halfWidth, center.Y - halfHeight };
    HMM_Vec2 bottomRight = { center.X + halfWidth, center.Y + halfHeight };
    HMM_Vec2 bottomLeft = { center.X - halfWidth, center.Y + halfHeight };
    
    // Draw the four sides
    addDebugLine(topLeft, topRight, color);       // Top
    addDebugLine(topRight, bottomRight, color);   // Right
    addDebugLine(bottomRight, bottomLeft, color); // Bottom
    addDebugLine(bottomLeft, topLeft, color);     // Left
}

void flushDebugShapes() {
    if (!debugState.initialized || debugState.lineCount == 0) {
        return;
    }
    
    // Prepare vertex data
    float* vertices = (float*)malloc(debugState.lineCount * 2 * 5 * sizeof(float)); // 2 vertices per line, 5 floats per vertex
    if (!vertices) {
        printf("Error: Failed to allocate memory for debug shape vertices\n");
        return;
    }
    
    int vertexIndex = 0;
    for (int i = 0; i < debugState.lineCount; i++) {
        DebugLine* line = &debugState.lines[i];
        
        // Start vertex
        vertices[vertexIndex++] = line->start.X;
        vertices[vertexIndex++] = line->start.Y;
        vertices[vertexIndex++] = line->color.R;
        vertices[vertexIndex++] = line->color.G;
        vertices[vertexIndex++] = line->color.B;
        
        // End vertex
        vertices[vertexIndex++] = line->end.X;
        vertices[vertexIndex++] = line->end.Y;
        vertices[vertexIndex++] = line->color.R;
        vertices[vertexIndex++] = line->color.G;
        vertices[vertexIndex++] = line->color.B;
    }
    
    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, debugState.VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, debugState.lineCount * 2 * 5 * sizeof(float), vertices);
    
    // Set up matrices
    useShader(debugState.shaderProgram);
    
    
    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, debugState.shaderProgram);
    
    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});
    setUniformMat4("view", view, debugState.shaderProgram);
    
    // Render lines
    glBindVertexArray(debugState.VAO);
    glDrawArrays(GL_LINES, 0, debugState.lineCount * 2);
    glBindVertexArray(0);
    
    // Cleanup
    free(vertices);
    debugState.lineCount = 0; // Reset for next frame
}
