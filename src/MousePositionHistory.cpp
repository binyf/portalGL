#include "MousePositionHistory.h"

MousePositionHistory::MousePositionHistory()
{
	this->current_position = vec2(0.0f, 0.0f);
	this->prev_position = vec2(0.0f, 0.0f);
}

MousePositionHistory::~MousePositionHistory()
{
}

void MousePositionHistory::change_position(vec2 position)
{
	this->current_position = position;
}

void MousePositionHistory::make_prev_position()
{
	this->prev_position = this->current_position;
}

vec2 MousePositionHistory::get_current_position()
{
	return this->current_position;
}

vec2 MousePositionHistory::get_prev_position()
{
	return this->prev_position;
}
