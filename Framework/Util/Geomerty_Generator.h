#pragma once
#include <vector>

namespace Framework
{
	class Geometry_Generator final
	{
	public:
		template<typename T>
		static std::vector<T> ScreenQuad_Vertices(float width = 2.0f, float height = 2.0f)
		{
			return {
				T(Vector3( -width / 2.0f, -height / 2.0f, 0.0f ), Vector2( 0.0f, 1.0f )),
				T(Vector3( -width / 2.0f, +height / 2.0f, 0.0f), Vector2(0.0f, 0.0f)),
				T(Vector3( +width / 2.0f, -height / 2.0f, 0.0f), Vector2(1.0f, 1.0f)),
				T(Vector3( +width / 2.0f, +height / 2.0f, 0.0f), Vector2(1.0f, 0.0f))
			};
		}
		static std::vector<uint> ScreenQuad_Indices()
		{
			return { 0, 1, 2, 2, 1, 3 };
		}

		template<typename T>
		static std::vector<T> Triangle_Vertices()
		{
			return { T(-1, -1, 0),  T(1, -1, 0), T(0, 1, 0)};
		}

		static std::vector<unsigned int> Triangle_Indices()
		{
			return { 0, 1, 2 };
		}

		template<typename T>
		static std::vector<T> Square_Vertices()
		{
			return { T(Vector3(-1, -1, 0), Vector2(0, 1)), T(Vector3(-1, 1, 0), Vector2(0, 0)), 
					 T(Vector3(1, 1, 0),  Vector2(1, 0)),  T(Vector3(1, -1, 0), Vector2(1, 1)) };
		}

		template<typename T>
		static std::vector<T> Cube_Vertices(float size = 0.5f)
		{
			return { 
				T(Vector3(-size, +size, -size), Vector2(0.25f, 0.33f), Vector3(0,0,-1)),
				T(Vector3(-size, -size, -size), Vector2(0.25f, 0.66f), Vector3(0,0,-1)),
				T(Vector3(+size, +size, -size), Vector2(0.50f, 0.33f), Vector3(0,0,-1)),
				T(Vector3(+size, -size, -size), Vector2(0.50f, 0.66f), Vector3(0,0,-1)),

				T(Vector3(+size, +size, +size), Vector2(0.75f, 0.33f), Vector3(0,0,1)),
				T(Vector3(+size, -size, +size), Vector2(0.75f, 0.66f), Vector3(0,0,1)),
				T(Vector3(-size, +size, +size), Vector2(1.00f, 0.33f), Vector3(0,0,1)),
				T(Vector3(-size, -size, +size), Vector2(1.00f, 0.66f), Vector3(0,0,1)),

				T(Vector3(-size, +size, +size), Vector2(0.00f, 0.33f), Vector3(-1,0,0)),
				T(Vector3(-size, -size, +size), Vector2(0.00f, 0.66f), Vector3(-1,0,0)),
				T(Vector3(-size, +size, -size), Vector2(0.25f, 0.33f), Vector3(-1,0,0)),
				T(Vector3(-size, -size, -size), Vector2(0.25f, 0.66f), Vector3(-1,0,0)),

				T(Vector3(+size, +size, -size), Vector2(0.50f, 0.33f), Vector3(+1,0,0)),
				T(Vector3(+size, -size, -size), Vector2(0.50f, 0.66f), Vector3(+1,0,0)),
				T(Vector3(+size, +size, +size), Vector2(0.75f, 0.33f), Vector3(+1,0,0)),
				T(Vector3(+size, -size, +size), Vector2(0.75f, 0.66f), Vector3(+1,0,0)),

				T(Vector3(-size, +size, +size), Vector2(0.25f, 0.00f), Vector3(0,1,0)),
				T(Vector3(-size, +size, -size), Vector2(0.25f, 0.33f), Vector3(0,1,0)),
				T(Vector3(+size, +size, +size), Vector2(0.50f, 0.00f), Vector3(0,1,0)),
				T(Vector3(+size, +size, -size), Vector2(0.50f, 0.33f), Vector3(0,1,0)),

				T(Vector3(-size, -size, -size), Vector2(0.25f, 0.66f), Vector3(0,-1,0)),
				T(Vector3(-size, -size, +size), Vector2(0.25f, 0.99f), Vector3(0,-1,0)),
				T(Vector3(+size, -size, -size), Vector2(0.50f, 0.66f), Vector3(0,-1,0)),
				T(Vector3(+size, -size, +size), Vector2(0.50f, 0.99f), Vector3(0,-1,0)),
			};
		}

		static std::vector<unsigned int> Square_Indices()
		{
			return { 0, 2, 1, 0, 3, 2 };
		}

