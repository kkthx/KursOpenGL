#version 330
in vec2 texture_coordinates;
in vec3 vertex_to_camera;
in vec3 normal_to_camera;

uniform mat4 view_matrix;
uniform sampler2D basic_texture;
uniform samplerCube env_map;

out vec4 frag_colour;

vec3 light_position = vec3(5.0, 3.0, 3.0);
vec3 ambient_color = vec3(0.2, 0.2, 0.2);
vec3 diffuse_color = vec3(0.8, 0.8, 0.8);
vec3 specular_color = vec3(1.0, 1.0, 1.0);

vec3 object_ambient_factor = vec3(1.0, 1.0, 1.0);
vec3 object_diffuse_factor = vec3(0.5, 0.5, 0.5);
vec3 object_specular_factor = vec3(1.0, 1.0, 1.0);
 
void main()
{
   // Ambient
   vec3 ambient_intense = ambient_color * object_ambient_factor;  

   // Diffuse
   vec3 distance_to_light = vec3(view_matrix * vec4(light_position, 1.0)) - vertex_to_camera;
   vec3 light_direction = normalize(distance_to_light);
   float dot_product = dot(light_direction, normal_to_camera);
   dot_product = max(dot_product, 0.0);
   vec3 diffuse_intense = diffuse_color * object_diffuse_factor * dot_product;  

   // Specular
   vec3 reflection = reflect(-light_direction, normal_to_camera);
   vec3 surface_to_camera = normalize(-vertex_to_camera);
   float dot_specular = dot(reflection, surface_to_camera);
   dot_specular = max(dot_specular, 0.0);
   float specular_power = 10.0;
   float specular_factor = pow(dot_specular, specular_power);
   vec3 specular_intense = specular_color * object_specular_factor * specular_factor; 

    vec4 texel = texture(basic_texture, texture_coordinates); 
    if (texel.a < 0.6)
        discard;  
        
    vec3 camera_vector = normalize(vertex_to_camera);
    vec3 normal_vector = normalize(normal_to_camera);
    
    vec3 reflected_vector = reflect(camera_vector, normal_vector);
    reflected_vector = vec3(inverse(view_matrix) * vec4(reflected_vector, 0.0));
    reflected_vector.yz = -reflected_vector.yz;
    vec4 reflected_colour = texture(env_map, reflected_vector);
        
    frag_colour = vec4(ambient_intense + diffuse_intense + specular_intense, 1.0) *  mix(reflected_colour, texel, 0.6);
}
