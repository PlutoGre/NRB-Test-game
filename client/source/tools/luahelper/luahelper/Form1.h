#pragma once
#include "pkgParser.h"

namespace luahelper {
	
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	pkgParser parser;
	void MarshalString ( System::String^ str, std::string& text ) {

		char* p = (char*)(int)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str);
		text = p;
		System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)p);
	}


	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ComboBox^  classCombo;


	protected: 

	protected: 



	private: System::Windows::Forms::ListBox^  txtList;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;







	private: System::Windows::Forms::TextBox^  classDetail;
	private: System::Windows::Forms::ContextMenuStrip^  contextMenuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  copyToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  copyWholelineToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  viewClassToolStripMenuItem;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TabPage^  tabPage4;

	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::CheckBox^  checkProto;


	private: System::Windows::Forms::CheckBox^  checkPKG;


	private: System::Windows::Forms::FolderBrowserDialog^  folderBrowserDialog1;
	private: System::Windows::Forms::ListBox^  fileList;

	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TabPage^  tabPage5;
	private: System::Windows::Forms::ListBox^  listHistory;
	private: System::ComponentModel::IContainer^  components;





	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->classCombo = (gcnew System::Windows::Forms::ComboBox());
			this->txtList = (gcnew System::Windows::Forms::ListBox());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->copyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->copyWholelineToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->viewClassToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->classDetail = (gcnew System::Windows::Forms::TextBox());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->listHistory = (gcnew System::Windows::Forms::ListBox());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->fileList = (gcnew System::Windows::Forms::ListBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->checkProto = (gcnew System::Windows::Forms::CheckBox());
			this->checkPKG = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->contextMenuStrip1->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->tabPage5->SuspendLayout();
			this->tabPage4->SuspendLayout();
			this->panel1->SuspendLayout();
			this->panel2->SuspendLayout();
			this->tabPage3->SuspendLayout();
			this->SuspendLayout();
			// 
			// classCombo
			// 
			this->classCombo->AutoCompleteMode = System::Windows::Forms::AutoCompleteMode::Suggest;
			this->classCombo->Dock = System::Windows::Forms::DockStyle::Fill;
			this->classCombo->Location = System::Drawing::Point(3, 3);
			this->classCombo->Name = L"classCombo";
			this->classCombo->Size = System::Drawing::Size(355, 20);
			this->classCombo->TabIndex = 0;
			this->classCombo->TextChanged += gcnew System::EventHandler(this, &Form1::classCombo_TextChanged);
			// 
			// txtList
			// 
			this->txtList->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->txtList->ContextMenuStrip = this->contextMenuStrip1;
			this->txtList->FormattingEnabled = true;
			this->txtList->ItemHeight = 12;
			this->txtList->Location = System::Drawing::Point(0, 30);
			this->txtList->Margin = System::Windows::Forms::Padding(3, 150, 3, 3);
			this->txtList->Name = L"txtList";
			this->txtList->Size = System::Drawing::Size(361, 316);
			this->txtList->TabIndex = 4;
			this->txtList->DoubleClick += gcnew System::EventHandler(this, &Form1::txtList_DoubleClick);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->copyToolStripMenuItem, 
				this->copyWholelineToolStripMenuItem, this->viewClassToolStripMenuItem});
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(242, 70);
			// 
			// copyToolStripMenuItem
			// 
			this->copyToolStripMenuItem->Name = L"copyToolStripMenuItem";
			this->copyToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::C));
			this->copyToolStripMenuItem->Size = System::Drawing::Size(241, 22);
			this->copyToolStripMenuItem->Text = L"copy keywrod";
			this->copyToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::copyToolStripMenuItem_Click);
			// 
			// copyWholelineToolStripMenuItem
			// 
			this->copyWholelineToolStripMenuItem->Name = L"copyWholelineToolStripMenuItem";
			this->copyWholelineToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift) 
				| System::Windows::Forms::Keys::C));
			this->copyWholelineToolStripMenuItem->Size = System::Drawing::Size(241, 22);
			this->copyWholelineToolStripMenuItem->Text = L"copy wholeline";
			this->copyWholelineToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::copyWholelineToolStripMenuItem_Click);
			// 
			// viewClassToolStripMenuItem
			// 
			this->viewClassToolStripMenuItem->Name = L"viewClassToolStripMenuItem";
			this->viewClassToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::I));
			this->viewClassToolStripMenuItem->Size = System::Drawing::Size(241, 22);
			this->viewClassToolStripMenuItem->Text = L"view class";
			this->viewClassToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::viewClassToolStripMenuItem_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage5);
			this->tabControl1->Controls->Add(this->tabPage4);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(369, 394);
			this->tabControl1->TabIndex = 5;
			// 
			// tabPage1
			// 
			this->tabPage1->AutoScroll = true;
			this->tabPage1->Controls->Add(this->classCombo);
			this->tabPage1->Controls->Add(this->txtList);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(361, 368);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"filter";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this->tabPage2->AutoScroll = true;
			this->tabPage2->Controls->Add(this->classDetail);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(361, 368);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"detail";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// classDetail
			// 
			this->classDetail->Dock = System::Windows::Forms::DockStyle::Fill;
			this->classDetail->Location = System::Drawing::Point(3, 3);
			this->classDetail->MaxLength = 32767000;
			this->classDetail->Multiline = true;
			this->classDetail->Name = L"classDetail";
			this->classDetail->ReadOnly = true;
			this->classDetail->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->classDetail->Size = System::Drawing::Size(355, 362);
			this->classDetail->TabIndex = 0;
			this->classDetail->WordWrap = false;
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->listHistory);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Size = System::Drawing::Size(361, 368);
			this->tabPage5->TabIndex = 4;
			this->tabPage5->Text = L"history";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// listHistory
			// 
			this->listHistory->ContextMenuStrip = this->contextMenuStrip1;
			this->listHistory->Dock = System::Windows::Forms::DockStyle::Fill;
			this->listHistory->FormattingEnabled = true;
			this->listHistory->ItemHeight = 12;
			this->listHistory->Location = System::Drawing::Point(0, 0);
			this->listHistory->Name = L"listHistory";
			this->listHistory->Size = System::Drawing::Size(361, 368);
			this->listHistory->TabIndex = 0;
			// 
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->fileList);
			this->tabPage4->Controls->Add(this->panel1);
			this->tabPage4->Controls->Add(this->panel2);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(361, 368);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = L"setting";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// fileList
			// 
			this->fileList->Dock = System::Windows::Forms::DockStyle::Fill;
			this->fileList->FormattingEnabled = true;
			this->fileList->ItemHeight = 12;
			this->fileList->Location = System::Drawing::Point(0, 29);
			this->fileList->Name = L"fileList";
			this->fileList->Size = System::Drawing::Size(361, 316);
			this->fileList->TabIndex = 15;
			this->fileList->DoubleClick += gcnew System::EventHandler(this, &Form1::fileList_DoubleClick);
			// 
			// panel1
			// 
			this->panel1->Controls->Add(this->label1);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(361, 29);
			this->panel1->TabIndex = 14;
			// 
			// label1
			// 
			this->label1->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->label1->Location = System::Drawing::Point(0, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(361, 20);
			this->label1->TabIndex = 14;
			this->label1->Text = L" Set the folders that contain pkg file or proto file.";
			// 
			// panel2
			// 
			this->panel2->Controls->Add(this->button1);
			this->panel2->Controls->Add(this->button2);
			this->panel2->Controls->Add(this->checkProto);
			this->panel2->Controls->Add(this->checkPKG);
			this->panel2->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->panel2->Location = System::Drawing::Point(0, 345);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(361, 23);
			this->panel2->TabIndex = 7;
			// 
			// button1
			// 
			this->button1->Dock = System::Windows::Forms::DockStyle::Right;
			this->button1->Location = System::Drawing::Point(211, 0);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 11;
			this->button1->Text = L"add(+)";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Dock = System::Windows::Forms::DockStyle::Right;
			this->button2->Location = System::Drawing::Point(286, 0);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 10;
			this->button2->Text = L"delete(-)";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// checkProto
			// 
			this->checkProto->AutoSize = true;
			this->checkProto->Checked = true;
			this->checkProto->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkProto->Location = System::Drawing::Point(74, 3);
			this->checkProto->Name = L"checkProto";
			this->checkProto->Size = System::Drawing::Size(60, 16);
			this->checkProto->TabIndex = 8;
			this->checkProto->Text = L".proto";
			this->checkProto->UseVisualStyleBackColor = true;
			this->checkProto->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkProto_CheckedChanged);
			// 
			// checkPKG
			// 
			this->checkPKG->AutoSize = true;
			this->checkPKG->Checked = true;
			this->checkPKG->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkPKG->Location = System::Drawing::Point(8, 3);
			this->checkPKG->Name = L"checkPKG";
			this->checkPKG->Size = System::Drawing::Size(48, 16);
			this->checkPKG->TabIndex = 7;
			this->checkPKG->Text = L".pkg";
			this->checkPKG->UseVisualStyleBackColor = true;
			this->checkPKG->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkPKG_CheckedChanged);
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->textBox1);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(361, 368);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = L"about";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// textBox1
			// 
			this->textBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->textBox1->Location = System::Drawing::Point(3, 3);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(355, 362);
			this->textBox1->TabIndex = 0;
			this->textBox1->Text = resources->GetString(L"textBox1.Text");
			// 
			// Form1
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(369, 394);
			this->Controls->Add(this->tabControl1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MaximizeBox = false;
			this->Name = L"Form1";
			this->Opacity = 0.85;
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Show;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"LuaHelper";
			this->TopMost = true;
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->contextMenuStrip1->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->tabPage5->ResumeLayout(false);
			this->tabPage4->ResumeLayout(false);
			this->panel1->ResumeLayout(false);
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: void caculate()
			 {
				 parser.clear();
				 std::list<std::string> flags;
				 flags.push_back(".\\");
				 flags.push_back("..\\tolua++");
				 flags.push_back("..\\Game_tolua++");
				 int count = fileList->Items->Count;
				 for(int i=0;i<count;++i)
				 {
					 std::string classString;
					 MarshalString((System::String^)fileList->Items[i],classString);
					 flags.push_back(classString);
				 }
				 std::list<std::string>::iterator fileit = flags.begin();
				 for(;fileit!=flags.end();++fileit)
				 {
					 String ^ str = gcnew String(fileit->c_str());
					 try
					 {
						 System::IO::DirectoryInfo^ di = gcnew System::IO::DirectoryInfo( str );
						 // Determine whether the directory exists. 
						 if ( di->Exists )
						 {
							 if(checkProto->Checked)
							 {
								 array<FileInfo^>^ fileinfo = di->GetFiles("*.proto");

								 for(int i=0;i<fileinfo->Length;++i)
								 {
									 std::string filename;
									 MarshalString(fileinfo[i]->FullName,filename);
									 //parser.openfile("E:\\Projects\\main\\client\\tools\\Game_tolua++\\DataTableManager.pkg");
									 parser.openfile(filename);			 
								 }
							 }
							 if(checkPKG->Checked)
							 {
								 array<FileInfo^>^ fileinfo = di->GetFiles("*.pkg");

								 for(int i=0;i<fileinfo->Length;++i)
								 {
									 std::string filename;
									 MarshalString(fileinfo[i]->FullName,filename);
									 //parser.openfile("E:\\Projects\\main\\client\\tools\\Game_tolua++\\DataTableManager.pkg");
									 parser.openfile(filename);			 
								 }
							 }

						 }
					 }
					 catch (...)
					 {
					 }
				 }

				 parser.createData();
				 std::list<pkgReturn>& ret = parser.search("");
				 std::list<pkgReturn>::const_iterator it = ret.begin();
				 for(;it!=ret.end();++it)
				 {
					 if(it->indexString!="")
					 {
						 String ^ str = gcnew String(it->indexString.c_str());
						 txtList->Items->Add(str);
					 }

				 }
			 }

private: System::Void saveListToFile() {

			 FILE * fp = fopen("config.txt","w+");

			 if(fp)
			 {
				 if(!checkPKG->Checked)
					 fwrite("no_pkg\n", 1, 7, fp);
				 if(!checkProto->Checked)
					 fwrite("no_pb\n", 1, 6, fp);
				 int count = fileList->Items->Count;
				 for(int i=0;i<count;++i)
				 {
					 std::string classString;
					 MarshalString((System::String^)fileList->Items[i],classString);
					 classString.append("\n");
					 fwrite(classString.c_str(), 1, classString.length(), fp);

				 }
				 fclose(fp);
			 }
		 }
private: System::Void loadFileToList() {

			 checkPKG->Checked = true;
			 checkProto->Checked = true;

			 FILE * fp = fopen("config.txt","r+");

			 if(fp)
			 {
				 std::string line;
				 line.reserve(256);

				 while(!feof(fp))
				 {
					 char a = fgetc(fp);
					 if(a!='\n' && a!='\r')
					 {
						 line.push_back(a);
					 }
					 else if(!line.empty())
					 {
						 if(line == "no_pkg")
							 checkPKG->Checked = false;
						 else if(line == "no_pb")
							 checkProto->Checked = false;
						 else
						 {
							 String ^ str = gcnew String(line.c_str());
							 fileList->Items->Add(str);
						 }
						 line.clear();
					 }
				 }
				 fclose(fp);
			 }
		 }

private:void addToHistory(System::Object^ item)
		{
			if(listHistory->Items->Contains(item))
				listHistory->Items->Remove(item);
			listHistory->Items->Insert(0,item);
		}
//------------------------------------------------------------------------------

private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 loadFileToList();
				 caculate();
				 if(fileList->Items->Count==0 && txtList->Items->Count==0)
				 {
					 //MessageBox::Show("Choose a folders contain target .pkg files or .proto files!");
					 tabControl1->SelectTab(tabPage4);
				 }
				 this->SetDesktopLocation(System::Windows::Forms::Screen::PrimaryScreen->Bounds.Width - this->Width-100,100);
			 }
