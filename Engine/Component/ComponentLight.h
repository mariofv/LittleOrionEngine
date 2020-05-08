#ifndef _COMPONENTLIGHT_H
#define _COMPONENTLIGHT_H

#include "Component.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

class GameObject;

class ComponentLight : public Component
{
public:
	enum class LightType
	{
		POINT_LIGHT,
		SPOT_LIGHT,
		DIRECTIONAL_LIGHT
	};

	class PointLight
	{
		public:
			float range = 10.f; 
			float constant = 1.f;
			float linear = 0.045f;
			float quadratic = 0.0075f;

		public: 
			void ChangePointLightAttenuationValues(float range)
			{
				float augmented_range = 10 * range;
				linear = 4.5F / (10 * augmented_range);
				quadratic = 75.0F / (augmented_range*augmented_range);
			}
	};

	class SpotLight
	{
		public: 
			void SetSpotAngle(float new_angle)
			{
				spot_angle = new_angle;
				cutoff = cos(DegToRad(spot_angle / 2.f));
				outer_cutoff = cos(DegToRad((1 + edge_softness) * spot_angle / 2.f));
			}

			void SetEdgeSoftness(float new_softness)
			{
				edge_softness = new_softness;
				outer_cutoff = cos(DegToRad((1 + edge_softness) * spot_angle / 2.f));
			}

			void ChangeSpotLightAttenuationValues(float range)
			{
				float augmented_range = 10 * range;
				linear = 4.5F / augmented_range;
				quadratic = 75.F / (augmented_range*augmented_range);
			}

		public: 
			float spot_angle = 30.f; // Dear reader do not modify this directly, use SetSpotAngle!
			float cutoff = cos(DegToRad(15.f)); // Dear reader do not modify this directly, use SetSpotAngle!

			float edge_softness = 0.1f; // Dear reader do not modify this directly, use SetEdgeSoftness!
			float outer_cutoff = cos(DegToRad(16.5f)); // Dear reader do not modify this directly, use SetEdgeSoftness!

			float range = 1.f;
			float constant = 1.0f;
			float linear = 0.045f;
			float quadratic = 0.0075f;
	};

	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;

	//Copy and move
	ComponentLight(const ComponentLight& component_to_copy) = default;
	ComponentLight(ComponentLight&& component_to_move) = default;

	ComponentLight & operator=(const ComponentLight & component_to_copy) = default;
	ComponentLight & operator=(ComponentLight && component_to_move) = default;


	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;

public:
	float light_color[3] = {1.0f, 1.0f, 1.0f};
	float light_intensity = 1.f; 
	LightType light_type = LightType::POINT_LIGHT;

	PointLight point_light_parameters;
	SpotLight spot_light_parameters;

private:
	friend class PanelComponent;
};

#endif // !_COMPONENTLIGHT_H

