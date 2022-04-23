#ifndef PARCHED_RENDER_HPP
#define PARCHED_RENDER_HPP
#include "core/vector.hpp"
#include "gl/device.hpp"
#include "gl/renderer.hpp"

namespace game
{
	namespace detail
	{
		constexpr std::size_t initial_buf_size = 8096;
	}

	struct BallState
	{
		tz::Vec2 position;
		float pad0[2];
		tz::Vec3 colour;
		float scale;
		std::uint32_t is_active;
		float pad2[3];
	};

	class RenderState
	{
	public:
		RenderState();
		std::size_t ball_capacity() const;
		std::size_t ball_count() const;
		std::span<const BallState> get_balls() const;
		std::span<BallState> get_balls();
		void update();
		void add_ball(tz::Vec2 position, tz::Vec3 colour, float radius = 1.0);
	private:
		tz::gl::Renderer make_renderer();

		tz::gl::Device device;
		tz::gl::ResourceHandle ball_data;
		tz::gl::Renderer renderer;
		std::size_t num_balls = 0;
	};
}

#endif // PARCHED_RENDER_HPP