private: void showClass(std::string searchStr)
		 {
			 std::list<pkgReturn>& ret = parser.getLastSearch();

			 std::list<pkgReturn>::const_iterator it = ret.begin();
			 for(;it!=ret.end();++it)
			 {
				 if(it->indexString == searchStr)
				 {
					 pkgClass* pc = it->classPointer;
					 String ^ str = gcnew String(pc->content.c_str());
					 classDetail->Text = str;
					 return;
				 }

			 }
		 }
private: System::Void classCombo_TextChanged(System::Object^  sender, System::EventArgs^  e) {
			 
			 std::string searchStr;
			 MarshalString(classCombo->Text,searchStr);
			 std::list<pkgReturn>& ret = parser.search(searchStr);
			 
			 
			 std::list<pkgReturn> results;
			 std::list<pkgReturn>::const_iterator it = ret.begin();
			 for(;it!=ret.end();++it)
			 {
				 if(it->indexString!="")
				 {
					 results.push_back(*it);
				 }
			 }

			 txtList->Items->Clear();
			 array<String^>^ addStrs = gcnew array<String^>(results.size());

			 it = results.begin();
			 int i=0;
			 for(;it!=results.end();++it)
			 {
				 if(it->indexString!="")
				 {
					 String ^ str = gcnew String(it->indexString.c_str());	
					 addStrs[i]=str;
					 i++;
				 }
			 }
			 txtList->Items->AddRange(addStrs);
		 }

