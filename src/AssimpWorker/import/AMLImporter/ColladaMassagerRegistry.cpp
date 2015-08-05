/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "ColladaMassagerRegistry.hpp"

namespace AssimpWorker {

	ColladaMassagerRegistry::ColladaMassagerRegistry() :
		massagers(),
		rootScale(1.0),
		rootScaleSet(false)
	{
		return;
	}

	ColladaMassagerRegistry::~ColladaMassagerRegistry(){
		for (auto m : massagers){
			delete m.second;
		}
	}

	void ColladaMassagerRegistry::setRootScale(float newScale) {
		rootScale = newScale;
		rootScaleSet = true;
	}

	float ColladaMassagerRegistry::getRootScale() {
		return rootScale;
	}

	bool ColladaMassagerRegistry::isRootScaleSet() {
		return rootScaleSet;
	}

	ColladaMassager*  ColladaMassagerRegistry::getMassager(const Poco::URI& colladaFileURI){
		std::map<std::string, ColladaMassager*>::iterator iter = massagers.find(colladaFileURI.getPath());
		if (iter != massagers.end()){
			return iter->second;
			
		}
		ColladaMassager* massager = new ColladaMassager(colladaFileURI);
		massagers.insert(std::make_pair(colladaFileURI.getPath(), massager));
		return massager;
	}
} // End namespace AssimpWorker


