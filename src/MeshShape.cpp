#include "externals.h"
#include "MeshShape.h"
#include "IndentableStream.h"

MeshShape::MeshShape(const MFnMesh& fnMesh, const Arguments& args, const bool isBlendShape): isBlendShape(isBlendShape)
{
	m_semantics.reset(new MeshSemantics(fnMesh, isBlendShape));
	m_vertices.reset(new MeshVertices(*m_semantics, fnMesh, args));
	m_indices.reset(new MeshIndices(*m_semantics, fnMesh));
}

MeshShape::~MeshShape()
{
}

void MeshShape::dump(IndentableStream& out, const std::string& name) const
{
	out << quoted(name) << ": {" << endl << indent;

	out << "isBlendShape: " << isBlendShape << ", " << endl;
	m_semantics->dump(out, "semantics");
	out << "," << endl;
	m_vertices->dump(out, "vertices");
	out << "," << endl;
	m_indices->dump(out, "indices");
	out << "," << endl;

	out << undent << '}' << endl;
}

