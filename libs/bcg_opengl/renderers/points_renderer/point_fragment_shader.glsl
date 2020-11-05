#version 330

in vec4 f_color;
out vec4 final_color;

void main() {
    //final_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    final_color = f_color;
}
