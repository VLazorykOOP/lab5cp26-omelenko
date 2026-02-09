#include "GForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^ args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	OP26Lab5Graphics::GForm form;
	Application::Run(% form);
	return 0;
}