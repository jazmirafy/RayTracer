#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{

public:
	ExampleLayer()
		:m_Camera(45.0f, 0.1f, 100.0f) {
			{
				Sphere sphere;
				m_Scene.Spheres.push_back(sphere);
			}
			{
				Sphere sphere;
				m_Scene.Spheres.push_back(sphere);
			}

	}
	virtual void OnUpdate(float ts) override {
		
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);

		if (ImGui::Button("Render")) {

			Render();
		}
		ImGui::End();
		

		ImGui::Begin("Scene");
		ImGui::DragFloat3("Position", glm::value_ptr(m_Scene.Spheres[0].Position), 0.1f);
		ImGui::DragFloat("Radius", &m_Scene.Spheres[0].Radius, 0.1f);
		ImGui::ColorEdit3("Albedo", glm::value_ptr(m_Scene.Spheres[0].Albedo));


		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if(image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0,1), ImVec2(1,0));
		ImGui::End();
		ImGui::PopStyleVar();

		Render();

	}

	void Render() {
		
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);

		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		Timer timer;

		m_Renderer.Render(m_Scene, m_Camera);





		m_LastRenderTime = timer.ElapsedMillis();
	}

private:
	Camera m_Camera;
	Renderer m_Renderer;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Egypts Ray Tracer <3";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}