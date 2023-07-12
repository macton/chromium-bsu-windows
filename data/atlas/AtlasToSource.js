const fs = require('fs');

let hpp_text = `
#pragma once
// *** GENERATED FILE ***
// Generated from atlas.json

enum ImageIndex
{
IMAGE_INDEX_NAME_LIST
};

extern float  g_ImageTexCoords[][4];
extern GLuint g_AtlasTexture;

void AtlasDrawSprite( int image_index, float pos_x, float pos_y, float scale_x = 1.0f, float scale_y = 1.0f );
`;

let cpp_text = `
// *** GENERATED FILE ***
// Generated from atlas.json

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "../src/compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "../src/define.h"

#include "Atlas.hpp"

float g_ImageTexCoords[][4] = {
IMAGE_TEX_COORDS
};

void AtlasDrawSprite( int image_index, float pos_x, float pos_y, float scale_x, float scale_y )
{
  const float szx = scale_x;
  const float szy = scale_y;
  const float u0 = g_ImageTexCoords[image_index][0];
  const float v0 = g_ImageTexCoords[image_index][1];
  const float u1 = g_ImageTexCoords[image_index][2];
  const float v1 = g_ImageTexCoords[image_index][3];

  glBindTexture(GL_TEXTURE_2D, g_AtlasTexture);
  glPushMatrix();
  glTranslatef(pos_x, pos_y, HERO_Z);
  glBegin(GL_TRIANGLE_STRIP);
  glTexCoord2f(u1, v0); glVertex3f(szx, szy, 0.0);
  glTexCoord2f(u0, v0); glVertex3f(-szx, szy, 0.0);
  glTexCoord2f(u1, v1); glVertex3f(szx, -szy, 0.0);
  glTexCoord2f(u0, v1); glVertex3f(-szx, -szy, 0.0);
  glEnd();
  glPopMatrix();
}
`;

const atlas_text  = fs.readFileSync('atlas.json');
const atlas       = JSON.parse( atlas_text );
const atlas_w     = atlas.meta.size.w;
const atlas_h     = atlas.meta.size.h;
const frames      = atlas.frames;
const image_names = Object.keys( frames );

hpp_text = hpp_text.replace("IMAGE_INDEX_NAME_LIST", image_names.map( name => '  k' + name ).join(",\n"));
cpp_text = cpp_text.replace("IMAGE_TEX_COORDS", image_names.map( name => {
  const rect = frames[name].frame;
  const u0   = rect.x / atlas_w;
  const v0   = rect.y / atlas_h;
  const u1   = (rect.x + rect.w) / atlas_w;
  const v1   = (rect.y + rect.h) / atlas_h;
  return `  // k${name} ${rect.x} ${rect.y} ${rect.x+rect.w} ${rect.y+rect.h}\n` + 
  `  { ${u0.toFixed(8)}, ${v0.toFixed(8)}, ${u1.toFixed(8)}, ${v1.toFixed(8)} }`;
}).join(",\n"));

fs.writeFileSync("../../generated_src/Atlas.hpp",hpp_text);
fs.writeFileSync("../../generated_src/Atlas.cpp",cpp_text);
