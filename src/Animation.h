#pragma once
#include <vector>

typedef GLTF::Animation::Channel::Target::Path AniPath;

class Animation {
public:
	enum Interpolation {
		LINEAR, STEP, CATMULLROMSPLINE, CUBICSPLINE
	};

	Animation(unsigned int size, const float& fps=24);
	~Animation();

	inline std::vector<GLTF::Animation*> get() const {
		return m_animations;
	};

	void setFrame(const unsigned int& inputframe);
	static bool isIdentity(float* data, const AniPath path);
	void writeResult(const AniPath path, float* data, GLTF::Node*);

private:
	std::vector<GLTF::Animation*> m_animations;

	unsigned int m_size;
	unsigned int m_cur_frame;
	float m_fps;

	GLTF::Animation::Channel* createChannel(const AniPath& type, GLTF::Node* node, float* data);
};