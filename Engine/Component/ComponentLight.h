#ifndef _COMPONENTLIGHT_H
#define _COMPONENTLIGHT_H

#include "Component.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

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
			float range = 160.f; 
			float constant = 1.f;
			float linear = 0.027f;
			float quadratic = 0.0028f;

		public: 
			void ChangePointLightAttenuationValues(float range)
			{
				linear = 4.5 / range;
				quadratic = 75.0 / (range*range);
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
				linear = 4.5 / range;
				quadratic = 75.0 / (range*range);
			}

		public: 
			float spot_angle = 30.f; // Dear reader do not modify this directly, use SetSpotAngle!
			float cutoff = cos(DegToRad(30.f)); // Dear reader do not modify this directly, use SetSpotAngle!

			float edge_softness = 0.1f; // Dear reader do not modify this directly, use SetEdgeSoftness!
			float outer_cutoff = cos(DegToRad(33.f)); // Dear reader do not modify this directly, use SetEdgeSoftness!

			float range = 50.f;
			float linear = 0.09f;
			float quadratic = 0.032f;
			float constant = 1.0f;
	};

	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;

	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;

public:
	float light_color[3] = { 1.0f, 1.0f, 1.0f};
	float light_intensity = 1.f; 
	LightType light_type = LightType::DIRECTIONAL_LIGHT;

	PointLight point_light_parameters;
	SpotLight spot_light_parameters;

private:
	friend class PanelComponent;
};

#endif // !_COMPONENTLIGHT_H

