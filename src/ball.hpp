#ifndef PARCHED_BALL_HPP
#define PARCHED_BALL_HPP
#include "core/callback.hpp"

namespace game
{
	enum class BallType
	{
		Normal,
		Constraint,
		Trigger
	};

	template<BallType T>
	struct BallTypeInfo
	{
		constexpr BallType get_type() const{return T;}
	};

	template<>
	struct BallTypeInfo<BallType::Trigger>
	{
		constexpr BallType get_type() const{return BallType::Trigger;}
		tz::Callback<std::size_t> on_enter;
		tz::Callback<std::size_t> on_exit;
	};

	using BallInfo = std::variant<BallTypeInfo<BallType::Normal>, BallTypeInfo<BallType::Constraint>, BallTypeInfo<BallType::Trigger>>;
}

#endif // PARCHED_BALL_HPP
