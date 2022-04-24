#include "world.hpp"

namespace game
{
	World::World():
	render(),
	motion(),
	time(tz::system_time())
	{
		// Main circle to indicate playing space.
		this->add_ball({0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.95f);
	}

	void World::add_ball(tz::Vec2 position, tz::Vec3 colour, float radius)
	{
		std::size_t id = this->render.ball_count();
		this->render.add_ball(position, colour, radius);
		this->motion.push_back
		({
			.ball_id = id,
			.position_old = position,
		});
	}

	void World::apply_acceleration(std::size_t ball_id, tz::Vec2 acceleration)
	{
		tz_assert(this->render.ball_count() > ball_id, "Ball ID %zu is invalid. There are only %zu balls in the world.", ball_id, this->render.ball_count());
		this->motion[ball_id].acceleration += acceleration;
	}

	void World::update()
	{
		tz::Duration now = tz::system_time();
		const float dt = (now - this->time).seconds<float>();

		this->time = now;

		static constexpr std::size_t sub_steps = 3;
		const float subdt = dt / sub_steps;
		for(std::size_t i = 0; i < sub_steps; i++)
		{
			this->motion_integration(subdt);
			this->solve_physics();
		}
	}

	void World::draw()
	{
		this->render.update();
	}

	void World::motion_integration(float dt)
	{
		// Verlet integrate.
		for(std::size_t i = 1; i < this->render.ball_count(); i++)
		{
			tz::Vec2& position_current = this->render.get_balls()[i].position;
			tz::Vec2& position_old = this->motion[i].position_old;
			tz::Vec2 acceleration = this->motion[i].acceleration;

			const tz::Vec2 velocity = position_current - position_old;
			// Save current position.
			position_old = position_current;
			// Do verlet.
			position_current = position_current + velocity + acceleration * dt * dt;
			// Reset acceleration.
			acceleration = {0.0f, 0.0f};
		}
	}

	void World::solve_physics()
	{
		constexpr tz::Vec2 gravity{0.0f, -0.1f};
		for(std::size_t i = 1; i < this->render.ball_count(); i++)
		{
			this->apply_acceleration(i, gravity);
		}
		this->apply_constraint();
		this->solve_collisions();
	}

	void World::apply_constraint()
	{
		if(this->render.ball_count() == 0)
		{
			return;	
		}

		const tz::Vec2& arena_position = this->render.get_balls().front().position;
		const float arena_radius = this->render.get_balls().front().scale;
		for(std::size_t i = 0; i < this->render.ball_count(); i++)
		{
			const tz::Vec2 to_obj = this->render.get_balls()[i].position - arena_position;
			const float dist = to_obj.length();
			const float ball_radius = this->render.get_balls()[i].scale;
			if(dist > arena_radius - ball_radius)
			{
				const tz::Vec2 n = to_obj / dist;
				this->render.get_balls()[i].position = arena_position + n * (arena_radius - ball_radius);
			}
		}
	}

	void World::solve_collisions()
	{
		auto get_pos = [this](std::size_t ball_id)->tz::Vec2&{return this->render.get_balls()[ball_id].position;};
		for(std::size_t i = 1; i < this->render.ball_count(); i++)
		{
			for(std::size_t j = 1; j < this->render.ball_count(); j++)
			{
				if(i == j)
				{
					continue;
				}

				// Balls may be of different size. We get the max.
				const float radius = this->render.get_balls()[i].scale + this->render.get_balls()[j].scale;
				tz::Vec2 collision_axis = get_pos(i) - get_pos(j);
				const float dist = collision_axis.length();
				if(dist < radius)
				{
					const tz::Vec2 n = collision_axis / dist;
					const float delta = radius - dist;
					get_pos(i) += n * 0.5f * delta;
					get_pos(j) -= n * 0.5f * delta;
				}
			}
		}
	}
}
