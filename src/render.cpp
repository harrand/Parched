#include "render.hpp"

#include "gl/resource.hpp"
#include "gl/imported_shaders.hpp"
#include ImportedShaderHeader(parched, vertex)
#include ImportedShaderHeader(parched, fragment)

namespace game
{
	RenderState::RenderState():
	device(),
	ball_data(tz::nullhand),
	renderer(this->make_renderer())
	{
		tz_assert(this->ball_capacity() == detail::initial_buf_size, "RenderState had unexpected initial ball capacity. Expected %zu, got %zu", detail::initial_buf_size, this->ball_capacity());
		tz_assert(this->ball_count() == 0, "RenderState was not empty upon construction. Ball count = %zu when it should've been 0.", this->ball_count());
	}

	std::size_t RenderState::ball_capacity() const
	{
		return this->renderer.get_resource(this->ball_data)->data_as<BallState>().size();
	}

	std::size_t RenderState::ball_count() const
	{
		return this->num_balls;
	}

	std::span<const BallState> RenderState::get_balls() const
	{
		return {this->renderer.get_resource(this->ball_data)->data_as<BallState>().begin(), this->ball_count()};
	}

	std::span<BallState> RenderState::get_balls()
	{
		return {this->renderer.get_resource(this->ball_data)->data_as<BallState>().begin(), this->ball_count()};
	}

	void RenderState::update()
	{
		// Assume each ball is 1 triangle.
		this->renderer.render(this->ball_capacity());
	}

	void RenderState::add_ball(tz::Vec2 position, tz::Vec3 colour, float radius)
	{
		auto& ball = this->renderer.get_resource(this->ball_data)->data_as<BallState>()[this->num_balls];
		ball.position = position;
		ball.colour = colour;
		ball.scale = radius;
		ball.is_active = true;
		this->num_balls++;
		tz_assert(this->num_balls < this->ball_capacity(), "Buffer resource ran out of space, too many balls at once. TODO: Resize the buffer.");
	}

	tz::gl::Renderer RenderState::make_renderer()
	{
		tz::gl::RendererInfo rinfo;
		rinfo.shader().set_shader(tz::gl::ShaderStage::Vertex, ImportedShaderSource(parched, vertex));
		rinfo.shader().set_shader(tz::gl::ShaderStage::Fragment, ImportedShaderSource(parched, fragment));

		constexpr BallState default_ball
		{
			.position = {0.0f, 0.0f},
			.colour = {1.0f, 0.0f, 0.0f},
			.scale = 1.0f,
			.is_active = false
		};

		std::vector<BallState> initial_ball_data(detail::initial_buf_size, default_ball);
		
		tz::gl::BufferResource ball_buf = tz::gl::BufferResource::from_many
		(
			initial_ball_data,
			tz::gl::ResourceAccess::DynamicVariable
		);
		this->ball_data = rinfo.add_resource(ball_buf);
		return this->device.create_renderer(rinfo);
	}
}

