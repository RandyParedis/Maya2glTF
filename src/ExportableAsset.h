#pragma once
#include "ExportableResources.h"
class Arguments;

class ExportableAsset
{
public:
	ExportableAsset(const Arguments& args);
	~ExportableAsset();

	const std::string& prettyJsonString() const;

	void save();
	void sampleAnimations();

private:
	GLTF::Asset m_glAsset;
	GLTF::Scene m_glScene;
	GLTF::Asset::Metadata m_glMetadata;

	ExportableResources m_resources;

	std::vector<std::unique_ptr<ExportableItem>> m_items;

	std::string m_rawJsonString;
	mutable std::string m_prettyJsonString;

	void addNode(MDagPath& dagPath);

	static void create(std::ofstream& file, const std::string& path, const std::ios_base::openmode mode);
};

