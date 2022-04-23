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
		render.add_ball({0.0f, 0.0f}, {0.5f, 0.0f, 1.0f}, 0.05f);
		while(!tz::window().is_close_requested())
		{
			tz::window().update();
			render.update();

			if(tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left))
			{
				tz::Vec2ui mpos = tz::window().get_mouse_position_state().get_mouse_position();
				const float aspect_ratio = tz::window().get_width() / tz::window().get_height();
				tz::Vec2 bpos = mpos;
				bpos[0] /= tz::window().get_width() * 0.5;
				bpos[0] -= 1.0f;
				bpos[1] /= tz::window().get_height() * -0.5f;
				bpos[1] += 1.0f;
				render.add_ball(bpos, {0.5f, 0.0f, 1.0f}, 0.1f);
			}

		}
	}
	tz::terminate();
}
