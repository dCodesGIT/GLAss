function getSphereVertices(radius, lats, longs) {
	// Variable declaration
	var vertices = Array();

	const rad = Math.PI / 180.0;
	const latitudes = 180.0 / lats;
	const longitudes = 360.0 / longs;

	for(var i = 0.0; i < 360.0; i += longitudes) {
		var iNext = i + longitudes;
		for(var j = 0.0; j < 180.0; j += latitudes) {
			var jNext = j + latitudes;

			var points = Array(4);
			var x, y, z;

			x = radius * Math.cos(i*rad) * Math.sin(j*rad);
			y = radius * Math.cos(j*rad);
			z = radius * Math.sin(i*rad) * Math.sin(j*rad);
			points[0] = Array(x, y, z);
			
			x = radius * Math.cos(iNext*rad) * Math.sin(j*rad);
			y = radius * Math.cos(j*rad);
			z = radius * Math.sin(iNext*rad) * Math.sin(j*rad);
			points[1] = Array(x, y, z);
			
			x = radius * Math.cos(iNext*rad) * Math.sin(jNext*rad);
			y = radius * Math.cos(jNext*rad);
			z = radius * Math.sin(iNext*rad) * Math.sin(jNext*rad);
			points[2] = Array(x, y, z);
			
			x = radius * Math.cos(i*rad) * Math.sin(jNext*rad);
			y = radius * Math.cos(jNext*rad);
			z = radius * Math.sin(i*rad) * Math.sin(jNext*rad);
			points[3] = Array(x, y, z);
			
			var tri1 = points[0].concat(points[1], points[2]);
			var tri2 = points[2].concat(points[3], points[0]);
			
			vertices = vertices.concat(tri1, tri2);
		}
	}
	return Float32Array.from(vertices);
}

function getSphereColors(arrLength, red, green, blue) {
	// Variable declaration
	var color = Array();

	// Code
	for(var i = 0; i < arrLength/3; i++)
		color = color.concat(red, green, blue);

	return Float32Array.from(color);
}

function getSphereNormals(lats, longs) {
	// Variable declaration
	var normals = Array();
	const rad = Math.PI / 180.0;
	const latitude = 180.0 / lats;
	const longitude = 360.0 / longs;

	// Code
	for(var i = 0.0; i < 360.0; i += longitude) {
		var iNext = i + longitude;
		for(var j = 0.0; j < 180.0; j += latitude) {
			var jNext = j + latitude;

			var points = Array(4);
			var x, y, z;

			x = Math.cos(i*rad) * Math.sin(j*rad);
			y = Math.cos(j*rad);
			z = Math.sin(i*rad) * Math.sin(j*rad);
			points[0] = Array(x, y, z);
			
			x = Math.cos(iNext*rad) * Math.sin(j*rad);
			y = Math.cos(j*rad);
			z = Math.sin(iNext*rad) * Math.sin(j*rad);
			points[1] = Array(x, y, z);
			
			x = Math.cos(iNext*rad) * Math.sin(jNext*rad);
			y = Math.cos(jNext*rad);
			z = Math.sin(iNext*rad) * Math.sin(jNext*rad);
			points[2] = Array(x, y, z);
			
			x = Math.cos(i*rad) * Math.sin(jNext*rad);
			y = Math.cos(jNext*rad);
			z = Math.sin(i*rad) * Math.sin(jNext*rad);
			points[3] = Array(x, y, z);
			
			var tri1 = points[0].concat(points[1], points[2]);
			var tri2 = points[2].concat(points[3], points[0]);
			
			var tri1_norm = Array(3);
			var tri2_norm = Array(3);
			for(var n = 0; n < 3; n++) {
//				tri1_norm[n] = (tri1[0+n] + tri1[3+n] + tri1[6+n] + tri2[0+n]) / 4;
				tri1_norm[n] = (tri1[0+n] + tri1[3+n] + tri1[6+n]) / 3;
				tri2_norm[n] = (tri2[0+n] + tri2[3+n] + tri2[6+n]) / 3;
			}
			
//			normals = normals.concat(tri1_norm, tri1_norm, tri1_norm, tri2_norm, tri2_norm, tri2_norm);
//			normals = normals.concat(tri1_norm, tri1_norm, tri1_norm, tri1_norm, tri1_norm, tri1_norm);

			normals = normals.concat(tri1, tri2);
		}
	}
	return Float32Array.from(normals);
}

function getSphereTexCoords(lats, longs) {
	// Variable declaration
	var texCoords = Array();
	const uDiff = 1.0 / lats;
	const vDiff = 1.0 / longs;

	// Code
	for(var u = 0.0; u < 1.0; u += uDiff) {
		var uNext = u + uDiff;
		for(var v = 0.0; v < 1.0; v += vDiff) {
			var vNext = v + vDiff;

			var points = Array(4);
			points[0] = Array(u, v);
			points[1] = Array(uNext, v);
			points[2] = Array(uNext, vNext);
			points[3] = Array(u, vNext);

			var tri1 = points[0].concat(points[1], points[2]);
			var tri2 = points[2].concat(points[3], points[0]);

			if(u === 0.0 && v === 0.0)
				texCoords = tri1.concat(tri2);
			else
				texCoords = texCoords.concat(tri1, tri2);
		}
	}
	return Float32Array.from(texCoords);
}
