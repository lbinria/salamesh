#version 440 core

// Avoid the disable of earlier Depth-testing
layout(depth_less) out float gl_FragDepth;

// Color output
layout(location = 0) out vec4 FragColor;

flat in int FragHalfedgeIndex;

in vec3 fragWorldPos;
uniform bool is_clipping_enabled = false;
uniform vec3 clipping_plane_normal; // (a, b, c)
uniform vec3 clipping_plane_point;  // A point on the plane
uniform int invert_clipping = 0; // 0: normal, 1: inverted

uniform vec3 hoverColor = vec3(1.,1.,1.);
uniform vec3 selectColor = vec3(0., 0.22, 1.);

in vec2 vLocalUV;  // u in [0..1] across thickness, v in [0..1] along length

uniform vec3 uColorInside = vec3(0.0, 0.97, 0.73);
uniform vec3 uColorOutside = vec3(0.0, 0.6, 0.45);

// Note: cannot index samplerBuffer with dynamic indexing !
uniform sampler2D colormap0;
uniform sampler2D colormap1;
uniform sampler2D colormap2;

uniform samplerBuffer filterBuf;
uniform samplerBuffer highlightBuf;
uniform samplerBuffer colormap0Buf;
uniform samplerBuffer colormap1Buf;
uniform samplerBuffer colormap2Buf;

uniform vec2 attrRange[3];
uniform int attrRepeat[3] = {1, 1, 1};
uniform int attrNDims[3] = {1, 1, 1};

uniform int colormapElement[3] = {-1, -1, -1};
uniform int highlightElement;
uniform int filterElement;

in mat4 fragView;
in mat4 plane;

vec3 encode_id(int id) {
    int r = id & 0x000000FF;
    int g = (id & 0x0000FF00) >> 8;
    int b = (id & 0x00FF0000) >> 16;
    return vec3(r / 255.f, g / 255.f, b / 255.f); 
}

float fetchLayer(int idx, int layer) {
    if (layer == 0) return texelFetch(colormap0Buf, idx).x;
    if (layer == 1) return texelFetch(colormap1Buf, idx).x;
    if (layer == 2) return texelFetch(colormap2Buf, idx).x;
    if (layer == 3) return texelFetch(highlightBuf, idx).x;
    if (layer == 4) return texelFetch(filterBuf, idx).x;
    return 0.;
}

vec4 fetchColormap(int layer, vec2 coords) {
    if (layer == 0) return texture(colormap0, coords);
    if (layer == 1) return texture(colormap1, coords);
    if (layer == 2) return texture(colormap2, coords);
    return vec4(0.);
}

vec4 getLayerColor(int idx, int layer) {

    vec2 range = attrRange[layer];
    int nRepeat = attrRepeat[layer];
    int nDims = attrNDims[layer];
    // bool automap = getLayerAutomap(layer);

    float rangeLength = range.y - range.x;
    float rangeRepeat = rangeLength / nRepeat;


    vec2 coords = vec2(0.);
    for (int d = 0; d < nDims; d++) {

        float val = fetchLayer(idx * nDims + d, layer).x;
        if (nDims == 1) {
            // Apply range
            // val = (mod(attrVal - attrRange.x, rangeRepeat + 1)) / rangeRepeat;
            val = (val - range.x) / rangeLength; // Simple auto map range version with no repeat
        }
        
        float v = clamp(val, 0., 1.);
        coords[d] = v;
    }
    
    return fetchColormap(layer, coords);
}

float sdfEquilateralTriangle(vec2 p) {
    float k = sqrt(3.);
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0 / k;
    if (p.x+k*p.y > 0) p = vec2(p.x - k*p.y, -k*p.x-p.y) / 2.;
    p.x -= clamp(p.x, -2., 0.);
    return -length(p)*sign(p.y);
}

void _filter(inout vec3 col) {
    if (filterElement == -1)
        return;

    bool filtered = texelFetch(filterBuf, FragHalfedgeIndex).x > 0;

    if (filtered)
        discard;
}

