#include "Renderer.h"
#include "Walnut/Random.h"
#include <omp.h>

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4 color) {
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}
void Renderer::OnResize(uint32_t width, uint32_t height) {
	if (m_FinalImage) {

		//no resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height) {
			return;
		}
		m_FinalImage->Resize(width, height);
	}
	else {
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);

	}
	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

}
void Renderer::Render(const Scene& scene, const Camera& camera) {


	
	//render every pixel
	//keeping y as the outer loop so we traverse horizontally and are more cpu cache friendly
	#pragma omp parallel for num_threads(8)
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {
			Ray ray;
			ray.Origin = camera.GetPosition();

			ray.Direction = camera.GetRayDirections()[x + y * m_FinalImage->GetWidth()];


			glm::vec4 color = TraceRay(scene, ray);
			color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
	
		}
	}

	//upload data to gpu
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	if (scene.Spheres.size() == 0) {
		return glm::vec4(1, 1, 1, 1);
	}
	const Sphere& sphere = scene.Spheres[0];

	//(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 +az^2- r^2) = 0

	//a = ray origin
	//b = ray direction
	//r = radius of sphere/circle
	//t = how far along the ray the intersection occured

	glm::vec3 origin = ray.Origin - sphere.Position;
	//coefficient in the quadratic equation (coefficient of t^2)
	float a = glm::dot(ray.Direction, ray.Direction);
	//coefficient of t in the quadratic equation
	float b = 2.0f * glm::dot(origin, ray.Direction);
	//constant c in the quadratic equation
	float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

	//quadratic formula discriminant (to know if we have any t solutions):
	//b^2 - 4ac

	
	float discriminant = b * b - 4.0f * a * c;
	//if there are no intersection return background color
	if (discriminant < 0.0f) { 
		return glm::vec4(1, 1, 1, 1);
	}

	//(-b +- sqrt(discriminant)) /2a
	float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

	//glm::vec3 h0 = rayOrigin + rayDirection * t0;
	glm::vec3 hitPoint = origin + ray.Direction * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);
	
	glm::vec3 sphereColor = sphere.Albedo;
	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	float d = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle between vectors)
	sphereColor *= d;

	return glm::vec4(sphereColor, 1.0f);
}
