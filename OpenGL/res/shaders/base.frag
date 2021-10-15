#version 450 core

uniform sampler2D galaxyTex; //https://unsplash.com/photos/Oze6U2m1oYU
uniform vec2 u_Resolution;
uniform float u_AspectRatio;
uniform float u_Time;
uniform float u_LongerTime;
uniform float u_RoadSlidingLinesTime;
out vec4 FragColor;

float signedTriangleArea(vec2 v1, vec2 v2, vec2 v3) {
    //https://www.shadertoy.com/view/ssjSzW
    float det = v2.x*v3.y + v3.x*v1.y + v1.x*v2.y - v2.x*v1.y - v1.x*v3.y - v3.x*v2.y;
    return .5*det;
}

vec4 roadGlassCanvas(vec2 uv){
    vec4 col = vec4(0);
    vec4 A = vec4(0, 0, .3, 1);
    float a = signedTriangleArea(uv, vec2(1, 0), vec2(.5, .21));
    float b = signedTriangleArea(uv, vec2(.5, .21), vec2(0));
    if(a > 0 && b > 0) col = A;
    return step(uv.y, .2) * col;
}

vec4 terrain() {
    //heavily modified code from https://www.shadertoy.com/view/4tsGD7
    //Did a lot of optimization and changed it to my "synthwave" taste :)
    vec3 currentFragment = vec3(gl_FragCoord.xy/u_Resolution - .5, u_AspectRatio - .5);
    currentFragment.y += .3; //rotating the cam. upwards
    vec3 camera = vec3(-1.57, .1, u_LongerTime);

    for (float i = 0; i < 9; i += .01) { //Render distance
        camera += currentFragment * i * .3; //hint: camera = floor(camera) gives voxel (minecraft) terrain
        if (cos(camera.z) + sin(camera.x) > camera.y) { //shapes the terrain into small mountains
            return vec4(vec3(.1, .02, 0) / i, 1);
        }
    }
}

float finiteLine(vec2 P, vec2 A, vec2 B){
    vec2 g = B - A;
    vec2 h = P - A;
    return length(h - g * clamp(dot(g, h) / dot(g, g), 0, 1));
    //(smooth)step cannot control bloom thickness but only the line thickness
    //that's why we are not using it here
}

float slidingLine(vec2 P, vec2 A, vec2 B, vec2 direction) {
    P -= B;//Normalize P for some unknown reason
    B *= u_RoadSlidingLinesTime*direction;
    A *= u_RoadSlidingLinesTime*direction;
    A.y *= 1.2;//Length of the line over time
    return finiteLine(P, A, B);
}

void main() {
    //Normalizing coords
    vec2 uv = gl_FragCoord.xy / u_Resolution;
    vec2 center_uv = (gl_FragCoord.xy * 2 - u_Resolution.xy) / u_Resolution.y;
    vec2 texCoord = vec2(gl_FragCoord.xy) / vec2(textureSize(galaxyTex, 0));
    texCoord.y *= 1.25;
    
    //Positions and bloom thickness
    vec2 leftCenterJoint = vec2(-.1, -.6);
    vec2 rightCenterJoint = vec2(.1, -.6);
    float bloomThiccness = .001;

    //Let's set a color for the lines and a synthwave terrain!
    vec4 lineColor = vec4(1, .6, 0, 1);
    vec4 color = vec4(0);
    vec4 bloomyLines = color;
    color += terrain();

    //Let's also add a texture as a background and a road canvas!
    color = mix(color, texture(galaxyTex, texCoord), 1-color.a); //this needs to be run before bloomyLines merges with color!
    color += roadGlassCanvas(uv);    
    
    //Road borders
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(-1.8, -1), leftCenterJoint));
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(-1.5, -1), leftCenterJoint));
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(1.8, -1), rightCenterJoint));
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(1.5, -1), rightCenterJoint));

    //Road end
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, leftCenterJoint, rightCenterJoint));

    //Road
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(0, -.6 - u_Time), vec2(0, -.65 - u_Time)));
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(0, -.75 - u_Time), vec2(0, -.8 - u_Time)));
    bloomyLines += lineColor*(bloomThiccness/finiteLine(center_uv, vec2(0, -.9 - u_Time), vec2(0, -.95 - u_Time)));

    //Sliding lines
    bloomyLines += lineColor*(bloomThiccness/slidingLine(center_uv, rightCenterJoint, rightCenterJoint, vec2(25.5,1)));
    bloomyLines += lineColor*(bloomThiccness/slidingLine(center_uv, leftCenterJoint, leftCenterJoint, vec2(25.5,1)));

    //let's put it on the screen!
    FragColor = color + bloomyLines;
};