void clip(inout vec3 col) {
   // Calculate the distance from the cell barycenter to the plane
   if (is_clipping_enabled) {
      float distance = dot(clipping_plane_normal, fragWorldPos - clipping_plane_point) / length(clipping_plane_normal);
      
      if ((invert_clipping == 0 && distance < 0.0) || (invert_clipping == 1 && distance >= 0.0)) {
         discard;
      }
   }
}

// vertical cylinder
float sdCylinder( vec3 p, float h, float r )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - vec2(r,h);
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdCone( vec3 p, vec2 c, float h )
{
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c,h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y,-1.0);
    
  vec2 w = vec2( length(p.xz), p.y );
  vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
  vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
  float k = sign( q.y );
  float d = min(dot( a, a ),dot(b, b));
  float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
  return sqrt(d)*sign(s);
}

float map( in vec3 pos )
{
    float coneHeight = 1.;
    float coneRadius = .5;
    //vec3 conePos = vec3(cos(iTime),sin(iTime), 0.1);
    vec3 arrowPos = vec3(0.,0, 0.);
    vec3 conePos = vec3(0., 1., 0.);
    vec3 cylinderPos = vec3(0., -0.0, 0.);
    //float sdfCone = sdCone(pos, conePos, coneHeight, coneRadius);
    float sdfCone = sdCone(pos - (arrowPos + conePos), vec2(sin(radians(25.)), cos(radians(25.))), 0.6);
    float sdfCylinder = sdCylinder(pos - (arrowPos + cylinderPos), 0.4, 0.2);
    return min(sdfCylinder, sdfCone);
    //return sdfCone;
}

vec3 calcNormal(vec3 hp) {
    // Normal calculation using gradient approximation
    //float eps = 0.01;
    const float eps = 0.0005;
    return normalize(vec3(
        map(hp + vec3(eps, 0, 0)) - 
        map(hp - vec3(eps, 0, 0)),
        map(hp + vec3(0, eps, 0)) - 
        map(hp - vec3(0, eps, 0)),
        map(hp + vec3(0, 0, eps)) - 
        map(hp - vec3(0, 0, eps))
    )) /*/ (2.0 * eps)*/;
}


vec3 trace(vec2 uv, inout vec3 col) {

    // // Combined rotation and initial camera position
    // vec3 ro = vec3(0, 0, 1.);
    
    // vec3 ta = vec3( 0.0, 0.0, 0.0 );
    // // camera matrix
    // vec3 ww = normalize( ta - ro );
    // vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    // vec3 vv = normalize( cross(uu,ww));

    // // Orthographic ray origin 
    // // Scale uv to control view size (lower value = zoomed in)
    // float orthoScale = 1.0; // Adjust this to zoom in/out
    // ro = ro + uv.x * uu * orthoScale + uv.y * vv * orthoScale;
    // // Orthographic ray direction (always looking forward)
    // vec3 rd = ww;

    // vec3 ro = vec3(uv, -1.);
    // vec3 rd = vec3(0,0,1.);

	mat4 simulatedView = mat4(
        vec4(1, 0, 0, 0),   // Right vector (X-axis)
        vec4(0, 1, 0, 0),   // Up vector (Y-axis)
        vec4(0, 0, -1, 0),  // Look vector (negated for right-handed system)
        vec4(0, 0, 0, 1)    // Translation row (no translation)
    );

	mat4 v = mat4(
		fragView[0].xyz, 0,
		fragView[1].xyz, 0,
		fragView[2].xyz, 0,
		0, 0, 0, 1
	);

	// mat3 pp = mat3(
	// 	plane[0].xyz, 
	// 	plane[1].xyz, 
	// 	plane[2].xyz
	// );

	// Rotate 90
	// uv = vec2(uv.y, uv.x);

    vec3 ro = vec3(uv, -1.);
    vec3 rd = vec3(0,0,1.);


	mat4 x = simulatedView;
	// mat4 x = simulatedView * plane;
	// mat4 x = simulatedView;
	// mat4 x = v;
    ro = (x * vec4(ro, 1.0)).xyz;
    rd = (x * vec4(rd, 0.0)).xyz;



	// mat3 x = v * pp;
    // ro = (x * ro).xyz;
    // rd = (x * rd).xyz;


    // raymarch
    const float tmax = 3.0;
    float t = 0.0;
    for( int i=0; i<256; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map(pos);
        if( h<0.0001 || t>tmax ) break;
        t += h;
    }

    // shading/lighting	
    if( t<tmax )
    {
        vec3 pos = ro + t*rd;
        // vec3 nor = calcNormal(pos);
        
		vec3 N = calcNormal(pos);
        float dif = clamp( dot(N,vec3(0.57703, 0.12, -0.43)), 0.0, 1.0 );
        float amb = 0.5 + 0.5*dot(N,vec3(0.0,1.0,0.0));
        //col = vec3(0.2,0.3,0.4)*amb + vec3(0.8,0.7,0.5)*dif;
		//col = vec3(0.2,0.3,0.4);
		return N;
    } else {
		discard;
	}

    // // dist from border: center is u=0.5
    // float d = abs(vLocalUV.x - 0.5) - 0.5;  
    // float e = fwidth(d);
    // float t = smoothstep(e, -e, d);

    // // optional hard discard of outside
    // if(t<0.01) discard;

    // // Point coord from [0, 1] to [-1, 1]
    // vec2 V = 2.0 * (gl_PointCoord - vec2(0.5, 0.5));
    
    // // Distance from line center: center is u=0.5
    // float dc = abs(vLocalUV.x - 0.5);
    // float z = sqrt(dc);
    // vec3 N = vec3(V.x, -V.y, z);

    // // Update depth according to radius
    // gl_FragDepth = gl_FragCoord.z - 0.00005 * N.z;

    // return vec4(N, t);
}

