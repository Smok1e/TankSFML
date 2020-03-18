uniform sampler2D current;
uniform vec2 resolution;
uniform float d;
uniform float alpha;

void main ()

{

	vec4 col = vec4 (1.0, 0.0, 0.0, 1.0);

	vec2 uv = vec2 (gl_FragCoord.xy.x, resolution.y - gl_FragCoord.y) / resolution;

	float r = 0.0;
	float g = 0.0;
	float b = 0.0;

	float n = 0.0;

	for (float i = -d; i <= d; i++)

	{

		for (float j = -d; j <= d; j++)

		{

			float x = i / gl_FragCoord.xy.x;
			float y = j / (resolution.y - gl_FragCoord.xy.y);

			vec2 xy = uv;

			xy.x += x;
			xy.y += y;

			if (xy.x >= 0.0 && xy.x <= 1.0 && xy.y >= 0.0 && xy.y <= 1.0)

			{

				vec4 texColor = texture2D(current,xy);

				r += texColor.r;
				g += texColor.g;
				b += texColor.b;

				n++;

			}

		}

	}

	col = vec4 (r / n, g / n, b / n, alpha);

	gl_FragColor = col;

}