#include "externals.h"
#include "Animation.h"

Animation::Animation(int nodeid, unsigned int size) {
	this->m_size = size;
	GLTF::Animation* animation = new GLTF::Animation();
	m_animations.push_back(animation);
}

Animation::~Animation() {}

void Animation::setFrame(const unsigned int& inputframe) {
	this->m_cur_frame = inputframe;
}

/// What happens if not all data is filled?
void Animation::writeResult(const AniPath type, float* data) {
	GLTF::Animation* animation = m_animations.back();
	bool found = false;
	for (GLTF::Animation::Channel* channel : animation->channels) {
		if (channel->target->path == type) {
			channel->sampler->output->writeComponentAtIndex(m_cur_frame, data);
			found = true;
			break;
		}
	}
	if (!found) {
		std::vector<float> __i;
		for (int i = 0; i < m_size; ++i) {
			__i.push_back(i);
		}
		std::vector<float> __d(m_size, 0);
		GLTF::Accessor* inputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::SCALAR, 
			GLTF::Constants::WebGL::FLOAT, (unsigned char*)__i.data(), m_size, (GLTF::Constants::WebGL) - 1);
		GLTF::Accessor* outputAccessor;
		switch (type) {
		case AniPath::TRANSLATION:
		case AniPath::SCALE:
			outputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::VEC3,
				GLTF::Constants::WebGL::FLOAT, (unsigned char*)__d.data(), m_size, (GLTF::Constants::WebGL) - 1);
			break;

		default: //case AniPath::ROTATION:
			outputAccessor = new GLTF::Accessor(GLTF::Accessor::Type::VEC4,
				GLTF::Constants::WebGL::FLOAT, (unsigned char*)__d.data(), m_size, (GLTF::Constants::WebGL) - 1);
			break;
		}
		outputAccessor->writeComponentAtIndex(m_cur_frame, data);

		GLTF::Animation::Channel* channel = new GLTF::Animation::Channel();
		GLTF::Animation::Channel::Target* target = new GLTF::Animation::Channel::Target();
		GLTF::Animation::Sampler* sampler = new GLTF::Animation::Sampler();

		sampler->input = inputAccessor;
		sampler->output = outputAccessor;
		/// TODO: set interpolation method
		/// TODO: set node of target
		target->path = type;
		channel->target = target;
		channel->sampler = sampler;
		animation->channels.push_back(channel);
	}
}