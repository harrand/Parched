#ifndef PARCHED_WORLD_HPP
#define PARCHED_WORLD_HPP
#include "render.hpp"
#include "ball.hpp"

#include "core/time.hpp"

namespace game
{
	struct MotionData
	{
		std::size_t ball_id;
		tz::Vec2 position_old = {0.0f, 0.0f};
		tz::Vec2 acceleration = {0.0f, 0.0f};
		BallInfo info;
	};

	class World
	{
	public:
		World();
		void add_ball(tz::Vec2 position, tz::Vec3 colour, float radius, BallInfo info = BallTypeInfo<BallType::Normal>{});
		void pop_ball();
		void erase_ball(std::size_t ball_id);
		void apply_acceleration(std::size_t ball_id, tz::Vec2 acceleration);
		const tz::Vec3& get_ball_colour(std::size_t ball_id) const;
		void set_ball_colour(std::size_t ball_id, tz::Vec3 colour);
		BallType get_type(std::size_t ball_id) const;
		void update();
		void draw();
		std::size_t ball_count() const;
	private:
		void ball_swap(std::size_t i, std::size_t j);
		void swap_last(std::size_t ball_id);
		void motion_integration(float dt);
		void solve_physics();
		void apply_constraint();
		bool sweep(std::size_t i, std::size_t j, std::span<const BallState> balls);
		void solve_collisions();
		void solve_collision(std::size_t i, std::size_t j);
		void sort();
		void debug_colour_by_id();

		RenderState render;
		std::vector<MotionData> motion;
		tz::Duration time;
	};
}

#endif // PARCHED_WORLD_HPP