void highlight(inout vec3 col) {
    if (highlightElement == -1) 
        return;

    // Highlight
    float highlightVal = texelFetch(highlightBuf, FragHalfedgeIndex).x;

    if (highlightVal > 0) {
        // Interpolate between hover / select colors according to highlight value
        float t = highlightVal;
        // Add condition if you want a hard switch between hover and select
        t = step(0.5, highlightVal);
        vec3 hlColor = mix(hoverColor, selectColor, t);
        // Mix with current point color (80%)
        col = mix(col, hlColor, .8);
    }
}

void shading(inout vec3 col, vec3 N) {
    float light = 1. - dot(N, vec3(0.35,0.45,1.)) /** .5 + .5*/;
    col *= light * 0.5 + 0.5;
}

vec4 showColormap(int layer) {
    int kind = colormapElement[layer];
    
    if (kind == 2 /* corners */) {
        return getLayerColor(FragHalfedgeIndex, layer);
    } 
    
    if (kind == -1 /* no element => layer deactivated */) {
        return vec4(0., 0., 0., -1.);
    }

    return vec4(1., 0., 0., 1.);
}

// Mix with alpha discard
vec4 blendMix(vec4 c1, vec4 c2, float t) {
    if (c1.a <= -1.)
        return c2;
    if (c2.a <= -1.)
        return c1;
    // Second term is equals to 0. => discard
    if (c2.a <= 0.)
        return vec4(0.);
    // Else, mix colors
    return mix(c1, c2, t);
}

void main()
{
    vec3 col = uColorInside;
    _filter(col);
    clip(col);

	
	vec2 uv = vLocalUV;
    vec3 N = trace(uv, col);
    
	
	// vec3 N = vec3(Nt);
    // float t = Nt.w;

    // col = mix(uColorOutside, uColorInside, t);

    // Show colormap data if activated
    vec4 c[3];
    c[0] = showColormap(0);
    c[1] = showColormap(1);
    c[2] = showColormap(2);

    // Blend
    vec4 b = blendMix(c[0], blendMix(c[1], c[2], .5), .5);

    if (b.a > -1.) {
        if (b.a > 0.)
            col = b.rgb;
        else 
            discard;
    }

    highlight(col);
    shading(col, N);

    FragColor = vec4(col, 1.);
}