private: System::Void txtList_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
			 if(txtList->SelectedIndex<0)return;
			 tabControl1->SelectTab(tabPage2->Name);
			 std::string searchStr;
			 MarshalString(txtList->SelectedItem->ToString(),searchStr);
			 showClass(searchStr);
			 addToHistory(txtList->SelectedItem);
		 }

private: System::Void copyToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 std::string searchStr = "";
			 if(tabControl1->SelectedTab == tabPage1)
			 {
				 if(txtList->SelectedIndex<0)return;
				 MarshalString(txtList->SelectedItem->ToString(),searchStr);
				 addToHistory(txtList->SelectedItem);
			 }
			 else
			 {
				 if(listHistory->SelectedIndex<0)return;
				 MarshalString(listHistory->SelectedItem->ToString(),searchStr);
			 }
			 if(searchStr=="")return;
			 std::string ret = parser.getKeyword(searchStr);
			 if(ret!="")
			 {
				 String ^ str = gcnew String(ret.c_str());
				 System::Windows::Forms::Clipboard::SetDataObject(str , true);
				 //Clipboard.SetDataObject(this.textBox1.Text , true);
			 }
		 }
private: System::Void copyWholelineToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 std::string searchStr = "";
			 if(tabControl1->SelectedTab == tabPage1)
			 {
				 if(txtList->SelectedIndex<0)return;
				 System::Windows::Forms::Clipboard::SetDataObject(txtList->SelectedItem , true);
				 addToHistory(txtList->SelectedItem);
			 }
			 else
			 {
				 if(listHistory->SelectedIndex<0)return;
				 System::Windows::Forms::Clipboard::SetDataObject(listHistory->SelectedItem , true);
			 }			 
			 
		 }
