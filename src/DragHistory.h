#ifndef __DRAG_HISTORY_H__
#define __DRAG_HISTORY_H__

#include "cgmath.h"

class DragHistory
{
private:
	vec2 initial_position;
	vec2 prev_position;
	vec2 current_position;

public:
	DragHistory();
	~DragHistory();
	void start(vec2 position);
	void change_position(vec2 position);
	vec2 get_initial_position();
	vec2 get_current_position();
	vec2 get_prev_position();
};

#endif
