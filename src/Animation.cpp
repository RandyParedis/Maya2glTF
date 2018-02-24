#include "externals.h"
#include "Animation.h"
#include <limits>

Animation::Animation(unsigned int size, const float& fps) {
	this->m_size = size;
	this->m_fps = fps;
}

Animation::~Animation() {}

void Animation::setFrame(const unsigned int& inputframe) {
	this->m_cur_frame = inputframe;
}

bool Animation::isIdentity(float* data, const AniPath path) {
	bool iden = false;
	switch (path) {
	case AniPath::ROTATION:
		iden = (data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 1);
		break;

	case AniPath::TRANSLATION:
		iden = (data[0] == 0 && data[1] == 0 && data[2] == 0);
		break;

	case AniPath::SCALE:
		iden = (data[0] == 1 && data[1] == 1 && data[2] == 1);
		break;
	}
	return iden;
}

GLTF::Animation::Channel* Animation::createChannel(const AniPath& type, GLTF::Node* node, float* data) {
	std::vector<float> inpData;
	for (int i = 0; i < m_size; ++i) {
		inpData.push_back(i*1.0f / m_fps);
	}
	std::vector<float> outData(m_size, 0);
	GLTF::Accessor* inputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR,
		GLTF::Constants::WebGL::FLOAT, (unsigned char*)inpData.data(), m_size, (GLTF::Constants::WebGL) - 1);
	GLTF::Accessor* outputAccessor;
	switch (type) {
	case AniPath::TRANSLATION:
	case AniPath::SCALE:
		outputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::VEC3,
			GLTF::Constants::WebGL::FLOAT, (unsigned char*)outData.data(), m_size, (GLTF::Constants::WebGL) - 1);
		break;

	default: //case AniPath::ROTATION:
		outputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::VEC4,
			GLTF::Constants::WebGL::FLOAT, (unsigned char*)outData.data(), m_size, (GLTF::Constants::WebGL) - 1);
		break;
	}
	outputAccessor->max = data;
	outputAccessor->min = data;
	outputAccessor->writeComponentAtIndex(m_cur_frame, data);

	GLTF::Animation::Channel* channel = new GLTF::Animation::Channel();
	GLTF::Animation::Channel::Target* target = new GLTF::Animation::Channel::Target();
	GLTF::Animation::Sampler* sampler = new GLTF::Animation::Sampler();

	sampler->input = inputAccessor;
	sampler->output = outputAccessor;
	/// TODO: set interpolation method
	target->node = node;
	target->path = type;
	channel->target = target;
	channel->sampler = sampler;
	return channel;
}

/// What happens if not all data is filled?
void Animation::writeResult(const AniPath type, float* data, GLTF::Node* node) {
	if (isIdentity(data, type)) {
		return;
	}
	GLTF::Animation* animation = nullptr;
	for (GLTF::Animation* a : m_animations) {
		for (GLTF::Animation::Channel* c : a->channels) {
			GLTF::Node* tnode = c->target->node;
			if (node == tnode) {
				if (c->target->path == type) {
					animation = a;
					c->sampler->output->writeComponentAtIndex(m_cur_frame, data);
					c->sampler->output->computeMinMax();
					break;
				}
			}
		}
		if (animation != nullptr) {
			break;
		}
	}
	if (animation == nullptr) {
		animation = new GLTF::Animation();
		GLTF::Animation::Channel* channel = createChannel(type, node, data);
		animation->channels.push_back(channel);
		m_animations.push_back(animation);
	}
}