/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

namespace ATLAS {

namespace Model {

	struct Color {
		float r;
		float g;
		float b;
	};

	struct Material {
		Color ambient;
		Color diffuse;
		Color emissive;
		Color specular;
		float opacity;
		float shininess;
		//TODO: Textures
		Material() :
		    opacity(1.0),
		    shininess(0.0)
		{
			// deliberately empty
		}
	};

}

}
