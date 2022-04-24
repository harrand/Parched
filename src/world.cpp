#include "world.hpp"
#include "core/profiling/zone.hpp"

namespace game
{
	World::World():
	render(),
	motion(),
	time(tz::system_time())
	{
		// Main circle to indicate playing space.
		this->add_ball({0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 0.95f, BallTypeInfo<BallType::Constraint>{});
	}

	void World::add_ball(tz::Vec2 position, tz::Vec3 colour, float radius, BallInfo info)
	{
		TZ_PROFZONE("World - Add Ball", TZ_PROFCOL_GREEN);
		std::size_t id = this->render.ball_count();
		this->render.add_ball(position, colour, radius);
		this->motion.push_back
		({
			.ball_id = id,
			.position_old = position,
			.acceleration = {0.0f, 0.0f},
			.info = info
		});
	}

	void World::pop_ball()
	{
		if(this->render.ball_count() <= 1)
		{
			return;
		}
		this->motion.pop_back();
		this->render.pop_ball();
	}

	void World::erase_ball(std::size_t ball_id)
	{
		if(ball_id == this->ball_count() - 1)
		{
			this->pop_ball();
			return;
		}
		// Swap this ball with the last ball.
		std::size_t last_ball_id = this->ball_count() - 1;
		this->render.swap_balls(ball_id, last_ball_id);
		std::swap(this->motion[ball_id], this->motion[last_ball_id]);
		this->motion[ball_id].ball_id = ball_id;
		this->motion[last_ball_id].ball_id = last_ball_id;

		this->pop_ball();
	}

	void World::apply_acceleration(std::size_t ball_id, tz::Vec2 acceleration)
	{
		tz_assert(this->render.ball_count() > ball_id, "Ball ID %zu is invalid. There are only %zu balls in the world.", ball_id, this->render.ball_count());
		this->motion[ball_id].acceleration += acceleration;
	}

	const tz::Vec3& World::get_ball_colour(std::size_t ball_id) const
	{
		return this->render.get_balls()[ball_id].colour;
	}

	void World::set_ball_colour(std::size_t ball_id, tz::Vec3 colour)
	{
		this->render.get_balls()[ball_id].colour = colour;
	}

	BallType World::get_type(std::size_t ball_id) const
	{
		BallType t;
		std::visit([&t](auto&& arg)
		{
			t = arg.get_type();
		}, this->motion[ball_id].info);
		return t;
	}

	void World::update()
	{
		TZ_PROFZONE("World Update", TZ_PROFCOL_GREEN);
		tz::Duration now = tz::system_time();
		const float dt = (now - this->time).seconds<float>();

		this->time = now;

		static constexpr std::size_t sub_steps = 2;
		const float subdt = dt / static_cast<float>(sub_steps);
		for(std::size_t i = 0; i < sub_steps; i++)
		{
			TZ_PROFZONE("World Update Substep", TZ_PROFCOL_BROWN);
			this->solve_physics();
			this->motion_integration(subdt);
		}
	}

	void World::draw()
	{
		this->render.update();
	}

	std::size_t World::ball_count() const
	{
		return this->render.ball_count();
	}

	void World::motion_integration(float dt)
	{
		TZ_PROFZONE("World Motion Integration", TZ_PROFCOL_GREEN);
		// Verlet integrate.
		for(std::size_t i = 0; i < this->render.ball_count(); i++)
		{
			if(this->get_type(i) != BallType::Normal)
			{
				continue;
			}
			tz::Vec2& position_current = this->render.get_balls()[i].position;
			tz::Vec2& position_old = this->motion[i].position_old;
			tz::Vec2& acceleration = this->motion[i].acceleration;

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
		TZ_PROFZONE("World Solve Physics", TZ_PROFCOL_GREEN);
		constexpr tz::Vec2 gravity{0.0f, -3.0f};
		for(std::size_t i = 0; i < this->render.ball_count(); i++)
		{
			if(this->get_type(i) != BallType::Normal)
			{
				continue;
			}
			this->apply_acceleration(i, gravity);
		}
		this->apply_constraint();
		this->solve_collisions();
	}

	void World::apply_constraint()
	{
		TZ_PROFZONE("World Apply Constraint", TZ_PROFCOL_GREEN);
		if(this->render.ball_count() == 0)
		{
			return;	
		}

		for(std::size_t j = 0; j < this->render.ball_count(); j++)
		{
			if(this->get_type(j) != BallType::Constraint)
			{
				continue;
			}
			const tz::Vec2& arena_position = this->render.get_balls()[j].position;
			const float arena_radius = this->render.get_balls()[j].scale;
			for(std::size_t i = 0; i < this->render.ball_count(); i++)
			{
				if(this->get_type(i) != BallType::Normal)
				{
					continue;
				}
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
	}

	void World::solve_collisions()
	{
		TZ_PROFZONE("World Solve Collisions", TZ_PROFCOL_BROWN);
		auto get_pos = [this](std::size_t ball_id)->tz::Vec2&{return this->render.get_balls()[ball_id].position;};
		for(std::size_t i = 0; i < this->render.ball_count(); i++)
		{
			if(this->get_type(i) == BallType::Constraint)
			{
				continue;
			}
			for(std::size_t j = i+1; j < this->render.ball_count(); j++)
			{
				if(i == j || !this->render.get_balls()[j].is_active)
				{
					continue;
				}

				// Balls may be of different size. We get the max.
				const float radius = this->render.get_balls()[i].scale + this->render.get_balls()[j].scale;
				tz::Vec2 collision_axis = get_pos(i) - get_pos(j);
				const float dist = collision_axis.length();
				if(dist < radius)
				{
					if(this->get_type(i) == BallType::Trigger)
					{
						std::get<BallTypeInfo<BallType::Trigger>>(this->motion[i].info).on_enter(j);
					}
					else if(this->get_type(j) == BallType::Trigger)
					{
						std::get<BallTypeInfo<BallType::Trigger>>(this->motion[j].info).on_enter(i);
					}
					else
					{

						const tz::Vec2 n = collision_axis / dist;
						const float delta = radius - dist;

						if(this->get_type(i) == BallType::Selective)
						{
							if(std::get<BallTypeInfo<BallType::Selective>>(this->motion[i].info).filter(j))
							{
								get_pos(j) -= n * 0.4f * delta;
							}
						}
						else if(this->get_type(j) == BallType::Selective)
						{
							if(std::get<BallTypeInfo<BallType::Selective>>(this->motion[j].info).filter(i))
							{
								get_pos(i) += n * 0.4f * delta;
							}
						}
						else
						{
							get_pos(i) += n * 0.4f * delta;
							get_pos(j) -= n * 0.4f * delta;
						}
					}
				}
			}
		}
	}
}
