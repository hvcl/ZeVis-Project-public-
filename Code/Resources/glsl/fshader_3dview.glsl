#version 140
#extension GL_ARB_compatibility : enable

uniform vec3 eye_position;
uniform sampler3D tex;
uniform sampler3D heatmap_tex;

in vec3 pixel_position;
uniform vec3 box_min;
uniform vec3 box_max;
uniform vec3 up;
uniform float sample;
uniform sampler1D color_table;
uniform vec4 background_color;
uniform float Udiffu;
uniform float Uambi;
uniform float Uspecu;
uniform float Ushin;
uniform float l_t;
uniform vec3 v_size;
uniform vec4 heatmap_color;
uniform vec3 plane_normal;
uniform vec3 plane_point;

uniform vec2 window_size;

uniform float scale;

vec3 cross_product(vec3 v1, vec3 v2) {
	vec3 v;
	v.x = v1.y*v2.z - v2.y*v1.z;
	v.y = v1.z*v2.x - v2.z*v1.x;
	v.z = v1.x*v2.y - v2.x*v1.y;
	return v;
}

void main()
{
	float grid_rate = 0.02;
	float pad = 0.02;
	float edge_pad = 0.02;


	int grid_type = 0;
	if (plane_normal.y>plane_normal.x && plane_normal.y>plane_normal.z)grid_type = 1;
	else if (plane_normal.z>plane_normal.x && plane_normal.z>plane_normal.y)grid_type = 2;

	vec3 right = normalize(cross_product(-eye_position, up));
	vec3 t_pixel_position = eye_position * 0.5 + right * pixel_position.x *window_size.x + up * pixel_position.y * window_size.y;

	//    vec3 direction=normalize(t_pixel_position-eye_position);
	vec3 direction = normalize(-eye_position);

	int cn = 0;
	vec3 a[2];
	float k;
	vec3 point;
	vec3 render_box_min = box_min - vec3(pad, pad, pad);
	vec3 render_box_max = box_max + vec3(pad, pad, pad);
	if (cn<2) {
		k = (render_box_min.x - t_pixel_position.x) / direction.x;
		point = t_pixel_position + k * direction;
		if (point.y >= render_box_min.y && point.y <= render_box_max.y
			&& point.z >= render_box_min.z && point.z <= render_box_max.z) {
			a[cn++] = point;
		}
	}
	if (cn<2) {
		k = (render_box_max.x - t_pixel_position.x) / direction.x;
		point = t_pixel_position + k * direction;
		if (point.y >= render_box_min.y && point.y <= render_box_max.y
			&& point.z >= render_box_min.z && point.z <= render_box_max.z) {
			a[cn++] = point;
		}
	}
	if (cn<2) {
		k = (render_box_min.y - t_pixel_position.y) / direction.y;
		point = t_pixel_position + k * direction;
		if (point.x >= render_box_min.x && point.x <= render_box_max.x
			&& point.z >= render_box_min.z && point.z <= render_box_max.z) {
			a[cn++] = point;
		}
	}
	if (cn<2) {
		k = (render_box_max.y - t_pixel_position.y) / direction.y;
		point = t_pixel_position + k * direction;
		if (point.x >= render_box_min.x && point.x <= render_box_max.x
			&& point.z >= render_box_min.z && point.z <= render_box_max.z) {
			a[cn++] = point;
		}
	}
	if (cn<2) {
		k = (render_box_min.z - t_pixel_position.z) / direction.z;
		point = t_pixel_position + k * direction;
		if (point.x >= render_box_min.x && point.x <= render_box_max.x
			&& point.y >= render_box_min.y && point.y <= render_box_max.y) {
			a[cn++] = point;
		}
	}
	if (cn<2) {
		k = (render_box_max.z - t_pixel_position.z) / direction.z;
		point = t_pixel_position + k * direction;
		if (point.x >= render_box_min.x && point.x <= render_box_max.x
			&& point.y >= render_box_min.y && point.y <= render_box_max.y) {
			a[cn++] = point;
		}
	}
	float alpha = 0;
	vec4 cur_color = vec4(0, 0, 0, 0);

	if (cn == 2) {
		if (length(a[0] - eye_position)>length(a[1] - eye_position)) {
			vec3 t = a[0];
			a[0] = a[1];
			a[1] = t;
		}

		int sampling_num = int(length(a[1] - a[0])*sample);
		vec3 normalize_box = 1.0 / (box_max - box_min);
		vec3 dir = (a[1] - a[0])*normalize_box / sampling_num;
		vec3 cur_location = (a[0] - box_min)*normalize_box;
		vec3 d_l = eye_position + up * 2 + right * 2;
		vec3 edge_min = (box_min + vec3(-edge_pad, -edge_pad, -edge_pad) - box_min)*normalize_box;
		vec3 edge_max = (box_max + vec3(edge_pad, edge_pad, edge_pad) - box_min)*normalize_box;
		for (int i = 0; i<sampling_num; i++) {
			cur_location = cur_location + dir;

			if (cur_location.x>edge_min.x && cur_location.x<edge_max.x
				&& cur_location.y>edge_min.y && cur_location.y<edge_max.y
				&& cur_location.z>edge_min.z && cur_location.z<edge_max.z) {

				int edge_count = 0;
				if (cur_location.x - edge_min.x<0.003 || edge_max.x - cur_location.x<0.003)edge_count++;
				if (cur_location.y - edge_min.y<0.003 || edge_max.y - cur_location.y<0.003)edge_count++;
				if (cur_location.z - edge_min.z<0.003 || edge_max.z - cur_location.z<0.003)edge_count++;
				if (edge_count >= 2) {
					cur_color = cur_color + vec4(1 - background_color.x, 1 - background_color.y, 1 - background_color.z, 1)*(1 - alpha);
					alpha = 1;
					break;
				}
			}

			float planeToCurDistance = abs(plane_normal.x*(cur_location.x - plane_point.x)
				+ plane_normal.y*(cur_location.y - plane_point.y)
				+ plane_normal.z*(cur_location.z - plane_point.z))
				/ length(plane_normal);
			if (planeToCurDistance<0.003) {
				vec4 grid_color = vec4(0, 0, 0, 0);
				if (grid_type == 0) {
					if (abs(int(cur_location.y*grid_rate*v_size.y) - cur_location.y*grid_rate*v_size.y)<0.01
						|| abs(int(cur_location.z*grid_rate*v_size.z) - cur_location.z*grid_rate*v_size.z)<0.01) {
						grid_color = vec4(1, 1, 1, 0.3);
					}
				}
				else if (grid_type == 1) {
					if (abs(int(cur_location.x*grid_rate*v_size.x) - cur_location.x*grid_rate*v_size.x)<0.01
						|| abs(int(cur_location.z*grid_rate*v_size.z) - cur_location.z*grid_rate*v_size.z)<0.01) {
						grid_color = vec4(1, 1, 1, 0.3);
					}
				}
				else {
					if (abs(int(cur_location.x*grid_rate*v_size.x) - cur_location.x*grid_rate*v_size.x)<0.01
						|| abs(int(cur_location.y*grid_rate*v_size.y) - cur_location.y*grid_rate*v_size.y)<0.01) {
						grid_color = vec4(1, 1, 1, 0.3);
					}
				}

				//grid_color=vec4(0,1,1,0.02);
				cur_color = cur_color + grid_color * grid_color.w*(1 - alpha);
				alpha = alpha + (1 - alpha)*grid_color.w;
			}

			if (cur_location.x>0 && cur_location.x<1
				&& cur_location.y>0 && cur_location.y<1
				&& cur_location.z>0 && cur_location.z<1) {

				float value = texture3D(tex, cur_location).x;
				float heatmap_value = texture3D(heatmap_tex, cur_location).x;

				vec4 heatmap_rainbow_color=vec4(0,0,0,1);

				if(heatmap_value<0.3333){
					heatmap_rainbow_color=vec4(0,heatmap_value*3,1,1);
				}
				else if(heatmap_value<0.6666){
					heatmap_rainbow_color=vec4((heatmap_value-0.3333)*3,1,(0.6666-heatmap_value)*3,1);
				}
				else{
					heatmap_rainbow_color=vec4(1,(1-heatmap_value)*3,0,1);
				}

				vec4 t_color = texture1D(color_table, value);
				float t_alpha = t_color.w;

				if (alpha<l_t && t_color.w>0.01) {
					float right_color = texture3D(tex, cur_location + vec3(1.0 / v_size.x, 0, 0)).x;
					float left_color = texture3D(tex, cur_location - vec3(1.0 / v_size.x, 0, 0)).x;
					float up_color = texture3D(tex, cur_location + vec3(0, 1.0 / v_size.y, 0)).x;
					float down_color = texture3D(tex, cur_location - vec3(0, 1.0 / v_size.y, 0)).x;
					float front_color = texture3D(tex, cur_location - vec3(0, 0, 1.0 / v_size.z)).x;
					float back_color = texture3D(tex, cur_location + vec3(0, 0, 1.0 / v_size.z)).x;
					vec3 gr = -vec3(right_color - left_color, up_color - down_color, front_color - back_color);

					vec3 L = normalize(d_l - (cur_location / normalize_box + box_min));
					vec3 E = normalize(eye_position - (cur_location / normalize_box + box_min));
					vec3 R = normalize(-reflect(L, normalize(gr)));
					vec4 diffu = vec4(Udiffu, Udiffu, Udiffu, Udiffu)*max(dot(normalize(gr), L), 0.0) * 2;
					diffu = clamp(diffu, 0.0, 1.0);
					vec4 specu = vec4(Uspecu, Uspecu, Uspecu, Uspecu)*pow(max(dot(R, E), 0.0), Ushin*0.1);
					specu = clamp(specu, 0.0, 1.0);
					t_color = t_color + Uambi + diffu + specu;
				}

//				cur_color = cur_color + heatmap_color * heatmap_value*0.5*(1 - alpha) + t_color * t_alpha*0.5*(1 - alpha);
//				alpha = alpha + (1 - alpha)*(heatmap_value*0.5 + t_alpha * 0.5);


				cur_color = cur_color + heatmap_rainbow_color * pow(heatmap_value,2)*0.5 *scale*0.05*(1 - alpha) + t_color * t_alpha*0.5*(1 - alpha);
				alpha = alpha + (1 - alpha)*(pow(heatmap_value,2)*0.5 *scale*0.05 + t_alpha * 0.5);
				if (alpha>0.9)break;
			}
		}
	}
	cur_color = cur_color + background_color * (1 - alpha);
	cur_color.w = 1;
	gl_FragColor = cur_color;
};
