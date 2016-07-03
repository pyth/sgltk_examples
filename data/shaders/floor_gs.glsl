#version 150
layout(triangles) in;

// Three lines will be generated: 6 vertices
layout(line_strip, max_vertices=6) out;

uniform float normal_length;
uniform mat4 gxl3d_ModelViewProjectionMatrix;

in Vertex
{
	vec4 normal;
	vec4 color;
} vertex[];

out vec4 vertex_color;

void main()
{
	int i;
	for(i=0; i&lt;gl_in.length(); i++)
	{
		vec3 P = gl_in[i].gl_Position.xyz;
		vec3 N = vertex[i].normal.xyz;

		gl_Position = gxl3d_ModelViewProjectionMatrix * vec4(P, 1.0);
		vertex_color = vertex[i].color;
		EmitVertex();

		gl_Position = gxl3d_ModelViewProjectionMatrix * vec4(P + N * normal_length, 1.0);
		vertex_color = vertex[i].color;
		EmitVertex();

		EndPrimitive();
	}
}
