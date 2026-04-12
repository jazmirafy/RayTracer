#include "Renderer.h"
#include "Walnut/Random.h"

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
void Renderer::Render() {
	//render every pixel
	//keeping y as the outer loop so we traverse horizontally and are more cpu cache friendly
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {

		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++) {

			glm::vec2 coord = { (float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight() };
			coord = coord * 2.0f - 1.0f; //remap coordinate from 0->1 to -1->1
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord);
	
		}
	}

	//upload data to gpu
	m_FinalImage->SetData(m_ImageData);
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t r = (uint8_t)(coord.x * 255.0f);
	uint8_t g = (uint8_t)(coord.y * 255.0f);

	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);

	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
	float radius = 0.5f;
	rayDirection = glm::normalize(rayDirection);
	
	//(bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 +az^2- r^2) = 0

	//a = ray origin
	//b = ray direction
	//r = radius of sphere/circle
	//t = how far along the ray the intersection occured

	//coefficient in the quadratic equation (coefficient of t^2)
	float a = glm::dot(rayDirection, rayDirection);
	//coefficient of t in the quadratic equation
	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
	//constant c in the quadratic equation
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	//quadratic formula discriminant (to know if we have any t solutions):
	//b^2 - 4ac
	float discriminant = b * b - 4.0f * a * c;

	//if we have at least one t solution return sphere color
	if (discriminant >= 0.0f) {
		return 0xffff00ff;
	}
	//otherwise return background color
	return 0xff000000;
}
