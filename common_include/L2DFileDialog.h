#pragma once
/*
	select C++17. <<< !!!!!!!
	The available options are (under project > Properties > C/C++ > Language > C++ Language Standard)
*/

#include <imgui.h>
#include <imgui_internal.h>
#include <filesystem>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>	//

using namespace std::chrono_literals;

namespace FileDialog {

	enum class FileDialogSortOrder {
		Up,
		Down,
		None
	};

	// return true on 'ok', else 
	// return false on 'cancel' and empties buffer,
	bool ShowFileDialog(char* buffer, unsigned int bufferSize, std::string currentPath, std::string& returnedPath, bool &isFileDialogActivated) {
		isFileDialogActivated = true;

		static bool setHomeDir = false;
		returnedPath = currentPath;
		static std::string fileDialogCurrentPath = currentPath;
		static std::string fileDialogCurrentFile = "";
		static std::string fileDialogCurrentFolder = "";
		static int fileDialogFileSelectIndex = 0;
		static int fileDialogFolderSelectIndex = 0;
		static FileDialogSortOrder fileNameSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder sizeSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder dateSortOrder = FileDialogSortOrder::None;
		static FileDialogSortOrder typeSortOrder = FileDialogSortOrder::Up;	// default sort order on begin, was: None

		if (setHomeDir) {
			fileDialogCurrentPath = returnedPath = std::filesystem::current_path().string();
			setHomeDir = false;
		}

		std::vector<std::filesystem::directory_entry> files;
		std::vector<std::filesystem::directory_entry> folders;
		try {
			for (auto& p : std::filesystem::directory_iterator(fileDialogCurrentPath)) {
				if (p.is_directory()) {
					folders.push_back(p);
				}
				else {
					files.push_back(p);
				}
			}
		}
		catch (...) {}

		ImGui::Text("%s", fileDialogCurrentPath.c_str());

		ImGui::BeginChild("Directories##1", ImVec2(200, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
			if (ImGui::IsMouseDoubleClicked(0)) {
				fileDialogCurrentPath = std::filesystem::path(fileDialogCurrentPath).parent_path().string();
			}
		}
		for (int i = 0; i < (int)folders.size(); ++i) {
			if (ImGui::Selectable(folders[i].path().stem().string().c_str(), i == fileDialogFolderSelectIndex, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
				fileDialogCurrentFile = "";
				if (ImGui::IsMouseDoubleClicked(0)) {
					fileDialogCurrentPath = folders[i].path().string();
					fileDialogFolderSelectIndex = 0;
					fileDialogFileSelectIndex = 0;
					ImGui::SetScrollHereY(0.0f);
					fileDialogCurrentFolder = "";
				}
				else {
					fileDialogFolderSelectIndex = i;
					fileDialogCurrentFolder = folders[i].path().stem().string();
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Files##1", ImVec2(516, 300), true, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Columns(4);
		static float initialSpacingColumn0 = 230.0f;
		if (initialSpacingColumn0 > 0) {
			ImGui::SetColumnWidth(0, initialSpacingColumn0);
			initialSpacingColumn0 = 0.0f;
		}
		static float initialSpacingColumn1 = 80.0f;
		if (initialSpacingColumn1 > 0) {
			ImGui::SetColumnWidth(1, initialSpacingColumn1);
			initialSpacingColumn1 = 0.0f;
		}
		static float initialSpacingColumn2 = 80.0f;
		if (initialSpacingColumn2 > 0) {
			ImGui::SetColumnWidth(2, initialSpacingColumn2);
			initialSpacingColumn2 = 0.0f;
		}
		if (ImGui::Selectable("File")) {
			sizeSortOrder = FileDialogSortOrder::None;
			dateSortOrder = FileDialogSortOrder::None;
			typeSortOrder = FileDialogSortOrder::None;
			fileNameSortOrder = (fileNameSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Size")) {
			fileNameSortOrder = FileDialogSortOrder::None;
			dateSortOrder = FileDialogSortOrder::None;
			typeSortOrder = FileDialogSortOrder::None;
			sizeSortOrder = (sizeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Type")) {
			fileNameSortOrder = FileDialogSortOrder::None;
			dateSortOrder = FileDialogSortOrder::None;
			sizeSortOrder = FileDialogSortOrder::None;
			typeSortOrder = (typeSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
		}
		ImGui::NextColumn();
		if (ImGui::Selectable("Date")) {
			fileNameSortOrder = FileDialogSortOrder::None;
			sizeSortOrder = FileDialogSortOrder::None;
			typeSortOrder = FileDialogSortOrder::None;
			dateSortOrder = (dateSortOrder == FileDialogSortOrder::Down ? FileDialogSortOrder::Up : FileDialogSortOrder::Down);
		}
		ImGui::NextColumn();
		ImGui::Separator();

		// Sort files
		if (fileNameSortOrder != FileDialogSortOrder::None) {
			std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
				if (fileNameSortOrder == FileDialogSortOrder::Down) {
					return a.path().filename().string() > b.path().filename().string();
				}
				else {
					return a.path().filename().string() < b.path().filename().string();
				}
			});
		}
		else if (sizeSortOrder != FileDialogSortOrder::None) {
			std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
				if (sizeSortOrder == FileDialogSortOrder::Down) {
					return a.file_size() > b.file_size();
				}
				else {
					return a.file_size() < b.file_size();
				}
			});
		}
		else if (typeSortOrder != FileDialogSortOrder::None) {
			std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
				if (typeSortOrder == FileDialogSortOrder::Down) {
					return a.path().extension().string() > b.path().extension().string();
				}
				else {
					return a.path().extension().string() < b.path().extension().string();
				}
			});
		}
		else if (dateSortOrder != FileDialogSortOrder::None) {
			std::sort(files.begin(), files.end(), [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) {
				if (dateSortOrder == FileDialogSortOrder::Down) {
					return a.last_write_time() > b.last_write_time();
				}
				else {
					return a.last_write_time() < b.last_write_time();
				}
			});
		}

		for (int i = 0; i < (int)files.size(); ++i) {
			if (ImGui::Selectable(files[i].path().filename().string().c_str(), i == fileDialogFileSelectIndex, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
				fileDialogFileSelectIndex = i;
				fileDialogCurrentFile = files[i].path().filename().string();
				fileDialogCurrentFolder = "";
			}
			ImGui::NextColumn();
			ImGui::TextUnformatted(std::to_string(files[i].file_size()).c_str());
			ImGui::NextColumn();
			ImGui::TextUnformatted(files[i].path().extension().string().c_str());
			ImGui::NextColumn();
			auto ftime = files[i].last_write_time();
			auto st = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
			std::time_t tt = std::chrono::system_clock::to_time_t(st);
			std::tm* mt = std::localtime(&tt);
			std::stringstream ss;
			ss << std::put_time(mt, "%F %R");
			ImGui::TextUnformatted(ss.str().c_str());
			ImGui::NextColumn();
		}
		ImGui::EndChild();

		std::string selectedFilePath = fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + (fileDialogCurrentFolder.size() > 0 ? fileDialogCurrentFolder : fileDialogCurrentFile);
		char* buf = &selectedFilePath[0];
		ImGui::PushItemWidth(724);
		ImGui::InputText("", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6);

		//			ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 120);	// 

		if (ImGui::Button("Cancel")) {
			buffer[0] = '\0';	// clear buffer on cancel
			fileDialogFileSelectIndex = 0;
			fileDialogFolderSelectIndex = 0;
			fileDialogCurrentFile = "";
			ImGui::CloseCurrentPopup();
			isFileDialogActivated = false;
			return false;				// cancel
		}
		ImGui::SameLine();
		if (ImGui::Button("Choose")) {
			strncpy(buffer, (fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + fileDialogCurrentFolder + fileDialogCurrentFile).c_str(), bufferSize);
			returnedPath =  (fileDialogCurrentPath + (fileDialogCurrentPath.back() == '\\' ? "" : "\\") + fileDialogCurrentFolder);
			fileDialogFileSelectIndex = 0;
			fileDialogFolderSelectIndex = 0;
			fileDialogCurrentFile = "";
			ImGui::CloseCurrentPopup();
			if (strlen(buffer) == 0) return false;	// cancel

			char *bif = strrchr(buffer, '.');
			if (bif == nullptr) {	// no extension
				buffer[0] = '\0';
				isFileDialogActivated = false;
				return false;	// cancel
			}
			else {	// extension exists

			   // ".bmp,.png,.jpg,.jpeg,.gif,.pcx,.tga,.lbm"
				if (_strnicmp(bif, ".bmp", 5) == 0 ||
					_strnicmp(bif, ".png", 5) == 0 ||
					_strnicmp(bif, ".jpg", 5) == 0 ||
					_strnicmp(bif, ".jpeg", 5) == 0 ||
					_strnicmp(bif, ".gif", 5) == 0 ||
					_strnicmp(bif, ".pcx", 5) == 0 ||
					_strnicmp(bif, ".tga", 5) == 0 ||
					_strnicmp(bif, ".lbm", 5) == 0 ||
					_strnicmp(bif, ".defs", 7) == 0 
					) {
					// ok.
					isFileDialogActivated = false;
					return true;
				}
				else {
					// unknown extension
					buffer[0] = '\0';
					isFileDialogActivated = false;
					return false;	// cancel
				}

				/*
				char tokenString[3] = { ',', '\n', '\0' };
				std::string sz0 = (char *)&buffer[0];
				std::vector< char *> myVector;
				strtokimplementation2(myVector, sz0, &tokenString[0]);
				int size = myVector.size();
				printf("stack size = %d\n", size);
				for (int i = 0;i < size;i++) {
					char *tokenedstring = myVector[i];
					printf(" -%s- ", tokenedstring);
				}
				printf("\n");
				for (int i = 0;i < (int)myVector.size();i++) {
					myVector.pop_back();
				}
				*/
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Home Dir")) {
			buffer[0] = '\0';
			returnedPath = std::filesystem::current_path().string();
			setHomeDir = true;
			isFileDialogActivated = false;
			return false;	// cancel
		}

		ImGui::SameLine();
		ImGui::LabelText("v","   Working Extensions: .bmp .png .jpg .jpeg .gif .pcx .tga .lbm, .defs");

		//return true;	// standard exit?
		return false;	// standard exit
	}
}//  namespace

