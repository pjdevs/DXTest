#define STB_IMAGE_IMPLEMENTATION 
#define NOMINMAX

#include <iostream>
#include <string>
#include <entt/entity/registry.hpp>
#include "Graphics.hpp"
#include "RenderWindow.hpp"
#include "GameObject.hpp"


int CALLBACK AWinMain(_In_ HINSTANCE appInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPSTR cmdLine, _In_ int cmdCount)
{
	// Init window
	Graphics gfx;
	RenderWindow window(gfx, 800, 600);
	MSG msg = { 0 };
	Scene scene;

	GameObject cube(scene);
	cube.addComponent<MeshComponent>();

	GameObject camera(scene);
	camera.addComponent<CameraComponent>();
	camera.getComponent<CameraComponent>().main = true;

	// Main loop
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		// Render entities
		D3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.f, 0.f, window.getWidth(), window.getHeight());
		static float clearColor[] = { 0.5f, 0.5f, 0.8f };

		ID3D11RenderTargetView* renderTargets = window.getBackBufferRT();

		gfx.getDeviceContext()->OMSetRenderTargets(1, &renderTargets, window.getBackBufferDS());
		gfx.getDeviceContext()->RSSetViewports(1, &viewport);
		gfx.getDeviceContext()->ClearRenderTargetView(window.getBackBufferRT(), clearColor);
		gfx.getDeviceContext()->ClearDepthStencilView(window.getBackBufferDS(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		window.beginImGui();

		ImGui::Begin("Debug");
		ImGui::LabelText("I am debugging !", "Test");
		ImGui::End();

		window.endImGui();

		window.present();
	}

	return 0;
}