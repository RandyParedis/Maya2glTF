#pragma once

#include "IndentableStream.h"

class Arguments
{
public:
	Arguments(const MArgList& args, const MSyntax& syntax);
	~Arguments();

	static MSyntax createSyntax();

	MString sceneName;
	MString outputFolder;
	MSelectionList selection;
	bool glb = false;
	
	/** Create a default material for primitives that don't have shading in Maya? */
	bool defaultMaterial = false;

	/** Assign a color with a different hue to each material, for debugging purposes */
	bool colorizeMaterials = false;

	/** Convert any material into the scene to a glTF PBR material */
	bool forcePbrMaterials = false;

	/** Always use 32-bit indices, even when 16-bit would be sufficient */
	bool force32bitIndices = false;

	/** If non-null, dump the Maya intermediate objects to the stream */
	IndentableStream* dumpMaya;
	
	/** If non-null, dump the GLTF JSON to the stream */
	IndentableStream* dumpGLTF;

	/** By default a single GLTF file is exported; pass -separate to move the binary buffers to a separate BIN file */
	bool separate = false;

	/** By default the Maya node name is not assigned to the GLTF node name */
	bool assignObjectNames = false;

	/** The scale factor to apply to the vertex positions */
	double scaleFactor = 1;

private:
	static std::unique_ptr<IndentableStream> getOutputStream(const MArgDatabase& adb, const char* arg, const char *outputName, std::ofstream& fileOutputStream);

	std::ofstream m_mayaOutputFileStream;
	std::ofstream m_gltfOutputFileStream;

	std::unique_ptr<IndentableStream> m_mayaOutputStream;
	std::unique_ptr<IndentableStream> m_gltfOutputStream;
};

