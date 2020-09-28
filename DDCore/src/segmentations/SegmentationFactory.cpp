/*
 * SegmentationFactory.cpp
 *
 *  Created on: Dec 15, 2013
 *      Author: Christian Grefe, CERN
 */

#include "DDSegmentation/SegmentationFactory.h"

namespace dd4hep::DDSegmentation {

using std::map;
using std::vector;

/// Default constructor. Takes the class name as argument and takes care of registration with the factory
SegmentationCreatorBase::SegmentationCreatorBase(const std::string& name) {
	SegmentationFactory::instance()->registerSegmentation(name, this);
}

/// Initialize the global factory instance
SegmentationFactory* SegmentationFactory::_instance = nullptr;

/// Access to the global factory instance
auto SegmentationFactory::instance() -> SegmentationFactory* {
	if (not _instance) {
		_instance = new SegmentationFactory();
	}
	return _instance;
}

/// Create a new segmentation object with the given type name. Returns NULL if type name is unknown.
auto SegmentationFactory::create(const std::string& name, const std::string& identifier) const -> Segmentation* {
	map<std::string, SegmentationCreatorBase*>::const_iterator it;
	it = _segmentations.find(name);
	if (it != _segmentations.end()) {
		return it->second->create(identifier);
	}
	return nullptr;
}

/// Access to the list of registered segmentations
auto SegmentationFactory::registeredSegmentations() const -> vector<std::string> {
	vector<std::string> segmentationNames;
	map<std::string, SegmentationCreatorBase*>::const_iterator it;
	for (it = _segmentations.begin(); it != _segmentations.end(); ++ it) {
		segmentationNames.emplace_back(it->first);
	}
	return segmentationNames;
}

/// Registers a new SegmentationCreator with the factory
void SegmentationFactory::registerSegmentation(const std::string& name, SegmentationCreatorBase* creator) {
	_segmentations[name] = creator;
}

} /* namespace dd4hep */
