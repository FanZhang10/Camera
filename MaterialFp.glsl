#version 400

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 colour_interp;
in vec3 light_pos;

// Attributes passed with the material file
uniform vec4 specular_colour;
uniform float ambient_amount;
uniform float phong_exponent;


void main() 
{
    // Blinn–Phong shading

    vec3 N, // Interpolated normal for fragment
	     L, // Light-source direction
		 V, // View direction
		 H; // Half-way vector

	// Compute Lambertian lighting Id
    N = normalize(normal_interp);

	L = (light_pos - position_interp );
	L = normalize(L);

	float Id = max(dot(N, L), 0.0);
	
	// Compute specular term for Blinn–Phong shading
	// Initially: V = (eye_position - position_interp);
	V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    H = 0.5*(V + L); // Halfway vector
    H = normalize(H);

    float spec_angle_cos = max(dot(N, H), 0.0);
	float Is = pow(spec_angle_cos, phong_exponent);
	    
	// Assign light to the fragment based on object's colour
	gl_FragColor = (ambient_amount + Id)*colour_interp + Is*specular_colour;
	    
	// For debug, we can display the different values
	//gl_FragColor = vec4(ambient_color, 1.0);
	//gl_FragColor = vec4(diffuse_color, 1.0);
	//gl_FragColor = vec4(specular_color, 1.0);
	//gl_FragColor = vec4(vec3(ambient_amount), 1.0);
	//gl_FragColor = vec4(vec3(lambertian_amount), 1.0);
	//gl_FragColor = vec4(vec3(specular_amount), 1.0);
	//gl_FragColor = colour_interp;
	//gl_FragColor = vec4(N.xyz, 1.0);
	//gl_FragColor = vec4(L.xyz, 1.0);
	//gl_FragColor = vec4(V.xyz, 1.0);
}