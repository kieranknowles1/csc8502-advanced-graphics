#pragma once
#include <string>
#include <map>
#include <vector>

using std::map;
using std::string;
using std::vector;

class MeshMaterialEntry {
public:
	std::map<string, string> entries;

	bool GetEntry(const string& name, string& output) const {
		auto i = entries.find(name);
		if (i == entries.end()) {
			return false;
		}
		output = i->second;
		//*output = &i->second;
		//*output = &i->second;
		return true;
	}

};

class MeshMaterial
{
public:
	MeshMaterial(const std::string& filename);
	~MeshMaterial() {}
	int getLayerCount() const { return materialLayers.size(); }
	const MeshMaterialEntry* GetMaterialForLayer(int i) const;

protected:
	std::vector<MeshMaterialEntry>	materialLayers;
	std::vector<MeshMaterialEntry*> meshLayers;
};

