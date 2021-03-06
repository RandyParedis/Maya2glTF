#include "externals.h"
#include "MeshBlendShapes.h"
#include "MayaException.h"
#include "dump.h"
#include "DagHelper.h"
#include "MayaUtils.h"
#include "MeshBlendShapeWeights.h"
#include "IndentableStream.h"

MeshBlendShapes::MeshBlendShapes(MObject blendShapeNode, const Arguments& args)
{
	MStatus status;

	MFnBlendShapeDeformer fnController(blendShapeNode, &status);
	THROW_ON_FAILURE(status);

	cout << prefix << "Processing blend shapes of " << fnController.name().asChar() << "..." << endl;

	MPlug weightArrayPlug = fnController.findPlug("weight", &status);
	THROW_ON_FAILURE(status);

	MPlug outputGeometryPlugs = fnController.findPlug("outputGeometry", &status);
	THROW_ON_FAILURE(status);

	const auto outputGeometryPlugsDimension = outputGeometryPlugs.evaluateNumElements(&status);
	THROW_ON_FAILURE(status);

	if (outputGeometryPlugsDimension == 0)
	{
		MayaException::printError("Output geometry is not connected to anything!");
		return;
	}

	MPlug outputGeometryPlug = outputGeometryPlugs.elementByPhysicalIndex(0, &status);
	THROW_ON_FAILURE(status);

	MObject outputShape = getOrCreateOutputShape(outputGeometryPlug, m_tempOutputMesh);

	if (outputShape.isNull())
	{
		MayaException::printError("Could not get output geometry!");
		return;
	}

	MFnMesh outputMesh(outputShape, &status);
	THROW_ON_FAILURE(status);

	// We use the MeshBlendShapeWeights helper class to manipulate the weights 
	// in order to reconstruct the geometry of deleted blend shape targets.
	// When an exception is thrown, the weights and connections will be restored.
	MeshBlendShapeWeights weightPlugs(weightArrayPlug);
	weightPlugs.breakConnections();

	// Clear all weights to reconstruct base mesh
	weightPlugs.clearWeightsExceptFor(-1);

	m_baseShape = std::make_unique<MeshShape>(outputMesh, args, true);

	const auto numWeights = weightPlugs.numWeights();

	for (auto i = 0; i < numWeights; i++)
	{
		weightPlugs.clearWeightsExceptFor(i);
		auto weightPlug = weightPlugs.getWeightPlug(i);
		m_entries.emplace_back(std::make_unique<MeshBlendShapeEntry>(outputMesh, args, weightPlug));
	}
}

MeshBlendShapes::~MeshBlendShapes()
{
	if (!m_tempOutputMesh.isNull())
	{
		MDagModifier dagMod;
		dagMod.deleteNode(m_tempOutputMesh);
		m_tempOutputMesh = MObject::kNullObj;
	}
}

void MeshBlendShapes::dump(class IndentableStream& out, const std::string& name) const
{
	out << quoted(name) << ": {" << endl << indent;

	if (m_baseShape)
	{
		m_baseShape->dump(out, "base");
	}
	else
	{
		out << "base: null";
	}

	out << "," << endl;

	for (auto i = 0; i < m_entries.size(); ++i)
	{
		m_entries.at(i)->shape.dump(out, std::string("target#") + std::to_string(i));
		out << "," << endl;
	}

	out << undent << '}';
}

MObject MeshBlendShapes::getOrCreateOutputShape(MPlug& outputGeometryPlug, MObject& createdMesh) const
{
	MStatus status;
	MPlugArray connections;
	outputGeometryPlug.connectedTo(connections, false, true, &status);
	THROW_ON_FAILURE(status);

	MObject meshNode = MObject::kNullObj;

	for (auto i = 0U; i < connections.length() && meshNode.isNull(); ++i)
	{
		MPlug targetPlug = connections[i];
		MObject targetNode = targetPlug.node();

		if (!targetNode.isNull())
		{
			if (targetNode.hasFn(MFn::kMesh))
			{
				meshNode = targetNode;
			}
		}
	}

	if (meshNode.isNull())
	{
		// Not connected to a mesh, create one.

		// Create the mesh node
		MDagModifier dagMod;
		createdMesh = dagMod.createNode("mesh", MObject::kNullObj, &status);
		THROW_ON_FAILURE(dagMod.doIt());

		// Make sure we select the shape node, not the transform node.
		MDagPath meshDagPath = MDagPath::getAPathTo(createdMesh, &status);
		THROW_ON_FAILURE(status);
		THROW_ON_FAILURE(meshDagPath.extendToShape());

		// NOTE: The node does not yet have an MFnMesh interface!!!
		// This only happens when it is connected to another node that delivers geometry...
		MFnDagNode dagFn(meshDagPath, &status);
		THROW_ON_FAILURE(status);

		const MString newSuggestedName = "maya2glTW_" + utils::simpleName(outputGeometryPlug.name(&status));
		THROW_ON_FAILURE(status);

		const MString newName = dagFn.setName(newSuggestedName, &status);
		THROW_ON_FAILURE(status);

		cout << prefix << "Created temporary output mesh '" << newName  << "'. This will be deleted after exporting, but Maya will think your scene is modified, and warn you." << endl;

		// Make the mesh invisible
		MPlug intermediateObjectPlug = dagFn.findPlug("intermediateObject", &status);
		THROW_ON_FAILURE(status);
		THROW_ON_FAILURE(intermediateObjectPlug.setBool(true));

		MDGModifier dgMod;

		MPlug inMeshPlug = dagFn.findPlug("inMesh", &status);
		THROW_ON_FAILURE(status);

		THROW_ON_FAILURE(dgMod.connect(outputGeometryPlug, inMeshPlug));
		THROW_ON_FAILURE(dgMod.doIt());

		// Should now be a mesh...
		if (meshDagPath.hasFn(MFn::kMesh))
		{
			meshNode = meshDagPath.node(&status);
			THROW_ON_FAILURE(status);
		}
	}

	return meshNode;
}
