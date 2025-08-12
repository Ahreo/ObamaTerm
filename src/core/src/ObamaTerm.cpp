#include "ObamaTerm.h"

namespace OT {
    bool ObamaTerm::OnUserCreate() {
        // PortManager singleton
        manager = OT::PortManager::get_instance();

        // FontHandler object - font is adjustable!
        fontHandler = new FontHandler("assets/fonts/Tamzen6x12r.ttf", 12);

        // Init portname vector
        portList = manager->get_portname_vec();

        // Port List
        guiListPort = new olc::QuickGUI::ListBox(guiManager,
			portList, { 150.0f, 130.0f }, { 100.0f, 40.0f });

        // Baud list
        baudList.push_back("115200");
        guiListBaud = new olc::QuickGUI::ListBox(guiManager,
			baudList, { 150.0f, 180.0f }, { 100.0f, 40.0f });

        // Slider to be used as a scroll bar
        guiScrollBar = new olc::QuickGUI::Slider(guiManager, 
            {(float) ScreenWidth(), 0}, {(float) ScreenWidth(), (float) ScreenHeight()}, 
            0, 100, 0);

		// TextBox, allows basic text entry
        guiInitConfigButton = new olc::QuickGUI::Button(guiManager,
            "Ok", { 30.0f, 150.0f }, { 100.0f, 16.0f });
        guiCancelButton = new olc::QuickGUI::Button(guiManager,
            "Cancel", { 30.0f, 170.0f }, { 100.0f, 16.0f });
		return true;
    }

    bool ObamaTerm::OnUserUpdate(float fElapsedTime)  {
		// We must update the manager at some point each frame. Values of controls
		// are only valid AFTER this call to update()
		guiManager.Update(this);
		// Clear(olc::BLACK);

        if(guiInitConfigButton->bPressed) {
            std::vector<std::string> port_names = manager->get_portname_vec();
            serial = manager->get_serial_port(port_names[guiListPort->nSelectedItem]);
            m_start_polling = true;
        }

        if(m_start_polling) {
            std::vector<uint8_t> buf = serial->receive_data();
            std::string str(buf.begin(), buf.end());
            if(!buf.empty()) {
                fontHandler->RenderText(this, str, olc::WHITE);
            }
        }


        guiManager.DrawDecal(this);
		return true;
	}
}
