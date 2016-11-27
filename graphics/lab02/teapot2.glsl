#version 400
in vec3 normal_eye, position_eye;
in vec2 texture_coordinates; // texture from vertex shader
uniform sampler2D btex;
out vec4 frag_colour;


uniform mat4 V;

vec3 light_position_world  = vec3 (2.0, 1.0, 2.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

// surface reflectance
vec3 Ks = vec3 (1.0, 1.0, 1.0); // fully reflect specular light
vec3 Kd = vec3 (1.0, 0.5, 0.0); // orange diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0); // fully reflect ambient light
float specular_exponent = 100.0; // specular 'power'

vec3 lightcol = vec3  (0.2,1.2,0.0);


void main () {
    
    vec3 light = vec3(0,0,0);
    
    vec3 Ls = vec3(0.2,0.2,0.2);
    vec3 Ld = vec3(0.2,0.2,0.2);
    vec3 La = vec3(0.1,0.1,0.1);
    
    vec3 Ia = La * Ka;
    
    
    vec3 light_position_eye = vec3(vec4(light_position_world, 0.0));
    vec3 distance_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize(distance_to_light_eye);
    
    float dot_prod = dot(direction_to_light_eye, normalize(normal_eye));
    dot_prod = max(dot_prod, 0.0);
    vec3 Id = Ld * Kd * dot_prod;
    
    vec3 surface_to_viewer_eye = normalize(-position_eye);
    
    vec3 half_way_eye = normalize(surface_to_viewer_eye + direction_to_light_eye);
    float dot_prod_specular = max (dot (half_way_eye, normalize(normal_eye)), 1.0);
    float specular_factor = pow (dot_prod_specular, specular_exponent);
    
    
    vec3 Is = Ls * Ks * specular_factor;
    
    light = (Is + Id + Ia);
    vec3 abc = light*lightcol;
    vec2 tt = vec2(texture_coordinates.x , 1- texture_coordinates.y);
    vec4 texl = texture(btex, tt);
    frag_colour = texl *(abc, 1.0);
    /*
     vec2 tt = vec2(texture_coordinates.x , 1.0 - texture_coordinates.y);
     vec4 texl = texture(btex, texture_coordinates);
     frag_colour = texl;
     */
    
}


