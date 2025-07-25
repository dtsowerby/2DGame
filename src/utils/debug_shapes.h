#pragma once

#include <HandmadeMath.h>

// Initialize debug shapes rendering system
void initDebugShapes();

// Cleanup debug shapes resources
void cleanupDebugShapes();

// Draw a 2D circle with debug lines
// center: center position of the circle
// radius: radius of the circle
// segments: number of line segments to use (higher = smoother circle)
// color: RGB color of the lines
void drawDebugCircle(HMM_Vec2 center, float radius, int segments, HMM_Vec3 color);

// Draw a 2D square with debug lines
// center: center position of the square
// size: size of the square (width and height)
// color: RGB color of the lines
void drawDebugSquare(HMM_Vec2 center, float size, HMM_Vec3 color);

// Draw a 2D rectangle with debug lines
// center: center position of the rectangle
// width: width of the rectangle
// height: height of the rectangle
// color: RGB color of the lines
void drawDebugRect(HMM_Vec2 center, float width, float height, HMM_Vec3 color);

// Flush all debug lines to be rendered
// This should be called after all debug shapes have been queued
void flushDebugShapes();
