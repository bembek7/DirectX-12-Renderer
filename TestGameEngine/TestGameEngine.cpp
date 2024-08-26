#include "pch.h"
#include "CppUnitTest.h"
#include "../Actor.h"
#include "../SceneComponent.h"
#include <DirectXMath.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;



namespace TestGameEngine
{
	using namespace DirectX;

	XMVECTOR epsilon = { 0.005f, 0.005f, 0.005f };

	static bool AreVectorsEqual(XMVECTOR v1, XMVECTOR v2) noexcept
	{
		return XMVector3NearEqual(v1, v2, epsilon);
	}
	
	TEST_CLASS(TestGameEngine)
	{
	public:
		TEST_METHOD(TestActorInitialization)
		{
			Assert::IsTrue(true);
		}
	};
}
