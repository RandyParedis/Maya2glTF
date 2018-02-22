#include "externals.h"
#include "ExportableNode.h"
#include "MayaException.h"
#include "ExportableResources.h"
#include "Arguments.h"

ExportableNode::ExportableNode(MDagPath dagPath, ExportableResources& resources)
	: ExportableObject(dagPath.node()), m_obj(dagPath.node())
{
	MStatus status;

	handleNameAssignment(resources, glNode);

	glNode.transform = &m_matrix;

	MFnDagNode fnDagNode(dagPath, &status);
	THROW_ON_FAILURE(status);

	auto transform = fnDagNode.transformationMatrix(&status);
	THROW_ON_FAILURE(status);

	float m[4][4];
	transform.get(m);

	m_matrix = GLTF::Node::TransformMatrix(
		m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]);

	dagPath.extendToShape();

	switch (dagPath.apiType(&status))
	{
	case MFn::kMesh:
	{
		m_mesh = std::make_unique<ExportableMesh>(dagPath, resources);
		glNode.mesh = &m_mesh->glMesh;
	}
	break;
	default:
		cerr << "glTF2Maya: skipping '" << name() << "', it is not supported" << endl;
		break;
	}
}

ExportableNode::~ExportableNode()
{
}

void ExportableNode::sample(Animation* animation) {
	MFnDependencyNode fnDependNode{ m_obj };

	/// TRANSFORMATIONS
	double tx, ty, tz, rx, ry, rz, sx, sy, sz;
	{
		// Translation
		MPlug txPlug{ m_obj, fnDependNode.attribute("translateX") };
		tx = txPlug.asDouble();

		MPlug tyPlug{ m_obj, fnDependNode.attribute("translateY") };
		ty = tyPlug.asDouble();

		MPlug tzPlug{ m_obj, fnDependNode.attribute("translateZ") };
		tz = tzPlug.asDouble();

		// Rotation
		MPlug rxPlug{ m_obj, fnDependNode.attribute("rotateX") };
		rx = rxPlug.asDouble();

		MPlug ryPlug{ m_obj, fnDependNode.attribute("rotateY") };
		ry = ryPlug.asDouble();

		MPlug rzPlug{ m_obj, fnDependNode.attribute("rotateZ") };
		rz = rzPlug.asDouble();

		// Scaling
		MPlug sxPlug{ m_obj, fnDependNode.attribute("scaleX") };
		sx = sxPlug.asDouble();

		MPlug syPlug{ m_obj, fnDependNode.attribute("scaleY") };
		sy = syPlug.asDouble();

		MPlug szPlug{ m_obj, fnDependNode.attribute("scaleZ") };
		sz = szPlug.asDouble();

		/*std::cout << "Translation [X, Y, Z]: " << tx << ", " << ty << ", " << tz << std::endl;
		std::cout << "Rotation    [X, Y, Z]: " << radtodeg(rx) << "°, " << radtodeg(ry) << "°, " << radtodeg(rz) << "°\n";
		std::cout << "Scaling     [X, Y, Z]: " << sx << ", " << sy << ", " << sz << std::endl;*/
	}

	/// Matrix
	GLTF::Node::TransformTRS trs_matrix{};

	// Translate
	trs_matrix.translation[0] = (float)tx;
	trs_matrix.translation[1] = (float)ty;
	trs_matrix.translation[2] = (float)tz;

	// Rotate
	// ???

	// Scale
	trs_matrix.scale[0] = (float)sx;
	trs_matrix.scale[1] = (float)sy;
	trs_matrix.scale[2] = (float)sz;
}

std::unique_ptr<ExportableNode> ExportableNode::from(MDagPath dagPath, ExportableResources& usedShaderNames)
{
	MStatus status;

	MString name = dagPath.partialPathName(&status);
	THROW_ON_FAILURE(status);

	MObject mayaNode = dagPath.node(&status);
	if (mayaNode.isNull() || status.error())
	{
		cerr << "glTF2Maya: skipping '" << name.asChar() << "' as it is not a node" << endl;
		return nullptr;
	}

	return std::make_unique<ExportableNode>(dagPath, usedShaderNames);
}
