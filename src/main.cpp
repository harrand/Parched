#include "core/tz.hpp"
#include "render.hpp"

int main()
{
	tz::initialise
	({
		.name = "Parched"
	});
	{
		game::RenderState render;
		render.add_ball({-0.3f, 0.0f}, {0.5f, 0.0f, 1.0f}, {0.2f, 0.2f});
		while(!tz::window().is_close_requested())
		{
			tz::window().update();
			render.update();
		}
	}
	tz::terminate();
}
