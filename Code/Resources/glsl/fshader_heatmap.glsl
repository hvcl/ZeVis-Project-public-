#version 460
uniform sampler2D thumbnail;
uniform sampler2D heatmap;
uniform vec4 heatmap_color;
uniform int heatmap_max;
in vec2 vTexCoord;
out vec4 fragColor;

void main(void)
{
	
	vec4 background = texture(thumbnail, vTexCoord);
	background.a = 1.0f;

	float heatmap_value = texture(heatmap, vTexCoord).r / heatmap_max;
	vec4 heatmap_layer = vec4(heatmap_color.rgb, heatmap_value);

	vec4 result = background * (1- heatmap_layer.a) + heatmap_layer * (heatmap_layer.a);
	result.a = 1.0f;
	fragColor = result;
};