#pragma once
#include <Application.h>

class VGame final : public Application
{
	public:
		VGame(const std::string& name);
		virtual ~VGame() = default;

		VGame(const VGame& other) = delete;
		VGame(VGame&& other) = delete;
		VGame& operator=(const VGame& other) = delete;
		VGame& operator=(VGame&& other) = delete;

		void Initialize() override;
		void PostInitialize() override;

		void Update() override;
		void FixedUpdate() override;

		void Prerender() override;
		bool PreSideCamRender() override;
		void Render() override;
		void Postrender() override;

	protected:

	private:
};