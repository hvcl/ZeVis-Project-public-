#version 460

uniform sampler2D em_texture;
uniform float em_opacity;

uniform sampler2D subregion_texture[10];
uniform int subregion_opacity[10];
uniform vec4 subregion_color[10];

uniform usampler2D cell_texture;
uniform float cell_opacity;
uniform sampler2D cell_color_texture;


uniform vec4 selected_color;
uniform vec4 unselected_color;


uniform int visualize_method;


in vec2 vTexCoord;
out vec4 fragColor;
float rand(vec2 co)
{
	return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void)
{
	int i;

	
	uint cell_value = texture(cell_texture, vTexCoord).r;
	vec4 celllayer = vec4(0.0, 0.0, 0.0, 0.0);
	float select_value = texture(cell_color_texture, vTexCoord).r;


	float alpha=0;
	vec4 result=vec4(0,0,0,0);

	if(int(cell_value) != 0){
		if(visualize_method == 0){
			vec3 color = vec3(rand(vec2(float(cell_value)/10, 1)), rand(vec2(float(cell_value)/10, 2)), rand(vec2(float(cell_value)/10, 3)));
			celllayer = vec4(color, cell_opacity);
		}
		else if(visualize_method == 1){
			if(select_value > 0.5){
				vec3 color = selected_color.rgb * select_value;
				celllayer = vec4(color, cell_opacity);
			}
			else{
				vec3 color = unselected_color.rgb * select_value;
				celllayer = vec4(color, cell_opacity);
			}	
		}
		result=result+celllayer*cell_opacity*(1-alpha);
		alpha=alpha+cell_opacity*(1-alpha);
	}

	for(i=9;i>=0;i--){
		int value = int(texture(subregion_texture[i], vTexCoord).r * 255);
		if(value > 0){
			if(subregion_opacity[i] > 0){
				float opacity = float(subregion_opacity[i]) / 100.0;
				vec4 subregionlayer = vec4(subregion_color[i].rgb, opacity);
				result=result+subregionlayer*opacity*(1-alpha);
				alpha=alpha+opacity*(1-alpha);
			}			
		}
	}

	float em_value = texture(em_texture, vTexCoord).r;
	vec4 emlayer = vec4(em_value, em_value, em_value, em_opacity);

	result=result+emlayer*em_opacity*(1-alpha);
	alpha=alpha+em_opacity*(1-alpha);


	fragColor = vec4(result.xyz,alpha);
};