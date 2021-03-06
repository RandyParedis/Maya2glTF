#pragma once
#include "MeshSemantics.h"

class Arguments;

class Exporter : public MPxCommand
{
public:
	Exporter();
	~Exporter();

	static void* createInstance();

	MStatus doIt(const MArgList& args) override;

	bool isUndoable() const override;

	bool hasSyntax() const override;

	void exportScene(const Arguments& args);

private:
	class ExportableMesh* exportMesh(const MDagPath& dagPath);

};