		static std::vector<unsigned int> Cube_Indices()
		{
			return {
				0, 2, 1, // front
				1, 2, 3,

				4, 6, 5, // back
				5, 6, 7,
				
				8, 10, 9, //top
				9, 10, 11,

				12, 14, 13, //bottom
				13, 14, 15,

				16, 18, 17,// left
				17, 18, 19,

				20, 22, 21,//right
				21, 22, 23,
			};
		}

		template<typename T>
		static std::vector<T> Gizmo_Vertices(float smallest = 0.01f, float biggest = 1.0f)
		{
			return {
				T(Vector3(-smallest, +smallest,        0), Color4(0,0,1,1)),
				T(Vector3(-smallest, -smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, +smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest,        0), Color4(0,0,1,1)),
												  
				T(Vector3(+smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(-smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(-smallest, -smallest, +biggest), Color4(0,0,1,1)),
												  
				T(Vector3(-smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(-smallest, -smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(-smallest, +smallest,        0), Color4(0,0,1,1)),
				T(Vector3(-smallest, -smallest,        0), Color4(0,0,1,1)),
												  
				T(Vector3(+smallest, +smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest, +biggest), Color4(0,0,1,1)),
												  
				T(Vector3(-smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(-smallest, +smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, +smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(+smallest, +smallest,        0), Color4(0,0,1,1)),
												  
				T(Vector3(-smallest, -smallest,        0), Color4(0,0,1,1)),
				T(Vector3(-smallest, -smallest, +biggest), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest,        0), Color4(0,0,1,1)),
				T(Vector3(+smallest, -smallest, +biggest), Color4(0,0,1,1)),


				// Y
				T(Vector3(-smallest, +biggest, -smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest,        0, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest, +biggest, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, -smallest), Color4(0,1,0,1)),
												   
				T(Vector3(+smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, +smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest,        0, +smallest), Color4(0,1,0,1)),
												   
				T(Vector3(-smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest,        0, +smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest, +biggest, -smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest,        0, -smallest), Color4(0,1,0,1)),
												   
				T(Vector3(+smallest, +biggest, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, +smallest), Color4(0,1,0,1)),

				T(Vector3(-smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest, +biggest, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest, +biggest, +smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest, +biggest, -smallest), Color4(0,1,0,1)),

				T(Vector3(-smallest,        0, -smallest), Color4(0,1,0,1)),
				T(Vector3(-smallest,        0, +smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, -smallest), Color4(0,1,0,1)),
				T(Vector3(+smallest,        0, +smallest), Color4(0,1,0,1)),


				// X
				T(Vector3(       0, +smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(       0, -smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, +smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, -smallest), Color4(1,0,0,1)),

				T(Vector3(+biggest, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(       0, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(       0, -smallest, +smallest), Color4(1,0,0,1)),

				T(Vector3(       0, -smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(       0, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(       0, +smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(       0, -smallest, -smallest), Color4(1,0,0,1)),

				T(Vector3(+biggest, +smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, +smallest), Color4(1,0,0,1)),

				T(Vector3(       0, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(       0, +smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, +smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, +smallest, -smallest), Color4(1,0,0,1)),

				T(Vector3(       0, -smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(       0, -smallest, +smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, -smallest), Color4(1,0,0,1)),
				T(Vector3(+biggest, -smallest, +smallest), Color4(1,0,0,1)),
			};
		}

		static std::vector<unsigned int> Gizmo_Indices()
		{
			std::vector<unsigned int> result;
			result = Cube_Indices();

			for (auto i : Cube_Indices())
				result.emplace_back(i + 24);
			for (auto i : Cube_Indices())
				result.emplace_back(i + 48);

			return result;
		}

		template<typename T>
		static std::vector<T> Grid_Vertices()
		{
			std::vector<T> result;
			for (int i = -100; i <= 100; i++)
			{
				result.push_back(T(Vector3(i, 0, -100), Color4(0, 0, 0, 1)));
				result.push_back(T(Vector3(i, 0, +100), Color4(0, 0, 0, 1)));
				result.push_back(T(Vector3(-100, 0, i), Color4(0, 0, 0, 1)));
				result.push_back(T(Vector3(+100, 0, i), Color4(0, 0, 0, 1)));
			}
			return result;
		}

		static std::vector<unsigned int> Grid_Indices()
		{
			std::vector<unsigned int> result;
			for (int i = 0; i <= 200; i++)
			{
				for(int k = 0 ; k < 4; k++)
					result.push_back(i * 4 + k);
			}
			return result;
		}
	};

}