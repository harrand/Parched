add_subdirectory(engine)

add_app(
	TARGET Parched
	SOURCE_FILES
		src/ball.hpp
		src/main.cpp
		src/render.cpp
		src/render.hpp
		src/world.cpp
		src/world.hpp
	SHADER_SOURCES
		src/parched.vertex.tzsl
		src/parched.fragment.tzsl
)
