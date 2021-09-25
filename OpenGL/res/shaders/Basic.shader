#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform float u_Color;

void main()
{
    gl_Position = position;
};


#shader fragment
#version 450 core

layout(location = 0) out vec4 FragColor;

uniform float u_Time;
uniform float u_LongerTime;
uniform vec2 u_Resolution;
uniform float u_AspectRatio;

vec3 terrain() {
    //heavily modified code from https://www.shadertoy.com/view/4tsGD7
    //Did a lot of optimization and changed it to my "synthwave" taste :)
    vec3 currentFragment = vec3(gl_FragCoord.xy/u_Resolution - .5, u_AspectRatio - .5);
    currentFragment.y += 0.3;
    vec3 camera = vec3(-1.57, .1, u_LongerTime); //camera position and movement

    for (float i = .0; i < 9.; i += .01) { //Render distance
        camera += currentFragment * i * .3; //camera = floor(camera) gives voxel (minecraft) terrain
        if (cos(camera.z) + sin(camera.x) > camera.y) { //shapes the terrain into small mountains
            return vec3(.1, .02, 0) / i; //returning fragment's color
        }
    }
}

float finiteLine(vec2 P, vec2 A, vec2 B){
    vec2 g = B - A;
    vec2 h = P - A;
    return smoothstep(.005, .0, length(h - g * clamp(dot(g, h) / dot(g, g), .0, 1.)));
}

float LeftInfiniteLine(vec2 st, float altitude, float angle) {
    return step(st.x, .4) * smoothstep(.002, .0, abs((.0 - st.y + altitude + st.x / (2 * angle)))); //step()* = if true, apply smoothstep()
}

float RightInfiniteLine(vec2 st, float altitude, float angle) {
    return step(.6, st.x) * smoothstep(.002, .0, abs(st.y - .5 + altitude + st.x / (2 * angle)));
}

void main() {
    //Normalizing coords
    //let's put the uv coordinates into 0-1 range!
    vec2 uv = gl_FragCoord.xy / u_Resolution;

    //Let's find the center coords!
    vec2 center_uv = (gl_FragCoord.xy * 2.0 - u_Resolution.xy) / u_Resolution.y;
    
    //Let's make the canvas black
    vec3 color = vec3(0.0);

    //... and the road line color to orangered!
    vec3 lineColor = vec3(1.0, 0.6, 0.0);

    //Let's add some synthwave terrain :)
    color += terrain();

    //Road borders
    float pct = RightInfiniteLine(uv, 0.0, 1.0);
    color = (1.0 - pct) * color + pct * lineColor;

    pct = RightInfiniteLine(uv, 0.15, 2.0);
    color = (1.0 - pct) * color + pct * lineColor;

    pct = LeftInfiniteLine(uv, 0.0, 1.0);
    color = (1.0 - pct) * color + pct * lineColor;
    
    pct = LeftInfiniteLine(uv, 0.1, 2.0);
    color = (1.0 - pct) * color + pct * lineColor;

    ////Road end
    pct = finiteLine(center_uv, vec2(.36, -.6), vec2(-.36, -.6));
    color = mix(color, lineColor, pct);

    ////Road
    pct = finiteLine(center_uv, vec2(.0, -.6 - u_Time), vec2(.0, -.65 - u_Time));
    color = mix(color, lineColor, pct);

    pct = finiteLine(center_uv, vec2(.0, -.75 - u_Time), vec2(.0, -.8 - u_Time));
    color = mix(color, lineColor, pct);

    pct = finiteLine(center_uv, vec2(.0, -.9 - u_Time), vec2(.0, -.95 - u_Time));
    color = mix(color, lineColor, pct);

    //put it on the screen!
    FragColor = vec4(color, 1.0);
};