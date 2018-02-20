#pragma once

#include "ExportableObject.h"	
#include "ExportableMesh.h"

class ExportableNode : public ExportableObject
{
public:
	ExportableNode(MDagPath dagPath, ExportableResources& resources);
	~ExportableNode();

	static std::unique_ptr<ExportableNode> from(MDagPath dagPath, ExportableResources& resources);

	GLTF::Node glNode;

	void sample();

private:
	std::unique_ptr<ExportableMesh> m_mesh;

	MObject m_obj;

	GLTF::Node::TransformMatrix m_matrix;

	DISALLOW_COPY_AND_ASSIGN(ExportableNode);
};

