#pragma once
#include <Vec3.h>
#include <optional>

#include "Texture.h"

class Material
{
private:
	Vec3 ambient;
	Vec3 diffuse;
	Vec3 specular;

	double exponent;
	double local;
	std::optional<float> IOR;
	bool m_isShadowCaster;
	std::optional<Texture> texture;

public:
	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, double exponent)
		: Material(ambient, diffuse, specular, exponent, 1.0)
	{ }

	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, double exponent, double local)
		: Material(ambient, diffuse, specular, exponent, local, std::nullopt)
	{ }
	
	/// <param name="ambient">ambient object color</param>
	/// <param name="diffuse">diffuse object color</param>
	/// <param name="specular">specular object color</param>
	/// <param name="exponent">exponent specular exponent</param>
	/// <param name="local">local reflectivity, percentage of local illumination, 1 - local is the percentage of light
	///						that gets reflected or reflected and refracted according to Schlick's approximation of the
	///						Fresnel equations</param>
	/// <param name="IOR">Index Of Refraction</param>
	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, double exponent, double local, const std::optional<float>& IOR)
		: Material(ambient, diffuse, specular, exponent, local, IOR, std::nullopt)
	{ }

	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, double exponent, double local, const std::optional<float>& IOR, const std::optional<Texture>& texture)
		: Material(ambient, diffuse, specular, exponent, local, IOR, texture, true)
	{
	}

	Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, double exponent, double local, const std::optional<float>& IOR, const std::optional<Texture>& texture, bool isShadowCaster)
		: ambient(ambient), diffuse(diffuse), specular(specular), exponent(exponent), local(local), IOR(IOR), m_isShadowCaster(isShadowCaster), texture(texture)
	{ }
	
	Vec3 getAmbient() const;

	Vec3 getDiffuse() const;

	Vec3 getSpecular() const;

	double getExp() const;

	bool isShadowCaster() const;

	bool reflects() const;

	bool refracts() const;

	bool hasTexture() const;

	std::optional<float> getIndexOfRefraction() const;

	double getLocalRefectivity() const;

	double getReflectivity(float cosI) const;

	std::optional<Texture> getTexture() const;
};

