#pragma once

#include "RenderVulkan/ECS/Component.hpp"
#include "RenderVulkan/Util/Typedefs.hpp"

using namespace RenderVulkan::ECS;
using namespace RenderVulkan::Util;

namespace RenderVulkan
{
	namespace Math
	{
        class Transform : public Component
        {
        public:

            void SetLocalPosition(const Vector3f& position)
            {
                localPosition = position;
                MarkDirty();
            }

            void SetLocalRotation(const Vector3f& rotation)
            {
                localRotation = rotation;
                MarkDirty();
            }

            void SetLocalScale(const Vector3f& scale)
            {
                localScale = scale;
                MarkDirty();
            }

            Vector3f GetLocalPosition() const
            {
                return localPosition;
            }

            Vector3f GetLocalRotation() const
            {
                return localRotation;
            }

            Vector3f GetLocalScale() const
            {
                return localScale;
            }

            Vector3f GetWorldPosition()
            {
                UpdateWorldMatrixIfNeeded();
                return Vector3f(worldMatrix[3]);
            }

            Vector3f GetForwardVector() const
            {
                return glm::normalize(Vector3f(worldMatrix * Vector4f(0, 0, 1, 0)));
            }

            Vector3f GetRightVector() const
            {
                return glm::normalize(Vector3f(worldMatrix * Vector4f(1, 0, 0, 0)));
            }

            Vector3f GetUpVector() const
            {
                return glm::normalize(Vector3f(worldMatrix * Vector4f(0, 1, 0, 0)));
            }

            Matrix4x4f GetWorldMatrix()
            {
                UpdateWorldMatrixIfNeeded();
                return worldMatrix;
            }

            void Translate(const Vector3f& translation)
            {
                SetLocalPosition(localPosition + translation);
            }

            void Rotate(const Vector3f& rotation)
            {
                SetLocalRotation(localRotation + rotation);
                WrapAngles();
            }

            void ScaleBy(const Vector3f& scale)
            {
                SetLocalScale(localScale * scale);
            }

            void SetParent(Shared<Transform> parent)
            {
                parentTransform = parent;
                MarkDirty();
            }

            static Shared<Transform> Create()
            {
                return std::make_shared<Transform>();
            }

        private:

            void MarkDirty()
            {
                if (!isDirty)
                    isDirty = true;
            }

            void UpdateWorldMatrixIfNeeded()
            {
                if (isDirty)
                {
                    RecalculateWorldMatrix();
                    isDirty = false;
                }
            }

            void RecalculateWorldMatrix()
            {
                Matrix4x4f scaleMatrix = glm::scale(Matrix4x4f(1.0f), localScale);
                Matrix4x4f rotationMatrix = glm::toMat4(Quaternionf(glm::radians(-localRotation)));
                Matrix4x4f translationMatrix = glm::translate(Matrix4x4f(1.0f), -localPosition);

                worldMatrix = translationMatrix * rotationMatrix * scaleMatrix;

                if (parentTransform)
                    worldMatrix = parentTransform->GetWorldMatrix() * worldMatrix;
            }

            void WrapAngles()
            {
                localRotation = glm::mod(localRotation, Vector3f(360.0f));
            }

            Vector3f localPosition = Vector3f(0.0f, 0.0f, 0.0f);
            Vector3f localRotation = Vector3f(0.0f, 0.0f, 0.0f);
            Vector3f localScale = Vector3f(1.0f, 1.0f, 1.0f);
            Matrix4x4f worldMatrix = Matrix4x4f(1.0f);
            bool isDirty = true;

            Shared<Transform> parentTransform = nullptr;
        };
	}
}