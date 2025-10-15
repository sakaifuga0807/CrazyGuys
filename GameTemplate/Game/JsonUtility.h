#pragma once
#include <string>
#include <fstream>
#include "External/nlohmann/json.hpp"
using json = nlohmann::json;

//Jsonファイルの呼び出しを簡単にするためのクラス。
class JsonUtility : public IGameObject
{
public:
	//Jsonファイルを読み込んでJsonオブジェクトを返す。
	static bool LoadJson(const std::string& filePath, json& outJson)
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			//ファイルが開けなかったら、エラーメッセージを表示。
			std::wstring wpath(filePath.begin(), filePath.end());
			MessageBox(NULL, (L"ファイルが開けません:" + wpath).c_str(), L"JsonUtility",MB_OK);
			return false;
		}
		try
		{
			file >> outJson;
		}
		catch (const json::parse_error& e)
		{
			//パースに失敗したら、エラーメッセージを表示。
			std::wstring wmsg = L"Jsonのパースに失敗しました:\n";
			std::wstring wdetail(e.what(), e.what() + strlen(e.what()));
			MessageBox(NULL, (wmsg + wdetail).c_str(), L"JsonUtility", MB_OK);
			file.close();
			return false;
		}
		file.close();

		return true;
	}
};

