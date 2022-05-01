#ifndef __MOUSE_POSITION_HISTORY_H__
#define __MOUSE_POSITION_HISTORY_H__

#include "cgmath.h"

class MousePositionHistory
{
private:
	vec2 prev_position;
	vec2 current_position;

public:
	MousePositionHistory();
	~MousePositionHistory();
	void change_position(vec2 position);
	void make_prev_position();
	vec2 get_current_position();
	vec2 get_prev_position();
};

#endif
