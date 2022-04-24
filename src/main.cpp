#include "core/tz.hpp"
#include "world.hpp"
#include <random>
#include <limits>

int main()
{
	tz::initialise
	({
		.name = "Parched"
	});
	{
		game::World world;
		std::default_random_engine rand;
		using namespace tz::literals;
		tz::Delay fixed_update{17_ms};
		while(!tz::window().is_close_requested())
		{
			tz::window().update();
			world.draw();

			if(fixed_update.done())
			{
				fixed_update.reset();
				world.update();
				if(tz::window().get_mouse_button_state().is_mouse_button_down(tz::MouseButton::Left))
				{
					tz::Vec2ui mpos = tz::window().get_mouse_position_state().get_mouse_position();
					const float aspect_ratio = tz::window().get_width() / tz::window().get_height();
					tz::Vec2 bpos = mpos;
					bpos[0] /= tz::window().get_width() * 0.5;
					bpos[0] -= 1.0f;
					bpos[1] /= tz::window().get_height() * -0.5f;
					bpos[1] += 1.0f;
					tz::Vec3 random_colour{0.0f, 0.0f, 0.0f};
					std::generate(random_colour.data().begin(), random_colour.data().end(), [&rand]()->float{return static_cast<float>(rand()) / std::numeric_limits<std::default_random_engine::result_type>::max() * 2.0f;});
					world.add_ball(bpos, random_colour, 0.1f);
				}
			}
		}
	}
	tz::terminate();
}
