#ifndef PARCHED_WORLD_HPP
#define PARCHED_WORLD_HPP
#include "render.hpp"

#include "core/time.hpp"

namespace game
{
	struct MotionData
	{
		std::size_t ball_id;
		tz::Vec2 position_old = {0.0f, 0.0f};
		tz::Vec2 acceleration = {0.0f, 0.0f};
	};

	class World
	{
	public:
		World();
		void add_ball(tz::Vec2 position, tz::Vec3 colour, float radius);
		void apply_acceleration(std::size_t ball_id, tz::Vec2 acceleration);
		void update();
		void draw();
	private:
		void motion_integration(float dt);
		void solve_physics();
		void apply_constraint();
		void solve_collisions();

		RenderState render;
		std::vector<MotionData> motion;
		tz::Duration time;
	};
}

#endif // PARCHED_WORLD_HPP
