#version 130

uniform sampler2D texture;
uniform vec2 resolution;
uniform float time;

// Shader parameters
const float hardScan = -12.0; //-8.0
const float hardPix = -2.0; // -3.0
const vec2 warp = vec2(1.0/64.0, 1.0/48.0); // vec2(1.0/32.0, 1.0/24.0)
const float maskDark = 0.75; // 0.5
const float maskLight = 1.2; // 1.5

// sRGB to Linear
float ToLinear1(float c)
{
  return (c <= 0.04045) ? c / 12.92 : pow((c + 0.055) / 1.055, 2.4);
}

vec3 ToLinear(vec3 c)
{
  return vec3(ToLinear1(c.r), ToLinear1(c.g), ToLinear1(c.b));
}

// Linear to sRGB
float ToSrgb1(float c)
{
  return (c < 0.0031308 ? c * 12.92 : 1.055 * pow(c, 0.41666) - 0.055);
}

vec3 ToSrgb(vec3 c)
{
  return vec3(ToSrgb1(c.r), ToSrgb1(c.g), ToSrgb1(c.b));
}

// Emulated input resolution
vec2 res()
{
  return resolution / 3.0; // 6.0
}

// Fetch sample with offset
vec3 Fetch(vec2 pos, vec2 off)
{
  pos = floor(pos * res() + off) / res();
  if (max(abs(pos.x - 0.5), abs(pos.y - 0.5)) > 0.5)
      return vec3(0.0, 0.0, 0.0);
  return ToLinear(texture2D(texture, pos.xy).rgb);
}

// Distance in emulated pixels to nearest texel
vec2 Dist(vec2 pos)
{
  pos = pos * res();
  return -((pos - floor(pos)) - vec2(0.5));
}

// 1D Gaussian
float Gaus(float pos, float scale)
{
  return exp2(scale * pos * pos);
}

// 3-tap Gaussian filter along horizontal line
vec3 Horz3(vec2 pos, float off)
{
  vec3 b = Fetch(pos, vec2(-1.0, off));
  vec3 c = Fetch(pos, vec2(0.0, off));
  vec3 d = Fetch(pos, vec2(1.0, off));
  float dst = Dist(pos).x;
  float scale = hardPix;
  float wb = Gaus(dst - 1.0, scale);
  float wc = Gaus(dst + 0.0, scale);
  float wd = Gaus(dst + 1.0, scale);
  return (b * wb + c * wc + d * wd) / (wb + wc + wd);
}

// 5-tap Gaussian filter along horizontal line
vec3 Horz5(vec2 pos, float off)
{
  vec3 a = Fetch(pos, vec2(-2.0, off));
  vec3 b = Fetch(pos, vec2(-1.0, off));
  vec3 c = Fetch(pos, vec2(0.0, off));
  vec3 d = Fetch(pos, vec2(1.0, off));
  vec3 e = Fetch(pos, vec2(2.0, off));
  float dst = Dist(pos).x;
  float scale = hardPix;
  float wa = Gaus(dst - 2.0, scale);
  float wb = Gaus(dst - 1.0, scale);
  float wc = Gaus(dst + 0.0, scale);
  float wd = Gaus(dst + 1.0, scale);
  float we = Gaus(dst + 2.0, scale);
  return (a * wa + b * wb + c * wc + d * wd + e * we) / (wa + wb + wc + wd + we);
}

// Return scanline weight
float Scan(vec2 pos, float off)
{
  float dst = Dist(pos).y;
  return Gaus(dst + off, hardScan);
}

// Allow nearest three lines to affect pixel
vec3 Tri(vec2 pos)
{
  vec3 a = Horz3(pos, -1.0);
  vec3 b = Horz5(pos, 0.0);
  vec3 c = Horz3(pos, 1.0);
  float wa = Scan(pos, -1.0);
  float wb = Scan(pos, 0.0);
  float wc = Scan(pos, 1.0);
  return a * wa + b * wb + c * wc;
}

// Distortion of scanlines
vec2 Warp(vec2 pos)
{
  pos = pos * 2.0 - 1.0;
  pos *= vec2(1.0 + (pos.y * pos.y) * warp.x, 1.0 + (pos.x * pos.x) * warp.y);
  return pos * 0.5 + 0.5;
}

// Shadow mask
vec3 Mask(vec2 pos)
{
  pos.x += pos.y * 3.0;
  vec3 mask = vec3(maskDark, maskDark, maskDark);
  pos.x = fract(pos.x / 6.0);
  if (pos.x < 0.333)
      mask.r = maskLight;
  else if (pos.x < 0.666)
      mask.g = maskLight;
  else
      mask.b = maskLight;
  return mask;
}

void main()
{
  vec2 pos = Warp(gl_TexCoord[0].xy);
  gl_FragColor.rgb = ToSrgb(Tri(pos) * Mask(gl_FragCoord.xy));
  gl_FragColor.a = 1.0;
}
