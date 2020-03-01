#include "ComponentLight.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"

ComponentLight::ComponentLight() : Component(nullptr, ComponentType::LIGHT)
{
}

ComponentLight::ComponentLight(GameObject * owner) : Component(owner, ComponentType::LIGHT)
{

}

void ComponentLight::Delete()
{
	App->lights->RemoveComponentLight(this);
}

void ComponentLight::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddColor(float4(light_color[0], light_color[1], light_color[2], 1.f), "LightColor");
	config.AddFloat(light_intensity, "Intensity");
	config.AddInt(static_cast<int>(light_type), "LightType");

	config.AddFloat(point_light_parameters.range, "PointLightRange");
	config.AddFloat(point_light_parameters.constant, "PointLightConstant");
	config.AddFloat(point_light_parameters.linear, "PointLightLinear");
	config.AddFloat(point_light_parameters.quadratic, "PointLightQuadratic");

	config.AddFloat(spot_light_parameters.range, "SpotLightRange");
	config.AddFloat(spot_light_parameters.constant, "SpotLightConstant");
	config.AddFloat(spot_light_parameters.linear, "SpotLightLinear");
	config.AddFloat(spot_light_parameters.quadratic, "SpotLightQuadratic");
	config.AddFloat(spot_light_parameters.spot_angle, "SpotLightSpotAngle");
	config.AddFloat(spot_light_parameters.cutoff, "SpotLightCutoff");
	config.AddFloat(spot_light_parameters.edge_softness, "SpotLightSoftness");
	config.AddFloat(spot_light_parameters.outer_cutoff, "SpotLightOuterCutoff");
}

void ComponentLight::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	float4 color;
	config.GetColor("LightColor", color, float4(0.f, 0.f, 0.f, 1.f));
	light_color[0] = color.x;
	light_color[1] = color.y;
	light_color[2] = color.z;

	light_intensity = config.GetFloat("Intensity", 1.f);
	light_type = static_cast<LightType>(config.GetInt("LightType", static_cast<int>(LightType::POINT_LIGHT)));

	point_light_parameters.range = config.GetFloat("PointLightRange", 16.f);
	point_light_parameters.constant = config.GetFloat("PointLightConstant", 1.f);
	point_light_parameters.linear = config.GetFloat("PointLightLinear", 0.045f);
	point_light_parameters.quadratic = config.GetFloat("PointLightQuadratic", 0.0075f);

	spot_light_parameters.range = config.GetFloat("SpotLightRange", 10.f);
	spot_light_parameters.constant = config.GetFloat("SpotLightConstant", 1.f);
	spot_light_parameters.linear = config.GetFloat("SpotLightLinear", 0.045f);
	spot_light_parameters.quadratic = config.GetFloat("SpotLightQuadratic", 0.0075f);
	spot_light_parameters.spot_angle = config.GetFloat("SpotLightSpotAngle", 30.f);
	spot_light_parameters.cutoff = config.GetFloat("SpotLightCutoff", cos(DegToRad(15.f)));
	spot_light_parameters.edge_softness = config.GetFloat("SpotLightSoftness", 0.1f);
	spot_light_parameters.outer_cutoff = config.GetFloat("SpotLightOuterCutoff", cos(DegToRad(16.5f)));
}
