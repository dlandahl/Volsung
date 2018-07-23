
#include "Block.h"
#include "AudioDataflow.h"

Block AudioConnector::get_block() { return out->get_block(); };
uint AudioConnector::get_block_count() { return out->get_block_count(); };

Block AudioOutput::get_block() {
	return buf.get_block();
}

void AudioOutput::write_block(Block b)
{
	buf.write_block(b);
}

uint AudioOutput::get_block_count()
{
	return buf.get_block_count();
}