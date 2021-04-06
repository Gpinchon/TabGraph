R""(
#extension GL_EXT_geometry_shader4 : enable

void LayeredCubemapRender(void) {
    int i, layer;
    for (layer = 0; layer < 6; layer++) {
        gl_Layer = layer;
        for (i = 0; i < gl_in.length(); i++) {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}
)""