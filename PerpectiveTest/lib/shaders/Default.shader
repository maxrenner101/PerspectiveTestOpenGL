#shader vertex
#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

in vec3 position;
out vec3 fcolor;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);
	fcolor = vec3(0.0f,0.7f,0.7f);
}

#shader fragment
#version 330 core

in vec3 fcolor;
out vec4 color_out;

void main() {
	color_out = vec4(fcolor, 1.0f);
}