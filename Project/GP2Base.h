#pragma once

#include "GP2Window.h"
#include "GP2Device.h"
#include "GP2Model.h"
#include "GP2GameObject.h"
#include "GP2Renderer.h"

//std
#include <memory>
#include <vector>

namespace GP2 
{
	class GP2Base
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		
		GP2Base();
		~GP2Base();
		
		GP2Base(const GP2Base&) = delete;
		GP2Base& operator=(const GP2Base&) = delete;
		
		void Run();

	private:
		void LoadGameObjects();
		
		GP2Window m_GP2Window{WIDTH, HEIGHT, "I Vulkan't Anymore"};
		GP2Device m_GP2Device{ m_GP2Window };
		GP2Renderer m_GP2Renderer{ m_GP2Window, m_GP2Device };
		std::vector<GP2GameObject> m_GameObjects;
	};
}

