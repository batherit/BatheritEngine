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

	// ������ �޼����. 
	// �̰͵��� ���� �Ķ���� �������� ���õ� Ÿ������ ��ȯ�մϴ�.
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
	// �����Ͱ� ����� ����
	std::ifstream file_;

	// ���� ���ڿ�
	std::string current_line_;

	void GetParameterValueAsString(std::string& line);
	
	std::string GetNextParameter();

	// �̰��� ���� �ܶ� ���ڿ��� ã�� ��� �ּ����� �����Ѵ�.
	std::string GetNextToken();

	// ������ �ۼ��� ������ ��ȿ�ϸ� true�� ��ȯ�Ѵ�.	
	bool is_good_file_;
};

