add_subdirectory(engine)

add_app(
	TARGET Parched
	SOURCE_FILES
		src/main.cpp
		src/render.cpp
		src/render.hpp
	SHADER_SOURCES
		src/parched.vertex.tzsl
		src/parched.fragment.tzsl
)
