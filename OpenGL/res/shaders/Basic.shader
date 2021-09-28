#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
};


#shader fragment
#version 450 core

uniform float u_Time;
uniform float u_LongerTime;
uniform vec2 u_Resolution;
uniform float u_AspectRatio;
uniform sampler2D galaxyTex; //https://unsplash.com/photos/Oze6U2m1oYU

out vec4 FragColor;

vec4 terrain() {
    //heavily modified code from https://www.shadertoy.com/view/4tsGD7
    //Did a lot of optimization and changed it to my "synthwave" taste :)
    vec3 currentFragment = vec3(gl_FragCoord.xy/u_Resolution - .5, u_AspectRatio - .5);
    currentFragment.y += 0.3; //rotating the cam. upwards
    vec3 camera = vec3(-1.57, .1, u_LongerTime);

    for (float i = .0; i < 9.; i += .01) { //Render distance
        camera += currentFragment * i * .3; //hint: camera = floor(camera) gives voxel (minecraft) terrain
        if (cos(camera.z) + sin(camera.x) > camera.y) { //shapes the terrain into small mountains
            return vec4(vec3(.1, .02, 0) / i, 1.);
        }
    }
}

float finiteLine(vec2 P, vec2 A, vec2 B){
    vec2 g = B - A;
    vec2 h = P - A;
    return smoothstep(.005, .0, length(h - g * clamp(dot(g, h) / dot(g, g), .0, 1.)));
}

/* Perfect for lines with changing angles
float LeftInfiniteLine(vec2 st, float altitude, float angle) {
    return step(st.x, .5) * smoothstep(.002, .0, abs((.0 - st.y + altitude + st.x / (2 * angle)))); //step()* = if true, apply smoothstep()
}

float RightInfiniteLine(vec2 st, float altitude, float angle) {
    return step(.5, st.x) * smoothstep(.002, .0, abs(st.y - .5 + altitude + st.x / (2 * angle)));
}*/

void main() {
    //Normalizing coords
    //vec2 uv = gl_FragCoord.xy / u_Resolution;
    vec2 center_uv = (gl_FragCoord.xy * 2.0 - u_Resolution.xy) / u_Resolution.y;
    vec2 texCoord = vec2(gl_FragCoord.xy) / vec2(textureSize(galaxyTex, 0));
    texCoord.y *= 1.25;
    
    //Let's set a color for the lines and a synthwave terrain for the canvas!
    vec4 lineColor = vec4(1.0, 0.6, 0.0, 1.);
    vec4 color = vec4(0);
    color += terrain();

    //Positions
    vec2 leftCenterJoint = vec2(-.1, -.6);
    vec2 rightCenterJoint = vec2(.1, -.6);

    //Road borders
    color = mix(color, lineColor, finiteLine(center_uv, vec2(-1.8, -1), leftCenterJoint));
    color = mix(color, lineColor, finiteLine(center_uv, vec2(-1.5, -1), leftCenterJoint));
    color = mix(color, lineColor, finiteLine(center_uv, vec2(1.8, -1), rightCenterJoint));
    color = mix(color, lineColor, finiteLine(center_uv, vec2(1.5, -1), rightCenterJoint));

    ////Road end
    color = mix(color, lineColor, finiteLine(center_uv, rightCenterJoint, leftCenterJoint));

    ////Road
    color = mix(color, lineColor, finiteLine(center_uv, vec2(.0, -.6 - u_Time), vec2(.0, -.65 - u_Time)));
    color = mix(color, lineColor, finiteLine(center_uv, vec2(.0, -.75 - u_Time), vec2(.0, -.8 - u_Time)));
    color = mix(color, lineColor, finiteLine(center_uv, vec2(.0, -.9 - u_Time), vec2(.0, -.95 - u_Time)));

    //let's put it on the screen!
    FragColor = mix(color, texture(galaxyTex, texCoord), 1.-color.a);
};