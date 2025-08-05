#ifdef __EMSCRIPTEN__
#define RESOURCES(x) "res-web/" #x
#else
#define RESOURCES(x) "res/" #x
#endif

#include <glad.h>
#include <stdio.h>
#include <string.h>
#include <HandmadeMath.h>

#include "font.h"
#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "state.h"

static Texture fontTexture;
static int fontShader;
static int fontInitialized = 0;

// Batch rendering data
#define FONT_MAX_CHARACTERS_PER_BATCH 1000
#define FONT_VERTICES_PER_CHAR 6
#define FONT_FLOATS_PER_VERTEX 7  // position(2) + texcoord(2) + color(3)

static unsigned int fontVAO, fontVBO;
static float batchVertices[FONT_MAX_CHARACTERS_PER_BATCH * FONT_VERTICES_PER_CHAR * FONT_FLOATS_PER_VERTEX];
static int batchCharCount = 0;

void initFont()
{
    if (fontInitialized) return;
    
    // Load the font texture
    fontTexture = loadTexture(upheaval.texturePath);
    
    // Load custom font shader with per-vertex colors
    fontShader = createShaderProgramS(RESOURCES(shaders/font.vert), RESOURCES(shaders/font.frag));

    // Create VAO and VBO for batch rendering
    glGenVertexArrays(1, &fontVAO);
    glGenBuffers(1, &fontVBO);
    
    glBindVertexArray(fontVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
    
    // Allocate buffer for dynamic data
    glBufferData(GL_ARRAY_BUFFER, sizeof(batchVertices), NULL, GL_DYNAMIC_DRAW);
    
    // Set up vertex attributes
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    
    // Texture coordinate attribute  
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    
    // Color attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));
    
    glBindVertexArray(0);
    
    fontInitialized = 1;
}

Character getChar(char c)
{
    // Handle characters outside the printable ASCII range
    if (c < 32 || c > 126) {
        c = ' '; // Default to space for invalid characters
    }
    return font_chars[c - 32];
}

void flushFontBatch()
{
    if (batchCharCount == 0) return;
    
    // Set up rendering state
    useShader(fontShader);
    
    // Use the same projection setup as sprites
    HMM_Vec2 projDims = getProjectionDimensions();
    HMM_Mat4 projection = HMM_Orthographic_RH_NO(0.0f, projDims.X, projDims.Y, 0.0f, -1.0f, 1.0f);
    setUniformMat4("projection", projection, fontShader);
    
    HMM_Mat4 view = HMM_Translate((HMM_Vec3){(float)-state.camX, (float)-state.camY, 0.0f});
    setUniformMat4("view", view, fontShader);
    
    // Identity model matrix for batch (characters are already positioned in world space)
    HMM_Mat4 model = HMM_M4D(1.0f);
    setUniformMat4("model", model, fontShader);
    
    // Bind font texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexture.id);
    setUniformInt1("fontTexture", 0, fontShader);
    
    // Upload vertex data
    glBindVertexArray(fontVAO);
    glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batchCharCount * FONT_VERTICES_PER_CHAR * FONT_FLOATS_PER_VERTEX * sizeof(float), batchVertices);
    
    // Render all characters in one draw call
    glDrawArrays(GL_TRIANGLES, 0, batchCharCount * FONT_VERTICES_PER_CHAR);
    
    glBindVertexArray(0);
    
    // Reset batch
    batchCharCount = 0;
}

