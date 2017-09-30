#include "Raw.h"

namespace Gamebuino_Meta {

Sound_Handler_Raw::Sound_Handler_Raw(Sound_Channel* chan, uint8_t* _buffer, uint32_t _length) : Sound_Handler(chan) {
	buffer = _buffer;
	length = _length;
	rewind();
};

void Sound_Handler_Raw::update() {
	channel->buffer += channel->index;
	channel->index = 0;
	
	uint32_t cursor = (uint32_t)channel->buffer - (uint32_t)buffer;
	if (cursor + 0xFFFF <= length) {
		channel->total = length - cursor;
		channel->last = true;
	}
}

void Sound_Handler_Raw::rewind() {
	channel->index = 0;
	channel->total = length > 0xFFFF ? 0xFFFF : length;
	channel->buffer = buffer;
	channel->type = Sound_Channel_Type::raw;
	
	channel->use = true;
}

}; // namespace Gamebuino_Meta
