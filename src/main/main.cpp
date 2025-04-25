#include <cstdio>

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_QUICKGUI
#include "olcPGEX_QuickGUI.h"

extern "C" {
    #include "libserialport.h"
}

bool getPorts(std::vector<std::string>& ports_list) {
    // Search for ports
    struct sp_port **port_list_raw;

    printf("Getting port list.\n");

    /* Call sp_list_ports() to get the ports. The port_list_raw
    * pointer will be updated to refer to the array created. */
    enum sp_return result = sp_list_ports(&port_list_raw);

    if (result != SP_OK) {
        printf("sp_list_ports() failed!\n");
        return false;
    }

    /* Iterate through the ports. When port_list_raw[i] is NULL
        * this indicates the end of the list. */
    for (int i = 0; port_list_raw[i] != NULL; i++) {
        struct sp_port *port = port_list_raw[i];

        /* Get the name of the port. */
        std::string port_name = sp_get_port_name(port);
        ports_list.push_back(port_name);
        printf("got port: %s \n", port_name.c_str());
    }

    printf("Freeing port list.\n");
    /* Free the array created by sp_list_ports(). */
    sp_free_port_list(port_list_raw);
    return true;
}

class ObamaTerm : public olc::PixelGameEngine {
public:
    ObamaTerm() {
        sAppName = "Obama Term";
    }

public:
	bool OnUserCreate() override {
        if(!getPorts(portList)) {
            return false;
        }

        // Port List
        guiListPort = new olc::QuickGUI::ListBox(guiManager,
			portList, { 150.0f, 180.0f }, { 100.0f, 54.0f });

        // Baud list
        baudList.push_back("9600");
        baudList.push_back("115200");
        guiListBaud = new olc::QuickGUI::ListBox(guiManager,
			baudList, { 150.0f, 180.0f }, { 100.0f, 54.0f });

		// TextBox, allows basic text entry
		guiTextBox1 = new olc::QuickGUI::TextBox(guiManager,
			"Enter COM port", { 150.0f, 140.0f }, { 100.0f, 16.0f });
		guiTextBox2 = new olc::QuickGUI::TextBox(guiManager,
			"Enter Baud rate", { 150.0f, 160.0f }, { 100.0f, 16.0f });
        guiInitConfigButton = new olc::QuickGUI::Button(guiManager,
            "Ok", { 30.0f, 150.0f }, { 100.0f, 16.0f });
        guiCancelButton = new olc::QuickGUI::Button(guiManager,
            "Cancel", { 30.0f, 170.0f }, { 100.0f, 16.0f });
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		// We must update the manager at some point each frame. Values of controls
		// are only valid AFTER this call to update()
		guiManager.Update(this);

        if(guiInitConfigButton->bPressed) {
            printf("text input: %s", guiTextBox1->sText.c_str());
        }
		Clear(olc::BLACK);

        guiManager.Draw(this);
		return true;
	}
    
protected:
    olc::QuickGUI::Manager guiManager;

    olc::QuickGUI::TextBox* guiTextBox1 = nullptr;
    olc::QuickGUI::TextBox* guiTextBox2 = nullptr;
    olc::QuickGUI::Button* guiInitConfigButton = nullptr;
    olc::QuickGUI::Button* guiCancelButton = nullptr;

    olc::QuickGUI::ListBox* guiListPort = nullptr;
    std::vector<std::string> portList;

    olc::QuickGUI::ListBox* guiListBaud = nullptr;
    std::vector<std::string> baudList;

private:

};

int main() {
    ObamaTerm terminal;
	if (terminal.Construct(256, 240, 4, 4))
		terminal.Start();
	return 0;

    struct sp_port *rx_port;
    sp_get_port_by_name("COM9", &rx_port);

    printf("Opening port.\n");
    sp_open(rx_port, SP_MODE_READ_WRITE);

    printf("Setting port to 9600 8N1, no flow control.\n");
    sp_set_baudrate(rx_port, 115200);
    sp_set_bits(rx_port, 8);
    sp_set_parity(rx_port, SP_PARITY_NONE);
    sp_set_stopbits(rx_port, 1);
    sp_set_flowcontrol(rx_port, SP_FLOWCONTROL_NONE);

	/* Now send some data on each port and receive it back. */
    /* Get the ports to send and receive on. */
    // struct sp_port *rx_port = port;

    /* The data we will send. */
    constexpr int size = 64;
    while(1) {
        /* We'll allow a 1 second timeout for send and receive. */
        unsigned int timeout = 10;

        /* On success, sp_blocking_write() and sp_blocking_read()
            * return the number of bytes sent/received before the
            * timeout expired. We'll store that result here. */
        int result;

        /* Allocate a buffer to receive data. */
        char buf[size];

        /* Try to receive the data on the other port. */
        result = sp_blocking_read(rx_port, buf, size, timeout);

        /* Check whether we received the number of bytes we wanted. */
        for(int i=0;i<result;++i) {
            printf("%c", buf[i]);
        }
    }

	/* Close ports and free resources. */
    sp_close(rx_port);
    sp_free_port(rx_port);
    return 0;
}