void addCharToBatch(Font font, char c, HMM_Vec2 position, HMM_Vec3 color, float scale)
{
    if (batchCharCount >= FONT_MAX_CHARACTERS_PER_BATCH) {
        flushFontBatch(); // Flush if batch is full
    }
    
    Character ch = getChar(c);
    
    // Calculate the position and size for this character
    HMM_Vec2 charPos = {
        position.X + ch.originX * scale,
        position.Y + ch.originY * scale
    };
    
    HMM_Vec2 charSize = {
        ch.width * scale,
        ch.height * scale
    };
    
    // Set up texture coordinates for this character
    float texLeft = (float)ch.x / (float)font.width;
    float texRight = (float)(ch.x + ch.width) / (float)font.width;
    float texTop = (float)ch.y / (float)font.height;
    float texBottom = (float)(ch.y + ch.height) / (float)font.height;
    
    // Calculate world positions for the character quad
    float left = charPos.X;
    float right = charPos.X + charSize.X;
    float top = charPos.Y;
    float bottom = charPos.Y + charSize.Y;
    
    // Add vertices to batch (6 vertices for 2 triangles)
    int vertexIndex = batchCharCount * FONT_VERTICES_PER_CHAR * FONT_FLOATS_PER_VERTEX;
    
    // Triangle 1
    // Bottom-left
    batchVertices[vertexIndex++] = left;
    batchVertices[vertexIndex++] = bottom;
    batchVertices[vertexIndex++] = texLeft;
    batchVertices[vertexIndex++] = texBottom;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    // Top-right
    batchVertices[vertexIndex++] = right;
    batchVertices[vertexIndex++] = top;
    batchVertices[vertexIndex++] = texRight;
    batchVertices[vertexIndex++] = texTop;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    // Top-left
    batchVertices[vertexIndex++] = left;
    batchVertices[vertexIndex++] = top;
    batchVertices[vertexIndex++] = texLeft;
    batchVertices[vertexIndex++] = texTop;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    // Triangle 2
    // Bottom-left
    batchVertices[vertexIndex++] = left;
    batchVertices[vertexIndex++] = bottom;
    batchVertices[vertexIndex++] = texLeft;
    batchVertices[vertexIndex++] = texBottom;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    // Bottom-right
    batchVertices[vertexIndex++] = right;
    batchVertices[vertexIndex++] = bottom;
    batchVertices[vertexIndex++] = texRight;
    batchVertices[vertexIndex++] = texBottom;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    // Top-right
    batchVertices[vertexIndex++] = right;
    batchVertices[vertexIndex++] = top;
    batchVertices[vertexIndex++] = texRight;
    batchVertices[vertexIndex++] = texTop;
    batchVertices[vertexIndex++] = color.R;
    batchVertices[vertexIndex++] = color.G;
    batchVertices[vertexIndex++] = color.B;
    
    batchCharCount++;
}

void drawChar(Font font, char c, HMM_Vec2 position, HMM_Vec3 color, float scale)
{
    if (!fontInitialized) {
        initFont();
    }
    
    // Add character to batch instead of rendering immediately
    addCharToBatch(font, c, position, color, scale);
}

void drawString(Font font, const char* str, HMM_Vec2 position, HMM_Vec3 color, float scale)
{
    if (!fontInitialized) {
        initFont();
    }
    
    if (!str) return;
    
    HMM_Vec2 currentPos = position;
    int len = strlen(str);
    
    for (int i = 0; i < len; i++) {
        char c = str[i];
        
        // Handle newlines
        if (c == '\n') {
            currentPos.X = position.X;
            currentPos.Y += font.size * scale;
            continue;
        }
        
        // Handle carriage returns
        if (c == '\r') {
            currentPos.X = position.X;
            continue;
        }
        
        // Handle tabs (approximate with 4 spaces)
        if (c == '\t') {
            Character spaceChar = getChar(' ');
            currentPos.X += spaceChar.advance * scale * 4;
            continue;
        }
        
        // Draw the character
        drawChar(font, c, (HMM_Vec2){currentPos.X + scale/0.5f, currentPos.Y + scale/0.5f}, (HMM_Vec3){color.X - 0.5f, color.Y - 0.5f, color.Z - 0.5f}, scale);
        drawChar(font, c, currentPos, color, scale);
        
        // Advance position for next character
        Character ch = getChar(c);
        currentPos.X += ch.advance * scale;
    }
}

HMM_Vec2 measureString(Font font, const char* str, float scale)
{
    if (!str) return (HMM_Vec2){0, 0};
    
    float width = 0;
    float height = font.size * scale;
    float currentLineWidth = 0;
    float maxWidth = 0;
    int lines = 1;
    
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        char c = str[i];
        
        if (c == '\n') {
            maxWidth = HMM_MAX(maxWidth, currentLineWidth);
            currentLineWidth = 0;
            lines++;
            continue;
        }
        
        if (c == '\r') {
            maxWidth = HMM_MAX(maxWidth, currentLineWidth);
            currentLineWidth = 0;
            continue;
        }
        
        if (c == '\t') {
            Character spaceChar = getChar(' ');
            currentLineWidth += spaceChar.advance * scale * 4;
            continue;
        }
        
        Character ch = getChar(c);
        currentLineWidth += ch.advance * scale;
    }
    
    maxWidth = HMM_MAX(maxWidth, currentLineWidth);
    height = lines * font.size * scale;
    
    return (HMM_Vec2){maxWidth, height};
}

void cleanupFont()
{
    if (fontInitialized) {
        // Flush any remaining characters
        flushFontBatch();
        
        // Clean up OpenGL resources
        glDeleteVertexArrays(1, &fontVAO);
        glDeleteBuffers(1, &fontVBO);
        
        // Font texture cleanup is handled by texture system
        fontInitialized = 0;
    }
}
