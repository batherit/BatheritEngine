#pragma once

#include<fstream>
#include<string>
#include<cassert>

class CIniFileLoaderBase
{
public:
	CIniFileLoaderBase(const char* filename) : current_line_(""), is_good_file_(true) {
		file_.open(filename);

		if (!file_) { is_good_file_ = false; }
	}

	// 도움자 메서드들. 
	// 이것들은 다음 파라미터 변수들을 관련된 타입으로 변환합니다.
	double      GetNextParameterDouble() { if (is_good_file_) return atof(GetNextParameter().c_str()); throw std::runtime_error("bad file"); }
	float       GetNextParameterFloat() { if (is_good_file_) return (float)atof(GetNextParameter().c_str()); throw std::runtime_error("bad file"); }
	int         GetNextParameterInt() { if (is_good_file_) return atoi(GetNextParameter().c_str()); throw std::runtime_error("bad file"); }
	bool        GetNextParameterBool() { return (bool)(atoi(GetNextParameter().c_str())); throw std::runtime_error("bad file"); }
	std::string GetNextParameterString() { if (is_good_file_) return GetNextParameter().c_str(); throw std::runtime_error("bad file"); }

	double      GetNextTokenAsDouble() { if (is_good_file_) return atof(GetNextToken().c_str()); throw std::runtime_error("bad file"); }
	float       GetNextTokenAsFloat() { if (is_good_file_) return (float)atof(GetNextToken().c_str()); throw std::runtime_error("bad file"); }
	int         GetNextTokenAsInt() { if (is_good_file_) return atoi(GetNextToken().c_str()); throw std::runtime_error("bad file"); }
	std::string GetNextTokenAsString() { if (is_good_file_) return GetNextToken(); throw std::runtime_error("bad file"); }

	bool        eof()const { if (is_good_file_) return file_.eof(); throw std::runtime_error("bad file"); }
	bool        FileIsGood()const { return is_good_file_; }

private:
	// 데이터가 저장된 파일
	std::ifstream file_;

	// 현재 문자열
	std::string current_line_;

	void GetParameterValueAsString(std::string& line);
	
	std::string GetNextParameter();

	// 이것은 다음 단락 문자열을 찾고 몇가지 주석들을 무시한다.
	std::string GetNextToken();

	// 유저가 작성한 파일이 유효하면 true를 반환한다.	
	bool is_good_file_;
};

