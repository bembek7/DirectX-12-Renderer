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
	static bool AreVectorsEqual(XMVECTOR v1, XMFLOAT3 v2) noexcept
	{
		return AreVectorsEqual(v1, XMLoadFloat3(&v2));
	}
	static bool AreVectorsEqual(XMFLOAT3 v1, XMVECTOR v2) noexcept
	{
		return AreVectorsEqual(XMLoadFloat3(&v1), v2);
	}
	static bool AreVectorsEqual(XMFLOAT3 v1, XMFLOAT3 v2) noexcept
	{
		return AreVectorsEqual(XMLoadFloat3(&v1), XMLoadFloat3(&v2));
	}

	TEST_CLASS(TestGameEngine)
	{
	public:
		TEST_METHOD(TestActorInitialization)
		{
			Actor actor;
			XMFLOAT3 expectedLocation = { 0.f, 0.f, 0.f };
			XMFLOAT3 expectedScale = { 1.f, 1.f, 1.f };
			XMFLOAT3 expectedRotation = { 0.f, 0.f, 0.f };

			Assert::IsTrue(AreVectorsEqual(actor.GetActorLocation(), expectedLocation));
			Assert::IsTrue(AreVectorsEqual(actor.GetActorScale(), expectedScale));
			Assert::IsTrue(AreVectorsEqual(actor.GetActorRotation(), expectedRotation));
		}

		TEST_METHOD(TestSetTransformations)
		{
			Actor actor;

			XMFLOAT3 newLocation = { 10.f, 20.f, 30.f };
			actor.SetActorLocation(newLocation);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorLocation(), newLocation));

			XMFLOAT3 newScale = { 2.f, 3.f, 4.f };
			actor.SetActorScale(newScale);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorScale(), newScale));

			XMFLOAT3 newRotation = { XM_PI / 2.f, XM_PI / 4.f, XM_PI / 6.f };
			actor.SetActorRotation(newRotation);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorRotation(), newRotation));
		}
		TEST_METHOD(TestTransformPropagation)
		{
			SceneComponent rootComponent(nullptr);
			SceneComponent childComponent(&rootComponent);

			rootComponent.SetRelativeLocation(XMFLOAT3{ 10.f, 5.f, 0.f });
			rootComponent.SetRelativeScale(XMFLOAT3{ 2.f, 2.f, 2.f });
			rootComponent.SetRelativeRotation(XMFLOAT3{ 0.f, XM_PI / 4.f, 0.f });

			childComponent.SetRelativeLocation(XMFLOAT3{ 1.f, 0.f, 0.f });
			childComponent.SetRelativeScale(XMFLOAT3{ 0.5f, 0.5f, 0.5f });
			childComponent.SetRelativeRotation(XMFLOAT3{ 0.f, XM_PI / 4.f, 0.f });

			XMVECTOR expectedLocation = XMVectorSet(11.f, 5.f, 0.f, 0.f);
			XMVECTOR expectedScale = XMVectorSet(1.f, 1.f, 1.f, 0.f);
			XMVECTOR expectedRotation = XMVectorSet(0.f, XM_PI / 2.f, 0.f, 0.f);

			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentLocationVector(), expectedLocation));
			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentScaleVector(), expectedScale));
			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentRotationVector(), expectedRotation));
		}

		TEST_METHOD(TestNegativeScaling)
		{
			Actor actor;
			XMFLOAT3 negativeScale = { -1.f, -2.f, -3.f };

			actor.SetActorScale(negativeScale);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorScale(), negativeScale));

			SceneComponent rootComponent(nullptr);
			SceneComponent childComponent(&rootComponent);

			rootComponent.SetRelativeScale(negativeScale);
			XMFLOAT3 childScale = { 0.5f, 0.5f, 0.5f };
			childComponent.SetRelativeScale(childScale);

			XMFLOAT3 expectedChildScale = { -0.5f, -1.f, -1.5f };
			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentScale(), expectedChildScale));
		}

		TEST_METHOD(TestActorTransformModification)
		{
			Actor actor;

			XMFLOAT3 initialLocation = { 0.f, 0.f, 0.f };
			XMFLOAT3 locationToAdd = { 5.f, 10.f, 15.f };
			XMFLOAT3 expectedLocation = { 5.f, 10.f, 15.f };

			actor.AddActorLocation(locationToAdd);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorLocation(), expectedLocation));

			XMFLOAT3 initialScale = { 1.f, 1.f, 1.f };
			XMFLOAT3 scaleToAdd = { 1.f, 1.f, 1.f };
			XMFLOAT3 expectedScale = { 2.f, 2.f, 2.f };

			actor.AddActorScale(scaleToAdd);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorScale(), expectedScale));

			XMFLOAT3 initialRotation = { 0.f, 0.f, 0.f };
			XMFLOAT3 rotationToAdd = { 0.f, XM_PI / 4.f, 0.f };
			XMFLOAT3 expectedRotation = { 0.f, XM_PI / 4.f, 0.f };

			actor.AddActorRotation(rotationToAdd);
			Assert::IsTrue(AreVectorsEqual(actor.GetActorRotation(), expectedRotation));
		}

		TEST_METHOD(TestComplexHierarchyTransformPropagation)
		{
			SceneComponent rootComponent(nullptr);
			rootComponent.SetRelativeLocation(XMFLOAT3{ 10.f, 5.f, 0.f });
			rootComponent.SetRelativeScale(XMFLOAT3{ 2.f, 2.f, 2.f });
			rootComponent.SetRelativeRotation(XMFLOAT3{ 0.f, XM_PI / 4.f, 0.f });

			SceneComponent firstChild(&rootComponent);
			firstChild.SetRelativeLocation(XMFLOAT3{ 3.f, 0.f, 0.f });
			firstChild.SetRelativeScale(XMFLOAT3{ 0.5f, 0.5f, 0.5f });
			firstChild.SetRelativeRotation(XMFLOAT3{ 0.f, XM_PI / 4.f, 0.f });

			SceneComponent secondChild(&firstChild);
			secondChild.SetRelativeLocation(XMFLOAT3{ 2.f, 0.f, 0.f });
			secondChild.SetRelativeScale(XMFLOAT3{ 0.5f, 0.5f, 0.5f });
			secondChild.SetRelativeRotation(XMFLOAT3{ 0.f, XM_PI / 4.f, 0.f });

			XMVECTOR expectedLocation = XMVectorSet(15.f, 5.f, 0.f, 0.f);
			XMVECTOR expectedScale = XMVectorSet(0.5f, 0.5f, 0.5f, 0.f);
			XMVECTOR expectedRotation = XMVectorSet(0.f, 3 * XM_PI / 4.f, 0.f, 0.f);

			Assert::IsTrue(AreVectorsEqual(secondChild.GetComponentLocationVector(), expectedLocation));
			Assert::IsTrue(AreVectorsEqual(secondChild.GetComponentScaleVector(), expectedScale));
			Assert::IsTrue(AreVectorsEqual(secondChild.GetComponentRotationVector(), expectedRotation));
		}

		TEST_METHOD(TestIdentityTransformations)
		{
			Actor actor;

			XMFLOAT3 identityLocation = { 0.f, 0.f, 0.f };
			XMFLOAT3 identityScale = { 1.f, 1.f, 1.f };
			XMFLOAT3 identityRotation = { 0.f, 0.f, 0.f };

			actor.SetActorLocation(identityLocation);
			actor.SetActorScale(identityScale);
			actor.SetActorRotation(identityRotation);

			Assert::IsTrue(AreVectorsEqual(actor.GetActorLocation(), identityLocation));
			Assert::IsTrue(AreVectorsEqual(actor.GetActorScale(), identityScale));
			Assert::IsTrue(AreVectorsEqual(actor.GetActorRotation(), identityRotation));

			SceneComponent rootComponent(nullptr);
			SceneComponent childComponent(&rootComponent);

			rootComponent.SetRelativeLocation(identityLocation);
			rootComponent.SetRelativeScale(identityScale);
			rootComponent.SetRelativeRotation(identityRotation);

			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentLocationVector(), identityLocation));
			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentScaleVector(), identityScale));
			Assert::IsTrue(AreVectorsEqual(childComponent.GetComponentRotationVector(), identityRotation));
		}
	};
}