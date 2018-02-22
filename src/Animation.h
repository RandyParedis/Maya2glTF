#pragma once
#include <vector>

typedef GLTF::Animation::Channel::Target::Path AniPath;

class Animation {
public:
	enum Interpolation {
		LINEAR, STEP, CATMULLROMSPLINE, CUBICSPLINE
	};

	Animation(int nodeid, unsigned int size);
	~Animation();

	inline std::vector<GLTF::Animation*> get() const {
		return m_animations;
	};

	void setFrame(const unsigned int& inputframe);
	void writeResult(const AniPath path, float* data);

private:
	std::vector<GLTF::Animation*> m_animations;

	unsigned int m_size;
	unsigned int m_cur_frame;
};