private: System::Void viewClassToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 std::string searchStr = "";
			 if(tabControl1->SelectedTab == tabPage1)
			 {
				 if(txtList->SelectedIndex<0)return;
				 MarshalString(txtList->SelectedItem->ToString(),searchStr);
				 addToHistory(txtList->SelectedItem);
			 }
			 else
			 {
				 if(listHistory->SelectedIndex<0)return;
				 MarshalString(listHistory->SelectedItem->ToString(),searchStr);
			 }
			 if(searchStr=="")return;
			 showClass(searchStr);
			 tabControl1->SelectTab(tabPage2->Name);
		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			 folderBrowserDialog1->ShowDialog();
			 if(System::String::IsNullOrEmpty(folderBrowserDialog1->SelectedPath))
				 return;

			 if(fileList->Items->Contains(folderBrowserDialog1->SelectedPath))
			 {
				MessageBox::Show(L"already exist!");
			 }
			 else
			 {
				 fileList->Items->Add(folderBrowserDialog1->SelectedPath);
				 saveListToFile();
				 caculate();
			 }
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			 fileList->Items->Remove(fileList->SelectedItem);
			 saveListToFile();
			 caculate();
		 }
private: System::Void fileList_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
			 folderBrowserDialog1->ShowDialog();
			 if(System::String::IsNullOrEmpty(folderBrowserDialog1->SelectedPath))
				 return;

			 if(fileList->Items->Contains(folderBrowserDialog1->SelectedPath))
			 {
				 MessageBox::Show(L"already exist!");
			 }
			 else
			 {
				 int pos = fileList->SelectedIndex;
				 fileList->Items->Remove(fileList->SelectedItem);
				 if(pos>=0)
					fileList->Items->Insert(pos,folderBrowserDialog1->SelectedPath);
				 else
					 fileList->Items->Add(folderBrowserDialog1->SelectedPath);
				 saveListToFile();
				 caculate();
			 }
		 }
private: System::Void checkPKG_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 saveListToFile();
			 caculate();
		 }
private: System::Void checkProto_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 saveListToFile();
			 caculate();
		 }
};
}

