#pragma once
#include <Vec3.h>
#include <optional>

class Material
{
private:
	Vec3 ambient;
	Vec3 diffuse;
	Vec3 specular;

	float exponent;
	float local;
	std::optional<float> IOR;
	bool m_isShadowCaster;

public:
	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float exponent)
		: Material(ambient, diffuse, specular, exponent, 1.0)
	{ }

	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float exponent, float local)
		: Material(ambient, diffuse, specular, exponent, local, {})
	{ }
	
	/// <param name="ambient">ambient object color</param>
	/// <param name="diffuse">diffuse object color</param>
	/// <param name="specular">specular object color</param>
	/// <param name="exponent">exponent specular exponent</param>
	/// <param name="local">local reflectivity, percentage of local illumination, 1 - local is the percentage of light
	///						that gets reflected or reflected and refracted according to Schlick's approximation of the
	///						Fresnel equations</param>
	/// <param name="IOR">Index Of Refraction</param>
	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float exponent, float local, const std::optional<float>& IOR)
		: Material(ambient, diffuse, specular, exponent, local, IOR, true)
	{ }

	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float exponent, float local, const std::optional<float>& IOR, bool isShadowCaster)
		: ambient(ambient), diffuse(diffuse), specular(specular), exponent(exponent), local(local), IOR(IOR), m_isShadowCaster(isShadowCaster)
	{ }
	
	Vec3 getAmbient() const;

	Vec3 getDiffuse() const;

	Vec3 getSpecular() const;

	float getExp() const;

	bool isShadowCaster() const;

	bool reflects() const;

	bool refracts() const;

	std::optional<float> getIndexOfRefraction() const;

	float getLocalRefectivity() const;

};

